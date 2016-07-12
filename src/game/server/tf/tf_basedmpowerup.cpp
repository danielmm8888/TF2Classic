//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Base class for Deathmatch powerups 
//
//=============================================================================//

#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "tf_basedmpowerup.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFBaseDMPowerup )
	DEFINE_KEYFIELD( m_strPickupSound, FIELD_SOUNDNAME, "PickupSound" ),
	DEFINE_KEYFIELD( m_flRespawnTime, FIELD_FLOAT, "RespawnTime" ),
END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CTFBaseDMPowerup, DT_TFBaseDMPowerup )
	SendPropBool( SENDINFO( m_bRespawning ) ),
	SendPropTime( SENDINFO( m_flRespawnTime ) ),
	SendPropTime( SENDINFO( m_flRespawnAtTime ) ),
END_SEND_TABLE()

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFBaseDMPowerup::CTFBaseDMPowerup()
{
	m_flRespawnTime = 30.0f;
}

CTFBaseDMPowerup *CTFBaseDMPowerup::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, const char *pszClassname, float flDuration )
{
	CTFBaseDMPowerup *pPowerup = dynamic_cast<CTFBaseDMPowerup *>( CBaseEntity::CreateNoSpawn( pszClassname, vecOrigin, vecAngles, pOwner ) );

	if ( pPowerup )
	{
		pPowerup->SetEffectDuration( flDuration );
		pPowerup->AddSpawnFlags( SF_NORESPAWN );

		DispatchSpawn( pPowerup );

		pPowerup->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );

		pPowerup->SetThink( &CBaseEntity::SUB_Remove );
		pPowerup->SetNextThink( gpGlobals->curtime + 30.0f );
	}

	return pPowerup;
}

//-----------------------------------------------------------------------------
// Purpose: Precache 
//-----------------------------------------------------------------------------
void CTFBaseDMPowerup::Precache( void )
{
	UTIL_ValidateSoundName( m_strPickupSound, GetDefaultPickupSound() );
	if ( GetModelName() == NULL_STRING )
		SetModelName( AllocPooledString( GetDefaultPowerupModel() ) );

	PrecacheModel( STRING( GetModelName() ) );
	PrecacheScriptSound( STRING( m_strPickupSound ) );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFBaseDMPowerup::Spawn( void )
{
	Precache();
	SetModel( STRING( GetModelName() ) );
	SetRenderMode( kRenderTransColor );

	BaseClass::Spawn();

	AddEffects( EF_ITEM_BLINK );
}

//-----------------------------------------------------------------------------
// Purpose:  Override to get rid of EF_NODRAW
//-----------------------------------------------------------------------------
CBaseEntity* CTFBaseDMPowerup::Respawn( void )
{
	CBaseEntity *pRet = BaseClass::Respawn();

	RemoveEffects( EF_NODRAW );
	RemoveEffects( EF_ITEM_BLINK );
	SetRenderColorA( 80 );

	m_flRespawnAtTime = GetNextThink();

	return pRet;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBaseDMPowerup::Materialize( void )
{
	BaseClass::Materialize();

	if ( !IsDisabled() )
	{
		EmitSound( "Item.Materialize" );
		AddEffects( EF_ITEM_BLINK );
		SetRenderColorA( 255 );
	}
}

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
float CTFBaseDMPowerup::GetRespawnDelay( void )
{
	return m_flRespawnTime;
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFBaseDMPowerup::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
	if ( pTFPlayer && ValidTouch( pPlayer ) )
	{
		// Add the condition and duration from derived classes
		pTFPlayer->m_Shared.AddCond( GetCondition(), GetEffectDuration() );

		// Give full health
		int iHealthRestored = pTFPlayer->TakeHealth( pTFPlayer->GetMaxHealth(), DMG_GENERIC );

		if ( iHealthRestored )
		{
			IGameEvent *event_healonhit = gameeventmanager->CreateEvent( "player_healonhit" );
			if ( event_healonhit )
			{
				event_healonhit->SetInt( "amount", iHealthRestored );
				event_healonhit->SetInt( "entindex", pTFPlayer->entindex() );

				gameeventmanager->FireEvent( event_healonhit );
			}
		}

		CSingleUserRecipientFilter user( pTFPlayer );
		user.MakeReliable();

		UserMessageBegin( user, "ItemPickup" );
			WRITE_STRING( GetClassname() );
		MessageEnd();

		if ( m_strPickupSound != NULL_STRING )
			pTFPlayer->EmitSound( STRING( m_strPickupSound ) );

		bSuccess = true;
	}

	return bSuccess;
}
