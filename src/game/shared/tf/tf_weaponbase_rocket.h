//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Base Rockets.
//
//=============================================================================//
#ifndef TF_WEAPONBASE_ROCKET_H
#define TF_WEAPONBASE_ROCKET_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "baseprojectile.h"
#include "tf_shareddefs.h"
#ifndef CLIENT_DLL
// Server specific.
#include "smoke_trail.h"
#endif

#ifdef CLIENT_DLL
#define CTFBaseRocket C_TFBaseRocket
#endif

#define TF_ROCKET_RADIUS	146.0f	// Matches grenade radius.

//=============================================================================
//
// TF Base Rocket.
//
class CTFBaseRocket : public CBaseProjectile
{

//=============================================================================
//
// Shared (client/server).
//
public:

	DECLARE_CLASS( CTFBaseRocket, CBaseAnimating );
	DECLARE_NETWORKCLASS();

			CTFBaseRocket();
			~CTFBaseRocket();

	void	Precache( void );
	void	Spawn( void );

	CNetworkVar( int, m_iDeflected );
	CNetworkHandle( CBaseEntity, m_hLauncher );

protected:

	// Networked.
	CNetworkVector( m_vInitialVelocity );

//=============================================================================
//
// Client specific.
//
#ifdef CLIENT_DLL

public:

	virtual int		DrawModel( int flags );
	virtual void	OnPreDataChanged( DataUpdateType_t updateType );
	virtual void	PostDataUpdate( DataUpdateType_t type );

protected:
	int		m_iOldTeamNum;

private:

	float	 m_flSpawnTime;

//=============================================================================
//
// Server specific.
//
#else

public:

	DECLARE_DATADESC();

	static CTFBaseRocket *Create( CBaseEntity *pWeapon, const char *szClassname, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL );	

	virtual void	RocketTouch( CBaseEntity *pOther );
	virtual void	Explode( trace_t *pTrace, CBaseEntity *pOther );

	virtual float	GetDamage() { return m_flDamage; }
	virtual int		GetDamageType() { return g_aWeaponDamageTypes[ GetWeaponID() ]; }
	virtual void	SetDamage(float flDamage) { m_flDamage = flDamage; }
	virtual float	GetRadius();	
	void			DrawRadius( float flRadius );

	unsigned int	PhysicsSolidMaskForEntity( void ) const;

	void			SetupInitialTransmittedGrenadeVelocity( const Vector &velocity )	{ m_vInitialVelocity = velocity; }

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROCKETLAUNCHER; }

	virtual CBaseEntity		*GetEnemy( void )			{ return m_hEnemy; }

	void			SetHomingTarget( CBaseEntity *pHomingTarget );

	virtual void	IncremenentDeflected( void );
	virtual void	SetLauncher( CBaseEntity *pLauncher );

protected:

	void			FlyThink( void );

protected:

	// Not networked.
	float					m_flDamage;

	float					m_flCollideWithTeammatesTime;
	bool					m_bCollideWithTeammates;

	CHandle<CBaseEntity>	m_hEnemy;

#endif
};

#endif // TF_WEAPONBASE_ROCKET_H