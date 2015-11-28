//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
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
#include "tf_gamestats.h"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED(TFFlareGun, DT_WeaponFlareGun)

BEGIN_NETWORK_TABLE(CTFFlareGun, DT_WeaponFlareGun)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CTFFlareGun)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(tf_weapon_flaregun, CTFFlareGun);
PRECACHE_WEAPON_REGISTER(tf_weapon_flaregun);

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC(CTFFlareGun)
END_DATADESC()
#endif

#define TF_FLARE_MIN_VEL 1200

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
void CTFFlareGun::Spawn(void)
{
	BaseClass::Spawn();
}

