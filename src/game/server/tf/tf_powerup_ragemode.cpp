//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: The ragemode powerup (name is a W.I.P). Equips the mercenary with
//			the Hammerfist weapon for a short period of time.
//
//=============================================================================//

#include "cbase.h"
#include "tf_powerup_ragemode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFPowerupRagemode )
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_ragemode, CTFPowerupRagemode );

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFPowerupRagemode::CTFPowerupRagemode()
{
	m_flEffectDuration = 15.0f;
}
