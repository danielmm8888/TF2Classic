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
#include "iscorer.h"
#endif

#ifdef CLIENT_DLL
#define CTFBaseRocket C_TFBaseRocket
#endif

//=============================================================================
//
// TF Base Rocket.
//
class CTFBaseRocket : public CBaseProjectile
#ifdef GAME_DLL
	, public IScorer
#endif
{

//=============================================================================
//
// Shared (client/server).
//
public:
	DECLARE_CLASS( CTFBaseRocket, CBaseProjectile );
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
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_vecVelocity );
	CNetworkVar( bool, m_bCritical );

//=============================================================================
//
// Client specific.
//
#ifdef CLIENT_DLL

public:
	virtual int		DrawModel( int flags );
	virtual void	OnPreDataChanged( DataUpdateType_t updateType );
	virtual void	PostDataUpdate( DataUpdateType_t type );
	virtual void	Simulate( void );

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

	// IScorer interface
	virtual CBasePlayer *GetScorer( void );
	virtual CBasePlayer *GetAssistant( void ) { return NULL; }

	void			SetScorer( CBaseEntity *pScorer );

	virtual void	RocketTouch( CBaseEntity *pOther );
	virtual void	Explode( trace_t *pTrace, CBaseEntity *pOther );

	void			SetCritical( bool bCritical ) { m_bCritical = bCritical; }
	virtual float	GetDamage( void ) { return m_flDamage; }
	virtual int		GetDamageType( void );
	virtual void	SetDamage(float flDamage) { m_flDamage = flDamage; }
	virtual bool	UseStockSelfDamage( void ) { return true; }
	virtual float	GetRadius( void );
	virtual float	GetSelfDamageRadius( void );
	void			DrawRadius( float flRadius );
	virtual float	GetRocketSpeed( void );

	unsigned int	PhysicsSolidMaskForEntity( void ) const;

	void			SetupInitialTransmittedGrenadeVelocity( const Vector &velocity )	{ m_vInitialVelocity = velocity; }

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_ROCKETLAUNCHER; }

	virtual CBaseEntity		*GetEnemy( void )			{ return m_hEnemy; }

	virtual bool	IsDeflectable() { return true; }
	virtual void	Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );
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

	EHANDLE					m_hScorer;
#endif
};

#endif // TF_WEAPONBASE_ROCKET_H