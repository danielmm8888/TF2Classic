//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Critical Damage: Crits for 30/15 seconds with a crit glow. 
//			Crit glow effect would match the player’s merc color or be blue.
//
//=============================================================================//
#include "cbase.h"
#include "tf_powerup_critdamage.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFPowerupCritdamage )
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_critdamage, CTFPowerupCritdamage );

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFPowerupCritdamage::CTFPowerupCritdamage()
{
	m_flEffectDuration = 15.0f;
}
