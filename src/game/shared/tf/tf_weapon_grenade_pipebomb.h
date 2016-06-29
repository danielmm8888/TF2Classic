//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. ========//
//
// Purpose: TF Pipebomb Grenade.
//
//=============================================================================//
#ifndef TF_WEAPON_GRENADE_PIPEBOMB_H
#define TF_WEAPON_GRENADE_PIPEBOMB_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_grenadeproj.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFGrenadePipebombProjectile C_TFGrenadePipebombProjectile
#endif

//=============================================================================
//
// TF Pipebomb Grenade
//
class CTFGrenadePipebombProjectile : public CTFWeaponBaseGrenadeProj
{
public:
	DECLARE_CLASS( CTFGrenadePipebombProjectile, CTFWeaponBaseGrenadeProj );
	DECLARE_NETWORKCLASS();

	CTFGrenadePipebombProjectile();
	~CTFGrenadePipebombProjectile();

	// Unique identifier.
	virtual int			GetWeaponID( void ) const { return TF_WEAPON_GRENADE_DEMOMAN; }

private:
	float		m_flCreationTime;

#ifdef CLIENT_DLL

public:
	virtual void OnDataChanged( DataUpdateType_t updateType );
	virtual const char *GetTrailParticleName( void );
	virtual void CreateTrails( void );
	virtual int DrawModel( int flags );

#else

public:
	DECLARE_DATADESC();

	// Creation.
	static CTFGrenadePipebombProjectile *Create( const Vector &position, const QAngle &angles, const Vector &velocity,
		const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, CBaseEntity *pWeapon );

	// Overrides.
	virtual void	Spawn();
	virtual void	Precache();

	virtual void	BounceSound( void );
	virtual void	Detonate();

	virtual void	PipebombTouch( CBaseEntity *pOther );
	virtual void	VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );

	virtual int		OnTakeDamage( const CTakeDamageInfo &info );

	virtual CBaseEntity *GetEnemy( void ) { return m_hEnemy; }

	virtual void	Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );

private:
	EHANDLE			m_hEnemy;

	bool			m_bTouched;
	float			m_flFullDamage;

#endif
};
#endif // TF_WEAPON_GRENADE_PIPEBOMB_H
