//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_flaregun.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED( TFFlareGun, DT_WeaponFlareGun )

BEGIN_NETWORK_TABLE( CTFFlareGun, DT_WeaponFlareGun )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFFlareGun )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_flaregun, CTFFlareGun );
PRECACHE_WEAPON_REGISTER( tf_weapon_flaregun );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFFlareGun )
END_DATADESC()
#endif

acttable_t CTFFlareGun::m_acttable[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_SECONDARY2, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_SECONDARY2, false },
	{ ACT_MP_RUN, ACT_MP_RUN_SECONDARY2, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_SECONDARY2, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_SECONDARY2, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_SECONDARY2, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_SECONDARY2, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_SECONDARY2, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_SECONDARY2, false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_SECONDARY2, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_SECONDARY2, false },
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFFlareGun::CTFFlareGun()
{
	m_bReloadsSingly = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlareGun::Spawn( void )
{
	BaseClass::Spawn();
}

IMPLEMENT_DM_ACTTABLE( CTFFlareGun );
