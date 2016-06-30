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

/*
Old FGD info.

16 : "Sniper rifle ( Primary )"
17 : "Minigun ( Primary )"
21 : "Rocket launcher ( Primary )"
22 : "Pipebomb launcher ( Primary )"
24 : "Flamethrower ( Primary )"
67 : "Sten Gun ( Primary )"
71 : "Heavy Artillery ( Primary )"
68 : "Double barrel shotgun ( Primary )"
12 : "Shotgun ( Secondary )"
18 : "SMG ( Secondary )"
37 : "Pistol ( Secondary )"
69 : "Six Shooter ( Secondary )"
5 : "Crowbar ( Melee )"
*/

struct WeaponTranslation_t
{
	int iWeaponID;
	int iItemID;
};

// This is a temporary translation table from weapon IDs to Econ Item IDs.
// We need to eventually remove this once all DM maps are updated.
static WeaponTranslation_t g_aWeaponTranslations[] =
{
	{ 5, 9010 },
	{ 12, 10 },
	{ 15, 13 },
	{ 16, 14 },
	{ 17, 15 },
	{ 18, 16 },
	{ 21, 18 },
	{ 22, 19 },
	{ 24, 21 },
	{ 37, 22 },
	{ 65, 39 },
	{ 67, 9011 },
	{ 68, 9012 },
	{ 69, 9013 },
	{ 71, 9014 }
};

//#define RESPAWN_PARTICLE "particlename"

BEGIN_DATADESC( CWeaponSpawner )
	DEFINE_KEYFIELD( m_nWeaponID, FIELD_INTEGER, "WeaponNumber" ),
	DEFINE_KEYFIELD( m_nItemID, FIELD_INTEGER, "itemid" ),
	DEFINE_KEYFIELD( m_flRespawnTime, FIELD_FLOAT, "RespawnTime" ),
	DEFINE_KEYFIELD( m_bStaticSpawner, FIELD_BOOLEAN, "StaticSpawner" ),
	DEFINE_KEYFIELD( m_bOutlineDisabled, FIELD_BOOLEAN, "DisableWeaponOutline" ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CWeaponSpawner, DT_WeaponSpawner )
	SendPropBool( SENDINFO( m_bDisabled ) ),
	SendPropBool( SENDINFO( m_bRespawning ) ),
	SendPropBool( SENDINFO( m_bStaticSpawner ) ),
	SendPropBool( SENDINFO( m_bOutlineDisabled ) ),
	SendPropTime( SENDINFO( m_flRespawnTime ) ),
	SendPropTime( SENDINFO( m_flRespawnAtTime ) ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tf_weaponspawner, CWeaponSpawner );


CWeaponSpawner::CWeaponSpawner()
{
	m_nWeaponID = TF_WEAPON_NONE;
	m_nItemID = -1;
	m_flRespawnTime = 10.0f;
	m_bStaticSpawner = false;
	m_bOutlineDisabled = false;
	m_flRespawnAtTime = 0.0f;
}


//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Spawn( void )
{
	CEconItemDefinition *pItemDef = GetItemSchema()->GetItemDefinition( m_nItemID );
	if ( !pItemDef )
	{
		Warning( "tf_weaponspawner has incorrect item ID %d. DELETED\n", m_nItemID );
		UTIL_Remove( this );
		return;
	}

	m_Item.SetItemDefIndex( m_nItemID );

	Precache();

	SetModel( m_Item.GetWorldDisplayModel() );

	BaseClass::Spawn();

	// Ensures consistent trigger bounds for all weapons. (danielmm8888)
	SetSolid( SOLID_BBOX );
	SetCollisionBounds( -Vector( 22, 22, 15 ), Vector( 22, 22, 15 ) );

	AddEffects( EF_ITEM_BLINK );
}

float CWeaponSpawner::GetRespawnDelay( void )
{
	return m_flRespawnTime;
}

//-----------------------------------------------------------------------------
// Purpose: Precache function 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Precache( void )
{
	PrecacheModel( m_Item.GetWorldDisplayModel() );
	//PrecacheParticleSystem( RESPAWN_PARTICLE );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CWeaponSpawner::KeyValue( const char *szKeyName, const char *szValue )
{
	if ( FStrEq( szKeyName, "WeaponNumber" ) )
	{
		int iInputID = atoi( szValue );

		if ( iInputID == 0 )
			return true;

		Warning( "tf_weaponspawner is using obsolete keyvalue \"WeaponNumber\"! Remove it and use the new key \"itemid\" (item ID from schema).\n" );

		for ( int i = 0; i < ARRAYSIZE( g_aWeaponTranslations ); i++ )
		{
			if ( g_aWeaponTranslations[i].iWeaponID == iInputID )
			{
				Warning( "Weapon ID %d corresponds to item ID %d.\n", iInputID, g_aWeaponTranslations[i].iItemID );
				m_nItemID = g_aWeaponTranslations[i].iItemID;
			}
		}

		return true;
	}

	return BaseClass::KeyValue( szKeyName, szValue );
}

//-----------------------------------------------------------------------------
// Purpose:  Override to get rid of EF_NODRAW
//-----------------------------------------------------------------------------
CBaseEntity* CWeaponSpawner::Respawn( void )
{
	CBaseEntity *pRet = BaseClass::Respawn();

	RemoveEffects( EF_NODRAW );
	RemoveEffects( EF_ITEM_BLINK );
	m_nRenderFX = kRenderFxDistort;
	//m_nRenderMode = kRenderTransColor;
	//SetRenderColor( 246, 232, 99, 128 );

	m_flRespawnAtTime = GetNextThink();

	return pRet;
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
	}
}

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
void CWeaponSpawner::EndTouch( CBaseEntity *pOther )
{
	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>( pOther );

	if ( ValidTouch( pTFPlayer ) && pTFPlayer->IsPlayerClass( TF_CLASS_MERCENARY ) )
	{
		int iCurrentWeaponID = pTFPlayer->m_Shared.GetDesiredWeaponIndex();
		if ( iCurrentWeaponID == m_nItemID )
		{
			pTFPlayer->m_Shared.SetDesiredWeaponIndex( -1 );
		}
	}

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponSpawner::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );

	if ( ValidTouch( pTFPlayer ) && pTFPlayer->IsPlayerClass( TF_CLASS_MERCENARY ) )
	{
#ifndef DM_WEAPON_BUCKET
		int iSlot = m_Item.GetStaticData()->GetLoadoutSlot( TF_CLASS_MERCENARY );
		CTFWeaponBase *pWeapon = (CTFWeaponBase *)pTFPlayer->GetEntityForLoadoutSlot( iSlot );

		if ( pWeapon )
		{
			if ( pTFPlayer->ItemsMatch( pWeapon->GetItem(), &m_Item, pWeapon ) )
			{
				if ( pTFPlayer->GiveAmmo( pWeapon->GetInitialAmmo(), pWeapon->GetPrimaryAmmoType(), true, TF_AMMO_SOURCE_AMMOPACK ) )
					bSuccess = true;
			}
			else if ( !( pTFPlayer->m_nButtons & IN_ATTACK ) &&
				( pTFPlayer->m_nButtons & IN_USE ||
				( TFGameRules()->IsDeathmatch() && pWeapon->GetWeaponID() == TF_WEAPON_PISTOL ) ) ) // Check Use button, always replace pistol.
			{
				// Drop a usable weapon
				pTFPlayer->DropWeapon( pWeapon );

				pWeapon->UnEquip( pTFPlayer );
				pWeapon = NULL;
			}
			else
			{
				pTFPlayer->m_Shared.SetDesiredWeaponIndex( m_nItemID );
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
			const char *pszWeaponName = m_Item.GetEntityName();
			CTFWeaponBase *pNewWeapon = (CTFWeaponBase *)pTFPlayer->GiveNamedItem( pszWeaponName, 0, &m_Item );

			if ( pNewWeapon )
			{
				pPlayer->SetAmmoCount( pNewWeapon->GetInitialAmmo(), pNewWeapon->GetPrimaryAmmoType() );
				pNewWeapon->GiveTo( pPlayer );
				pTFPlayer->m_Shared.SetDesiredWeaponIndex( -1 );
				bSuccess = true;
			}
		}

		if ( bSuccess )
		{
			CSingleUserRecipientFilter user( pPlayer );
			user.MakeReliable();

			UserMessageBegin( user, "ItemPickup" );
				WRITE_STRING( GetClassname() );
			MessageEnd();

			pPlayer->EmitSound( "BaseCombatCharacter.AmmoPickup" );
		}
	}

	return bSuccess;
}
