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

#define TF_MIRV_TIMER	3.5f // seconds
#define TF_MIRV_BLIP_SOUND		"Weapon_Grenade_Mirv.Timer"
#define TF_MIRV_BLIP_FREQUENCY	1.0f

//=============================================================================
//
// TF Mirv Grenade Projectile and Bombs (Server specific.)
//
#ifdef CLIENT_DLL
#define CTFGrenadeMirvProjectile C_TFGrenadeMirvProjectile
#endif

class CTFGrenadeMirvProjectile : public CTFWeaponBaseGrenadeProj
{
public:
	DECLARE_CLASS( CTFGrenadeMirvProjectile, CTFWeaponBaseGrenadeProj );
	DECLARE_NETWORKCLASS();

	CTFGrenadeMirvProjectile();
	~CTFGrenadeMirvProjectile();

	// Unique identifier.
	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_GRENADE_MIRV; }
	
#ifdef GAME_DLL
	// Creation.
	static CTFGrenadeMirvProjectile *Create( const Vector &position, const QAngle &angles, const Vector &velocity, 
		                                     const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, CBaseEntity *pWeapon );

	// Overrides.
	virtual void	Spawn( void );
	virtual void	Precache( void );
	virtual int		OnTakeDamage( const CTakeDamageInfo &info );
	virtual void	BounceSound( void );
	virtual void	Detonate( void );
	virtual void	Explode( trace_t *pTrace, int bitsDamageType );
	virtual bool	IsDeflectable( void ) { return !m_bDefused; }

	void			BlipSound( void );

#else

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	CreateTrails( void );

#endif

private:
#ifdef GAME_DLL
	bool	m_bPlayedLeadIn;
	bool	m_bDefused;
#endif
};

#ifdef CLIENT_DLL
#define CTFGrenadeMirvBomb C_TFGrenadeMirvBomb
#endif

class CTFGrenadeMirvBomb : public CTFWeaponBaseGrenadeProj
{
public:
	DECLARE_CLASS( CTFGrenadeMirvBomb, CTFWeaponBaseGrenadeProj );
	DECLARE_NETWORKCLASS();

	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_GRENADE_MIRV; }

#ifdef GAME_DLL
	// Creation.
	static CTFGrenadeMirvBomb *Create( const Vector &position, const QAngle &angles, const Vector &velocity,
		const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, float timer );

	virtual void	Spawn( void );
	virtual void	Precache( void );
	virtual void	UpdateOnRemove( void );
	virtual void	BounceSound( void );

#endif
};

#endif // TF_WEAPON_GRENADE_MIRV_H
