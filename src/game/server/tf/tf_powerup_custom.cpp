//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Critical Damage: Crits for 30/15 seconds with a crit glow. 
//			Crit glow effect would match the player’s merc color or be blue.
//
//=============================================================================//
#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "tf_powerup_custom.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFPowerupCustom )

	DEFINE_KEYFIELD( m_iPowerupCondition, FIELD_INTEGER, "PowerupCondition" ),
	DEFINE_KEYFIELD( m_flEffectDuration, FIELD_FLOAT, "EffectDuration" ),

END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_custom, CTFPowerupCustom );

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFPowerupCustom::CTFPowerupCustom()
{	
	m_iPowerupCondition = TF_COND_POWERUP_CRITDAMAGE;
	m_flEffectDuration = 15.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFPowerupCustom::Spawn( void )
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache 
//-----------------------------------------------------------------------------
void CTFPowerupCustom::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFPowerupCustom::MyTouch( CBasePlayer *pPlayer )
{
	return BaseClass::MyTouch( pPlayer );
}
