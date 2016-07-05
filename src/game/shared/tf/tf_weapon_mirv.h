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

	virtual bool	HasChargeBar( void ) { return true; }
	virtual float	InternalGetEffectBarRechargeTime( void ) { return 8.0f; }
	virtual const char	*GetEffectLabelText( void ) { return "#TF_MIRV"; }

public:
	// ITFChargeUpWeapon
	virtual float GetChargeBeginTime( void ) { return m_flChargeBeginTime; }
	virtual float GetChargeMaxTime( void );
	virtual const char *GetChargeSound( void ) { return "Weapon_Grenade_Mirv.ChargeUp"; }

	void			LaunchGrenade( void );

private:
	CNetworkVar( float, m_flChargeBeginTime );

	CTFWeaponMirv( const CTFWeaponMirv & ) {}
};

#endif // TF_WEAPON_MIRV_H
