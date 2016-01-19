//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: The Speedshooter powerup. A powerup which increases your firing and
//			reload time for a certain (yet undetermined) period of time
//
//=============================================================================//
#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "tf_powerup_speedshooter.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFPowerupSpeedshooter )
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_speedshooter, CTFPowerupSpeedshooter );

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFPowerupSpeedshooter::CTFPowerupSpeedshooter()
{
	m_strModelName = MAKE_STRING( "models/class_menu/random_class_icon.mdl" );
	m_strPickupSound = MAKE_STRING( "HealthKit.Touch" );
	m_iRespawnTime = 30;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFPowerupSpeedshooter::Spawn( void )
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache 
//-----------------------------------------------------------------------------
void CTFPowerupSpeedshooter::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFPowerupSpeedshooter::MyTouch( CBasePlayer *pPlayer )
{
	return BaseClass::MyTouch( pPlayer );
}
