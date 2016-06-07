//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: The ragemode powerup (name is a W.I.P). Equips the mercenary with
//			the Hammerfist weapon for a short period of time.
//
//=============================================================================//

#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
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

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFPowerupRagemode::Spawn( void )
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache 
//-----------------------------------------------------------------------------
void CTFPowerupRagemode::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFPowerupRagemode::MyTouch( CBasePlayer *pPlayer )
{
	return BaseClass::MyTouch( pPlayer );
}
