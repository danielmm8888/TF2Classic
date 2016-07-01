//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_grenadelauncher.h"

//=============================================================================
//
// Weapon Grenade Launcher tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFGrenadeLauncher, tf_weapon_grenadelauncher )

//=============================================================================

#define TF_GRENADE_LAUNCER_VEL 1200

//=============================================================================
//
// Weapon Grenade Launcher functions.
//

CTFGrenadeLauncher::CTFGrenadeLauncher()
{
	m_bReloadsSingly = true;
}

CTFGrenadeLauncher::~CTFGrenadeLauncher()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeLauncher::Spawn( void )
{
	m_iAltFireHint = HINT_ALTFIRE_GRENADELAUNCHER;
	BaseClass::Spawn();
}

float CTFGrenadeLauncher::GetProjectileSpeed( void )
{
	float flVelocity = TF_GRENADE_LAUNCER_VEL;
	CALL_ATTRIB_HOOK_FLOAT( flVelocity, mult_projectile_speed );
	return flVelocity;
}
