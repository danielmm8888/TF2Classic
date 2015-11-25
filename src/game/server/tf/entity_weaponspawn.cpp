//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Deathmatch weapon spawning entity.
//
//=============================================================================//
#include "cbase.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_weaponspawn.h"
#include "tf_weaponbase.h"
#include "basecombatcharacter.h"
#include "in_buttons.h"
#include "tf_fx.h"
#include "tf_dropped_weapon.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern CTFWeaponInfo *GetTFWeaponInfo(int iWeapon);

// We don't have a proper sound yet, so we're using this
#define TF_HEALTHKIT_PICKUP_SOUND	"HealthKit.Touch"
//#define RESPAWN_PARTICLE "particlename"

BEGIN_DATADESC(CWeaponSpawner)

	DEFINE_KEYFIELD( m_nWeaponID, FIELD_INTEGER, "WeaponNumber" ),
	DEFINE_KEYFIELD( m_iRespawnTime, FIELD_INTEGER, "RespawnTime" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CWeaponSpawner, DT_WeaponSpawner )
	SendPropBool( SENDINFO( m_bDisabled ) ),
	SendPropBool( SENDINFO( m_bRespawning ) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(tf_weaponspawner, CWeaponSpawner);


CWeaponSpawner::CWeaponSpawner()
{
	m_nWeaponID = TF_WEAPON_SHOTGUN_SOLDIER;
	m_iRespawnTime = 10;
}


//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Spawn(void)
{
	m_pWeaponInfo = GetTFWeaponInfo( m_nWeaponID );
	if ( !m_pWeaponInfo )
	{
		Warning( "tf_weaponspawner has incorrect weapon ID %d\n", m_nWeaponID );
		UTIL_Remove( this );
		return;
	}

	Precache();

	SetModel( m_pWeaponInfo->szWorldModel );
	BaseClass::Spawn();

	// Ensures consistent trigger bounds for all weapons. (danielmm8888)
	SetSolid( SOLID_BBOX );
	SetCollisionBounds( -Vector(22, 22, 15), Vector(22, 22, 15) );

	AddEffects( EF_ITEM_BLINK );
}

float CWeaponSpawner::GetRespawnDelay( void )
{
	return (float)m_iRespawnTime;
}

//-----------------------------------------------------------------------------
// Purpose: Precache function 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Precache(void)
{
	PrecacheScriptSound( TF_HEALTHKIT_PICKUP_SOUND );
	//PrecacheParticleSystem( RESPAWN_PARTICLE );
}


//-----------------------------------------------------------------------------
// Purpose:  Override to get rid of EF_NODRAW
//-----------------------------------------------------------------------------
CBaseEntity* CWeaponSpawner::Respawn( void )
{
	BaseClass::Respawn();
	RemoveEffects( EF_NODRAW );
	RemoveEffects( EF_ITEM_BLINK );
	m_nRenderFX = kRenderFxDistort;
	//m_nRenderMode = kRenderTransColor;
	//SetRenderColor( 246, 232, 99, 128 );
	return this;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Materialize( void )
{
	BaseClass::Materialize();

	if ( !IsDisabled() )
	{
		EmitSound( "Item.Materialize" );
		CPVSFilter filter( GetAbsOrigin() );
		//TE_TFParticleEffect( filter, 0.0f, RESPAWN_PARTICLE, GetAbsOrigin(), QAngle( 0,0,0 ) );
		AddEffects( EF_ITEM_BLINK );
		m_nRenderFX = kRenderFxNone;
		SetRenderColor( 255, 255, 255, 255 );
	}
}

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
void CWeaponSpawner::EndTouch( CBaseEntity *pOther )
{
	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>(pOther);

	if ( ValidTouch( pTFPlayer ) && pTFPlayer->IsPlayerClass( TF_CLASS_MERCENARY ) )
	{
		int iCurrentWeaponID = pTFPlayer->m_Shared.GetDesiredWeaponIndex();
		if ( iCurrentWeaponID == m_nWeaponID )
		{
			pTFPlayer->m_Shared.SetDesiredWeaponIndex( TF_WEAPON_NONE );
		}
	}

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponSpawner::MyTouch(CBasePlayer *pPlayer)
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>(pPlayer);

	if ( ValidTouch( pTFPlayer ) && pTFPlayer->IsPlayerClass( TF_CLASS_MERCENARY ) )
	{
#ifndef DM_WEAPON_BUCKET
		CTFWeaponBase *pWeapon = (CTFWeaponBase *)pTFPlayer->Weapon_GetSlot( m_pWeaponInfo->iSlot );
		const char *pszWeaponName = WeaponIdToClassname( m_nWeaponID );
		int iAmmoType = m_pWeaponInfo->iAmmoType;
		int iMaxAmmo = m_pWeaponInfo->m_WeaponData[TF_WEAPON_PRIMARY_MODE].m_iMaxAmmo;

		if ( pWeapon )
		{
			if ( pWeapon->GetWeaponID() == m_nWeaponID )
			{
				// Weapon spawners give 50% ammo.
				if ( pPlayer->GiveAmmo( ceil( iMaxAmmo * 0.5 ), iAmmoType ) )
					bSuccess = true;
			}
			else if ( !(pTFPlayer->m_nButtons & IN_ATTACK) && 
			(pTFPlayer->m_nButtons & IN_USE || pWeapon->GetWeaponID() == TF_WEAPON_PISTOL) )
			{
				// Spawn a weapon model. - OLD
				//pTFPlayer->DropFakeWeapon(pWeapon);

				// Drop a usable weapon
				pTFPlayer->DropWeapon( pWeapon );

				// Check Use button, always replace pistol
				if ( pWeapon == pTFPlayer->GetActiveTFWeapon() )
				{
					pWeapon->Holster();
				}
				pTFPlayer->Weapon_Detach( pWeapon );
				UTIL_Remove( pWeapon );
				pWeapon = NULL;
			}
			else
			{
				pTFPlayer->m_Shared.SetDesiredWeaponIndex( m_nWeaponID );
			}
		}
#else
		CTFWeaponBase *pWeapon = pTFPlayer->Weapon_OwnsThisID( m_nWeaponID );
		const char *pszWeaponName = WeaponIdToClassname( m_nWeaponID );

		if ( pWeapon )
		{
			if ( pPlayer->GiveAmmo(999, m_pWeaponInfo->iAmmoType) )
				bSuccess = true;
		}
#endif

		if ( !pWeapon )
		{
			pTFPlayer->GiveNamedItem( pszWeaponName );
			pTFPlayer->SetAmmoCount( iMaxAmmo, iAmmoType );
			pTFPlayer->m_Shared.SetDesiredWeaponIndex( TF_WEAPON_NONE );
			bSuccess = true;
		}

		if ( bSuccess )
		{
			CSingleUserRecipientFilter user( pPlayer );
			user.MakeReliable();

			UserMessageBegin( user, "ItemPickup" );
			WRITE_STRING( GetClassname() );
			MessageEnd();

			//EmitSound(user, entindex(), TF_HEALTHKIT_PICKUP_SOUND);
		}
	}

	return bSuccess;
}
