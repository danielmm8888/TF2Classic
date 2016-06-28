//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: TF Stickybomb.
//
//=============================================================================//
#ifndef TF_WEAPON_GRENADE_STICKY_H
#define TF_WEAPON_GRENADE_STICKYBOMB_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_grenadeproj.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFGrenadeStickybombProjectile C_TFGrenadeStickybombProjectile
#endif

//=============================================================================
//
// TF Pipebomb Grenade
//
class CTFGrenadeStickybombProjectile : public CTFWeaponBaseGrenadeProj
{
public:
	DECLARE_CLASS( CTFGrenadeStickybombProjectile, CTFWeaponBaseGrenadeProj );
	DECLARE_NETWORKCLASS();

	CTFGrenadeStickybombProjectile();
	~CTFGrenadeStickybombProjectile();

	// Unique identifier.
	virtual int			GetWeaponID( void ) const { return TF_WEAPON_GRENADE_PIPEBOMB; }

	virtual int			GetDamageType();
	virtual void		UpdateOnRemove( void );

	float				GetCreationTime( void ) { return m_flCreationTime; }

private:
	float		m_flCreationTime;

#ifdef CLIENT_DLL

public:
	virtual void OnDataChanged( DataUpdateType_t updateType );
	virtual const char *GetTrailParticleName( void );
	virtual void CreateTrails( void );
	virtual int DrawModel( int flags );
	virtual void	Simulate( void );

private:
	bool		m_bPulsed;

#else

public:
	DECLARE_DATADESC();

	// Creation.
	static CTFGrenadeStickybombProjectile *Create( const Vector &position, const QAngle &angles, const Vector &velocity,
		const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, CBaseEntity *pWeapon );

	// Overrides.
	virtual void	Spawn();
	virtual void	Precache();

	virtual void	Detonate();
	virtual void	Fizzle();

	virtual void	VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );

	virtual int		OnTakeDamage( const CTakeDamageInfo &info );
	virtual void	Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );

private:
	bool		m_bTouched;
	bool		m_bFizzle;
	float		m_flMinSleepTime;

#endif
};
#endif // TF_WEAPON_GRENADE_PIPEBOMB_H
