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

	DEFINE_KEYFIELD( m_strModelName, FIELD_STRING, "ModelName" ),
	DEFINE_KEYFIELD( m_strPickupSound, FIELD_SOUNDNAME, "PickupSound" ),
	DEFINE_KEYFIELD( m_iRespawnTime, FIELD_INTEGER, "RespawnTime" ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CTFBaseDMPowerup, DT_TFBaseDMPowerup )
END_SEND_TABLE()

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFBaseDMPowerup::CTFBaseDMPowerup()
{
	m_iRespawnTime = 0;
	m_strModelName = MAKE_STRING("models/class_menu/random_class_icon.mdl");
	m_strPickupSound = MAKE_STRING( "HealthKit.Touch" );
}

//-----------------------------------------------------------------------------
// Purpose: Precache 
//-----------------------------------------------------------------------------
void CTFBaseDMPowerup::Precache( void )
{
	PrecacheModel( GetPowerupModel() );	
	PrecacheScriptSound( GetPickupSound() );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFBaseDMPowerup::Spawn( void )
{
	Precache();
	SetModel( GetPowerupModel() );

	BaseClass::Spawn();

	AddEffects( EF_ITEM_BLINK );
}

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
float CTFBaseDMPowerup::GetRespawnDelay( void )
{
	return (float)m_iRespawnTime;
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

		EmitSound( user, entindex(), GetPickupSound() );

		bSuccess = true;
	}

	return bSuccess;
}
