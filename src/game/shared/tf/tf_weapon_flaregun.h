//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======
//
// Purpose: A remake of Pyro's Flare Gun from live TF2
//
//=============================================================================
#ifndef TF_WEAPON_FLAREGUN_H
#define TF_WEAPON_FLAREGUN_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

#ifdef CLIENT_DLL
#define CTFFlareGun C_TFFlareGun
#endif

class CTFFlareGun : public CTFWeaponBaseGun
{
public:
	DECLARE_CLASS( CTFFlareGun, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFFlareGun();

	virtual void	Spawn( void );
	virtual int		GetWeaponID( void ) const { return TF_WEAPON_FLAREGUN; }

	DECLARE_DM_ACTTABLE();
};

#endif // TF_WEAPON_FLAREGUN_H
