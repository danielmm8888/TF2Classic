//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_ubersaw.h"

//=============================================================================
//
// Weapon Ubersaw tables.
//

IMPLEMENT_NETWORKCLASS_ALIASED( TFUbersaw, DT_TFWeaponUbersaw )

BEGIN_NETWORK_TABLE( CTFUbersaw, DT_TFWeaponUbersaw )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFUbersaw )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_ubersaw, CTFUbersaw );
PRECACHE_WEAPON_REGISTER( tf_weapon_ubersaw );