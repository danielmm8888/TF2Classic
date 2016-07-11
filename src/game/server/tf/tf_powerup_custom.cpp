//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Critical Damage: Crits for 30/15 seconds with a crit glow. 
//			Crit glow effect would match the player’s merc color or be blue.
//
//=============================================================================//
#include "cbase.h"
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
