//====== Copyright ? 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_umbrella.h"
#include "decals.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Club tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED(TFUmbrella, DT_TFWeaponUmbrella)

BEGIN_NETWORK_TABLE(CTFUmbrella, DT_TFWeaponUmbrella)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CTFUmbrella)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(tf_weapon_umbrella, CTFUmbrella);
PRECACHE_WEAPON_REGISTER(tf_weapon_umbrella);

//=============================================================================
//
// Weapon FishWhacker functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFUmbrella::CTFUmbrella()
{
}