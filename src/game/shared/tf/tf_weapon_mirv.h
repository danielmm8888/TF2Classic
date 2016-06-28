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
class CTFWeaponMirv : public CTFWeaponBaseGun, public ITFChargeUpWeapon
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
	virtual void	ItemPostFrame( void );
	virtual void	PrimaryAttack( void );
	virtual void	WeaponIdle( void );
	virtual float	GetProjectileSpeed( void );
	virtual void	WeaponReset( void );

public:
	// ITFChargeUpWeapon
	virtual float GetChargeBeginTime( void ) { return m_flChargeBeginTime; }
	virtual float GetChargeMaxTime( void );
	virtual const char *GetChargeSound( void ) { return "Weapon_Grenade_Mirv.ChargeUp"; }

	void			LaunchGrenade( void );
	void			BlipSound( void );

private:
	CNetworkVar( float, m_flChargeBeginTime );
	float	m_flLastDenySoundTime;
	float	m_flNextBlipTime;

	CTFWeaponMirv( const CTFWeaponMirv & ) {}
};

#endif // TF_WEAPON_MIRV_H
