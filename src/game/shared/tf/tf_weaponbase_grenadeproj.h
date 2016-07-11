//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF basic grenade projectile functionality.
//
//=============================================================================//
#ifndef TF_WEAPONBASE_GRENADEPROJ_H
#define TF_WEAPONBASE_GRENADEPROJ_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"
#include "tf_weaponbase.h"
#include "basegrenade_shared.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFWeaponBaseGrenadeProj C_TFWeaponBaseGrenadeProj
#endif

//=============================================================================
//
// TF base grenade projectile class.
//
class CTFWeaponBaseGrenadeProj : public CBaseGrenade
{
public:

	DECLARE_CLASS( CTFWeaponBaseGrenadeProj, CBaseGrenade );
	DECLARE_NETWORKCLASS();

							CTFWeaponBaseGrenadeProj();
	virtual					~CTFWeaponBaseGrenadeProj();
	virtual void			Spawn();
	virtual void			Precache();

	void					InitGrenade( const Vector &velocity, const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, CBaseEntity *pWeapon );

	// Unique identifier.
	virtual int GetWeaponID( void ) const { return TF_WEAPON_NONE; }

	// This gets sent to the client and placed in the client's interpolation history
	// so the projectile starts out moving right off the bat.
	CNetworkVector( m_vInitialVelocity );

	virtual float		GetShakeAmplitude( void ) { return 10.0; }
	virtual float		GetShakeRadius( void ) { return 300.0; }

	void				SetCritical( bool bCritical ) { m_bCritical = bCritical; }
	virtual int			GetDamageType();
	virtual float		GetDamageRadius( void );

	CNetworkVar( int, m_iDeflected );
	CNetworkHandle( CBaseEntity, m_hLauncher );
	CNetworkHandle( CBaseEntity, m_hDeflectOwner );

private:

	CTFWeaponBaseGrenadeProj( const CTFWeaponBaseGrenadeProj & );

	// Client specific.
#ifdef CLIENT_DLL

public:

	virtual void			OnPreDataChanged( DataUpdateType_t updateType );
	virtual void			OnDataChanged( DataUpdateType_t type );

	virtual C_BaseEntity	*GetItemTintColorOwner( void ) { return GetThrower(); }

	float					m_flSpawnTime;
	bool					m_bCritical;
	int						m_iOldTeamNum;

	// Server specific.
#else

public:

	DECLARE_DATADESC();

	static CTFWeaponBaseGrenadeProj *Create( const char *szName, const Vector &position, const QAngle &angles, 
				const Vector &velocity, const AngularImpulse &angVelocity, 
				CBaseCombatCharacter *pOwner, CBaseEntity *pWeapon );
	 
	int						OnTakeDamage( const CTakeDamageInfo &info );

	virtual void			DetonateThink( void );
	void					Detonate( void );

	void					SetupInitialTransmittedGrenadeVelocity( const Vector &velocity )	{ m_vInitialVelocity = velocity; }

	bool					ShouldNotDetonate( void );
	void					RemoveGrenade( bool bBlinkOut = true );

	void					SetTimer( float time ){ m_flDetonateTime = time; }
	float					GetDetonateTime( void ){ return m_flDetonateTime; }

	void					SetDetonateTimerLength( float timer );

	void					VPhysicsUpdate( IPhysicsObject *pPhysics );

	void					Explode( trace_t *pTrace, int bitsDamageType );

	bool					UseImpactNormal()							{ return m_bUseImpactNormal; }
	const Vector			&GetImpactNormal( void ) const				{ return m_vecImpactNormal; }

	virtual void			SetLauncher( CBaseEntity *pLauncher ) { m_hLauncher = pLauncher; }

	virtual bool			IsDeflectable() { return true; }
	virtual void			Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );
	virtual void			IncremenentDeflected( void );

	virtual void			BlipSound( void ) { }
	void					SetNextBlipTime( float flTime ) { m_flNextBlipTime = flTime; }

protected:
	void					DrawRadius( float flRadius );

	bool					m_bUseImpactNormal;
	Vector					m_vecImpactNormal;

	float					m_flNextBlipTime;

private:
	// Custom collision to allow for constant elasticity on hit surfaces.
	virtual void			ResolveFlyCollisionCustom( trace_t &trace, Vector &vecVelocity );

	float					m_flDetonateTime;

	bool					m_bInSolid;

	CNetworkVar( bool,		m_bCritical );

	float					m_flCollideWithTeammatesTime;
	bool					m_bCollideWithTeammates;

#endif
};

#endif // TF_WEAPONBASE_GRENADEPROJ_H
