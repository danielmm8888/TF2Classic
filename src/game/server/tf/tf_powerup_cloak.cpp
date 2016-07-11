//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: FILL IN
//
//=============================================================================//
#include "cbase.h"
#include "tf_powerup_cloak.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFPowerupCloak )
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_cloak, CTFPowerupCloak );

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFPowerupCloak::CTFPowerupCloak()
{
	m_flEffectDuration = 15.0f;
}
