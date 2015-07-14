//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_UBERSAW_H
#define TF_WEAPON_UBERSAW_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFUbersaw C_TFUbersaw
#endif

//=============================================================================
//
// Ubersaw class.
//
class CTFUbersaw : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFUbersaw, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFUbersaw() {}
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_UBERSAW; }

private:

	CTFUbersaw( const CTFUbersaw & ) {}
};

#endif // TF_WEAPON_UBERSAW_H
