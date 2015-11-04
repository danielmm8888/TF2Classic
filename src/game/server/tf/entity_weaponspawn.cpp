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

	DEFINE_KEYFIELD(m_iWeaponNumber, FIELD_INTEGER, "WeaponNumber"),
	DEFINE_KEYFIELD(m_iRespawnTime, FIELD_INTEGER, "RespawnTime"),

END_DATADESC()

//-----------------------------------------------------------------------------
// Purpose: SendProxy that converts the UtlVector list of players in range to entindexes, where it's reassembled on the client
//-----------------------------------------------------------------------------
void SendProxy_NearbyPlayerList( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID )
{
	CWeaponSpawner *pSpawner = (CWeaponSpawner*)pStruct;

	// If this assertion fails, then SendProxyArrayLength_PlayerList has failed.
	Assert( iElement < pSpawner->m_hNearbyPlayers.Size() );

	CBaseEntity *pEnt = pSpawner->m_hNearbyPlayers[iElement].Get();
	EHANDLE hOther = pEnt;

	SendProxy_EHandleToInt( pProp, pStruct, &hOther, pOut, iElement, objectID );
}

int SendProxyArrayLength_NearbyPlayerList( const void *pStruct, int objectID )
{
	CWeaponSpawner *pSpawner = (CWeaponSpawner*)pStruct;
	return pSpawner->m_hNearbyPlayers.Count();
}

IMPLEMENT_SERVERCLASS_ST(CWeaponSpawner, DT_WeaponSpawner)
	SendPropBool( SENDINFO( m_bInactive ) ),
	SendPropArray2( 
		SendProxyArrayLength_NearbyPlayerList,
		SendPropInt( "nearby_player_list_element", 0, SIZEOF_IGNORE, NUM_NETWORKED_EHANDLE_BITS, SPROP_UNSIGNED, SendProxy_NearbyPlayerList ), 
		MAX_PLAYERS, 
		0, 
		"nearby_player_list"
		),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(tf_weaponspawner, CWeaponSpawner);


CWeaponSpawner::CWeaponSpawner()
{
	m_iWeaponNumber = TF_WEAPON_SHOTGUN_SOLDIER;
	m_iRespawnTime = 10;
}


//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Spawn(void)
{
	pWeaponInfo = GetTFWeaponInfo(m_iWeaponNumber);
	if ( !pWeaponInfo )
	{
		Warning( "tf_weaponspawner has incorrect weapon number %d \n", m_iWeaponNumber );
		UTIL_Remove( this );
		return;
	}

	Precache();

	SetModel(pWeaponInfo->szWorldModel);
	BaseClass::Spawn();

	// Ensures consistent BBOX size for all weapons. (danielmm8888)
	SetSolid( SOLID_BBOX );
	SetCollisionBounds( -Vector(22, 22, 15), Vector(22, 22, 15) );

	AddEffects( EF_ITEM_BLINK );

	SetThink( &CWeaponSpawner::GlowThink );
	SetNextThink( gpGlobals->curtime );
}

void CWeaponSpawner::GlowThink( void )
{
	// Get a list of all the players that are within a 128 unit radius
	m_hNearbyPlayers.Purge();

	CBaseEntity *pEntityArray [ MAX_PLAYERS ];
	Vector vecOrigin = GetAbsOrigin();
	static float flRadius = 128.0f;

	int iNearbyPlayers = UTIL_EntitiesInSphere( pEntityArray, ARRAYSIZE(pEntityArray), vecOrigin, flRadius, FL_CLIENT );

	for ( int i = 0; i < iNearbyPlayers; i++ )
	{
		m_hNearbyPlayers.AddToTail( pEntityArray[i] );
	}
	
	SetNextThink( gpGlobals->curtime );
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
	m_bInactive = true;
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
		m_bInactive = false;
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
		if ( iCurrentWeaponID == m_iWeaponNumber )
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
		CTFWeaponBase *pWeapon = pTFPlayer->Weapon_GetWeaponByBucket( pWeaponInfo->iSlot );
		const char *pszWeaponName = WeaponIdToAlias( m_iWeaponNumber );

		if ( pWeapon )
		{
			if ( pWeapon->GetWeaponID() == m_iWeaponNumber )
			{
				if ( pPlayer->GiveAmmo(999, pWeaponInfo->iAmmoType) )
					bSuccess = true;
			}
			else if ( !(pTFPlayer->m_nButtons & IN_ATTACK) && 
			(pTFPlayer->m_nButtons & IN_USE || pWeapon->GetWeaponID() == TF_WEAPON_PISTOL) )
			{
				// Spawn a weapon model. - OLD
				//pTFPlayer->DropFakeWeapon(pWeapon);

				// Drop a usable weapon
				Vector vecOrigin;
				QAngle vecAngles;
				pTFPlayer->CalculateAmmoPackPositionAndAngles( pWeapon, vecOrigin, vecAngles );

				if ( pWeapon->GetWeaponID() != TF_WEAPON_PISTOL )
					CTFDroppedWeapon::Create( vecOrigin, vecAngles, pWeapon->GetWorldModel(), pWeapon->GetWeaponID() );

				// Check Use button, always replace pistol
				pTFPlayer->Weapon_Detach( pWeapon );
				pWeapon->WeaponReset();
				UTIL_Remove(pWeapon);
				pWeapon = NULL;
			}
			else
			{
				pTFPlayer->m_Shared.SetDesiredWeaponIndex( m_iWeaponNumber );
			}
		}
#else
		CTFWeaponBase *pWeapon = pTFPlayer->Weapon_OwnsThisID( m_iWeaponNumber );
		const char *pszWeaponName = WeaponIdToAlias( m_iWeaponNumber );

		if ( pWeapon && pWeapon->GetWeaponID() == m_iWeaponNumber )
		{
			if ( pWeapon->GetWeaponID() == m_iWeaponNumber )
			{
				if ( pPlayer->GiveAmmo(999, pWeaponInfo->iAmmoType) )
					bSuccess = true;
			}
		}
#endif

		if ( !pWeapon )
		{
			pTFPlayer->GiveNamedItem( pszWeaponName );
			pTFPlayer->m_Shared.SetDesiredWeaponIndex( TF_WEAPON_NONE );
			bSuccess = true;
		}

		if ( bSuccess )
		{
			CSingleUserRecipientFilter user(pPlayer);
			user.MakeReliable();

			UserMessageBegin(user, "ItemPickup");
			WRITE_STRING(GetClassname());
			MessageEnd();

			//EmitSound(user, entindex(), TF_HEALTHKIT_PICKUP_SOUND);
		}
	}

	return bSuccess;
}
