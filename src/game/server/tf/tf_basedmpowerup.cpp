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

		pPowerup->RemoveSolidFlags( FSOLID_NOT_SOLID );
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

	BaseClass::Spawn();

	AddEffects( EF_ITEM_BLINK );
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

	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>( pPlayer );
	if  ( pTFPlayer && ValidTouch( pPlayer ) )
	{
		//Add the condition and duration from derived classes
		pTFPlayer->m_Shared.AddCond( GetCondition(), GetEffectDuration() );
		
		//Give full health
		SetHealth( GetMaxHealth() );

		CSingleUserRecipientFilter user( pPlayer );
		user.MakeReliable();

		UserMessageBegin( user, "ItemPickup" );
		WRITE_STRING( GetClassname() );
		MessageEnd();

		pPlayer->EmitSound( STRING( m_strPickupSound ) );

		bSuccess = true;
	}

	return bSuccess;
}
