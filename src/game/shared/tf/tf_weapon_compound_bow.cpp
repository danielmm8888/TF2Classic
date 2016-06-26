//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_compound_bow.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED( TFCompoundBow, DT_WeaponCompoundBow )

BEGIN_NETWORK_TABLE( CTFCompoundBow, DT_WeaponCompoundBow )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCompoundBow )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_compound_bow, CTFCompoundBow );
PRECACHE_WEAPON_REGISTER( tf_weapon_compound_bow );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFCompoundBow )
END_DATADESC()
#endif

CTFCompoundBow::CTFCompoundBow()
{
}

void CTFCompoundBow::Precache( void )
{
	BaseClass::Precache();

	PrecacheScriptSound( "Weapon_CompoundBow.SinglePull" );
	PrecacheScriptSound( "ArrowLight" );
}
