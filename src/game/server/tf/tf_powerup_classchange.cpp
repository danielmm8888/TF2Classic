//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Lets you join a specified class for a short duration of the time
//
//=============================================================================//

#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "tf_powerup_classchange.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

BEGIN_DATADESC( CTFPowerupClasschange )
	DEFINE_KEYFIELD( m_nClassIndex, FIELD_INTEGER, "classindex" )
END_DATADESC()

LINK_ENTITY_TO_CLASS( item_powerup_classchange, CTFPowerupClasschange );

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Constructor 
//-----------------------------------------------------------------------------
CTFPowerupClasschange::CTFPowerupClasschange()
{	
	m_nClassIndex = TF_CLASS_SCOUT;
	m_flEffectDuration = 15.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFPowerupClasschange::Spawn( void )
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache 
//-----------------------------------------------------------------------------
void CTFPowerupClasschange::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFPowerupClasschange::MyTouch( CBasePlayer *pPlayer )
{
	return BaseClass::MyTouch( pPlayer );
}
