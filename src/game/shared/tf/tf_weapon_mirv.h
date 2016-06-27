//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_MIRV_H
#define TF_WEAPON_MIRV_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weapon_grenade_mirv.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFWeaponMirv C_TFWeaponMirv
#endif

//=============================================================================
//
// TF Weapon Pipebomb Launcher.
//
#ifdef GAME_DLL
class CTFWeaponMirv : public CTFWeaponBaseGun, public ITFChargeUpWeapon
#else
class CTFWeaponMirv : public CTFWeaponBaseGun, public ITFChargeUpWeapon
#endif
{
public:

	DECLARE_CLASS( CTFWeaponMirv, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	// Server specific.
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFWeaponMirv();
	~CTFWeaponMirv();

	virtual void	Precache( void );
	virtual void	Spawn( void );
	virtual int		GetWeaponID( void ) const { return TF_WEAPON_GRENADE_MIRV; }

	virtual bool	Holster( CBaseCombatWeapon *pSwitchingTo );
	virtual bool	Deploy( void );
	virtual void	PrimaryAttack( void );
	virtual void	WeaponIdle( void );
	virtual float	GetProjectileSpeed( void );
	virtual bool	Reload( void );
	virtual void	WeaponReset( void );

public:
	// ITFChargeUpWeapon
	virtual float GetChargeBeginTime( void ) { return m_flChargeBeginTime; }
	virtual float GetChargeMaxTime( void );
	virtual const char *GetChargeSound( void ) { return "Weapon_Grenade_Mirv.ChargeUp"; }

	void LaunchGrenade( void );



#ifdef GAME_DLL
	void			UpdateOnRemove( void );
#endif

private:
	float	m_flChargeBeginTime;
	float	m_flLastDenySoundTime;

	CTFWeaponMirv( const CTFWeaponMirv & ) {}
};

#endif // TF_WEAPON_MIRV_H
