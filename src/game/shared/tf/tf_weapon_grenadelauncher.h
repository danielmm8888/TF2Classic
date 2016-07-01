//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#ifndef TF_WEAPON_GRENADELAUNCHER_H
#define TF_WEAPON_GRENADELAUNCHER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weaponbase_grenadeproj.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFGrenadeLauncher C_TFGrenadeLauncher
#endif

//=============================================================================
//
// TF Weapon Grenade Launcher.
//
class CTFGrenadeLauncher : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFGrenadeLauncher, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFGrenadeLauncher();
	~CTFGrenadeLauncher();

	virtual void	Spawn( void );
	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_GRENADELAUNCHER; }
	virtual float	GetProjectileSpeed( void );

private:
	CTFGrenadeLauncher( const CTFGrenadeLauncher & ) {}
};

#endif // TF_WEAPON_GRENADELAUNCHER_H