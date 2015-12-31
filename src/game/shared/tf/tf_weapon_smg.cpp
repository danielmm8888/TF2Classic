//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_smg.h"

//=============================================================================
//
// Weapon SMG tables.
//
#define CREATE_SIMPLE_WEAPON_TABLE( WpnName, entityname )			\
																	\
	IMPLEMENT_NETWORKCLASS_ALIASED( WpnName, DT_##WpnName )	\
															\
	BEGIN_NETWORK_TABLE( C##WpnName, DT_##WpnName )			\
	END_NETWORK_TABLE()										\
															\
	BEGIN_PREDICTION_DATA( C##WpnName )						\
	END_PREDICTION_DATA()									\
															\
	LINK_ENTITY_TO_CLASS( entityname, C##WpnName );			\
	PRECACHE_WEAPON_REGISTER( entityname );


CREATE_SIMPLE_WEAPON_TABLE( TFSMG, tf_weapon_smg )
CREATE_SIMPLE_WEAPON_TABLE( TFSMG_Primary, tf_weapon_smg_primary )

// Server specific.
//#ifndef CLIENT_DLL
//BEGIN_DATADESC( CTFSMG )
//END_DATADESC()
//#endif

//=============================================================================
//
// Weapon SMG functions.
//