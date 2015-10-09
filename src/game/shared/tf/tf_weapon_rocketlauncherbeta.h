//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket Launcher
//
//=============================================================================
#ifndef TF_WEAPON_ROCKETLAUNCHERBETA_H
#define TF_WEAPON_ROCKETLAUNCHERBETA_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weaponbase_rocket.h"
#include "tf_weapon_rocketlauncher.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFRocketLauncherBeta C_TFRocketLauncherBeta
#endif

//=============================================================================
//
// TF Weapon Rocket Launcher.
//
class CTFRocketLauncherBeta : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS(CTFRocketLauncherBeta, CTFWeaponBaseGun);
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFRocketLauncherBeta();
	~CTFRocketLauncherBeta();

#ifndef CLIENT_DLL
	virtual void	Precache();
#endif
	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROCKETLAUNCHERBETA; }
	virtual CBaseEntity *FireProjectile( CTFPlayer *pPlayer );
	virtual void	ItemPostFrame( void );
	virtual bool	Deploy( void );
	virtual bool	DefaultReload( int iClipSize1, int iClipSize2, int iActivity );

#ifdef CLIENT_DLL
	virtual void CreateMuzzleFlashEffects( C_BaseEntity *pAttachEnt, int nIndex );
#endif

private:
	float	m_flShowReloadHintAt;

	CTFRocketLauncherBeta( const CTFRocketLauncherBeta & ) {}
};

#endif // TF_WEAPON_ROCKETLAUNCHERBETA_H