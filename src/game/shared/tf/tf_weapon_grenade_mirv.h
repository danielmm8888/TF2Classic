//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: TF Mirv Grenade.
//
//=============================================================================//
#ifndef TF_WEAPON_GRENADE_MIRV_H
#define TF_WEAPON_GRENADE_MIRV_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_grenadeproj.h"

//=============================================================================
//
// TF Mirv Grenade Projectile and Bombs (Server specific.)
//
#ifdef GAME_DLL

class CTFGrenadeMirvProjectile : public CTFWeaponBaseGrenadeProj
{
public:

	DECLARE_CLASS( CTFGrenadeMirvProjectile, CTFWeaponBaseGrenadeProj );

	// Unique identifier.
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_GRENADE_MIRV; }

	// Creation.
	static CTFGrenadeMirvProjectile *Create( const Vector &position, const QAngle &angles, const Vector &velocity, 
		                                     const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, const CTFWeaponInfo &weaponInfo, float timer, int iFlags = 0 );

	// Overrides.
	virtual void	Spawn( void );
	virtual void	Precache( void );
	virtual void	BounceSound( void );
	virtual void	Detonate( void );
	virtual void	Explode( trace_t *pTrace, int bitsDamageType );
	void			DetonateThink( void );
	void			BlipSound( void );

	DECLARE_DATADESC();

private:
	float	m_flNextBlipTime;
};

class CTFGrenadeMirvBomb : public CTFWeaponBaseGrenadeProj
{
public:

	DECLARE_CLASS( CTFGrenadeMirvBomb, CTFWeaponBaseGrenadeProj );

	// Creation.
	static CTFGrenadeMirvBomb *Create( const Vector &position, const QAngle &angles, const Vector &velocity, 
		                               const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, float timer );

	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_GRENADE_MIRV; }

	virtual void	Spawn( void );
	virtual void	Precache( void );
	virtual void	Detonate( void );
	virtual void	BounceSound( void );
};

#endif

#endif // TF_WEAPON_GRENADE_MIRV_H
