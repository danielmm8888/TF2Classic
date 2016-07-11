//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: FILL IN
//
//=============================================================================//
#include "cbase.h"
#include "tf_powerup_shield.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFPowerupShield )
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_shield, CTFPowerupShield );
// For backwards compatibility.
LINK_ENTITY_TO_CLASS( item_powerup_shortuber, CTFPowerupShield );

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFPowerupShield::CTFPowerupShield()
{
	m_flEffectDuration = 15.0f;
}
