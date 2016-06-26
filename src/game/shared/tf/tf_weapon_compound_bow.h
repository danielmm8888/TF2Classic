//=============================================================================
//
// Purpose: A remake of Huntsman from live TF2.
//
//=============================================================================
#ifndef TF_WEAPON_COMPOUND_BOW_H
#define TF_WEAPON_COMPOUND_BOW_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

#ifdef CLIENT_DLL
#define CTFCompoundBow C_TFCompoundBow
#endif

class CTFCompoundBow : public CTFWeaponBaseGun
{
public:
	DECLARE_CLASS( CTFCompoundBow, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFCompoundBow();

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_COMPOUND_BOW; }

	virtual void	Precache( void );
};

#endif // TF_WEAPON_COMPOUND_BOW_H
