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

#define TF_GRENADE_LAUNCHER_XBOX_CLIP 4

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

	// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFGrenadeLauncher();
	~CTFGrenadeLauncher();

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_GRENADELAUNCHER; }
	virtual void	Spawn( void );
	virtual float	GetProjectileSpeed( void );

public:

	void LaunchGrenade( void );

private:

	CTFGrenadeLauncher( const CTFGrenadeLauncher & ) {}
};

#endif // TF_WEAPON_GRENADELAUNCHER_H