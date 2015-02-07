//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_fishwhacker.h"
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
IMPLEMENT_NETWORKCLASS_ALIASED( TFFishWhacker, DT_TFWeaponFishWhacker )

BEGIN_NETWORK_TABLE( CTFFishWhacker, DT_TFWeaponFishWhacker )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFFishWhacker )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_fishwhacker, CTFFishWhacker );
PRECACHE_WEAPON_REGISTER( tf_weapon_fishwhacker );

//=============================================================================
//
// Weapon FishWhacker functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFFishWhacker::CTFFishWhacker()
{
}
