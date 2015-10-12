//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: A lightweight minigun variant for use in DM
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_heavyartillery.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFHeavyArtillery, DT_TFHeavyArtillery )
															
BEGIN_NETWORK_TABLE( CTFHeavyArtillery, DT_TFHeavyArtillery )			
END_NETWORK_TABLE()										

BEGIN_PREDICTION_DATA( CTFHeavyArtillery )				
END_PREDICTION_DATA()									
														
LINK_ENTITY_TO_CLASS( tf_weapon_heavyartillery, CTFHeavyArtillery );		
PRECACHE_WEAPON_REGISTER( tf_weapon_heavyartillery );

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFHeavyArtillery::CTFHeavyArtillery()
{
	//WeaponReset();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor.
//-----------------------------------------------------------------------------
CTFHeavyArtillery::~CTFHeavyArtillery()
{
	//WeaponReset();
}
