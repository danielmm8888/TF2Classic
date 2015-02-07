//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_FISHWHACKER_H
#define TF_WEAPON_FISHWHACKER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFFishWhacker C_TFFishWhacker
#endif

//=============================================================================
//
// Club class.
//
class CTFFishWhacker : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS(CTFFishWhacker, CTFWeaponBaseMelee);
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFFishWhacker();
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_FISHWHACKER; }

private:

	CTFFishWhacker(const CTFFishWhacker &) {}
};

#endif // TF_WEAPON_FISHWHACKER_H
