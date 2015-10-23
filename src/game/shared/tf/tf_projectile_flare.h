//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. ========//
//
// Purpose: Flare used by the flaregun.
//
//=============================================================================//
#ifndef TF_PROJECTILE_FLARE_H
#define TF_PROJECTILE_FLARE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_rocket.h"
#ifdef GAME_DLL
#include "iscorer.h"
#endif

// Client specific.
#ifdef CLIENT_DLL
#define CTFProjectile_Flare C_TFProjectile_Flare
#endif

#ifdef GAME_DLL
class CTFProjectile_Flare : public CTFBaseRocket, public IScorer
#else
class C_TFProjectile_Flare : public C_TFBaseRocket
#endif
{
public:
	DECLARE_CLASS( CTFProjectile_Flare, CTFBaseRocket );
	DECLARE_DATADESC();
	DECLARE_NETWORKCLASS();

	CTFProjectile_Flare();
	~CTFProjectile_Flare();

#ifdef GAME_DLL

	static CTFProjectile_Flare *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pScorer = NULL );	
	virtual void	Spawn();
	virtual void	Precache();

	// IScorer interface
	virtual CBasePlayer *GetScorer( void );
	virtual CBasePlayer *GetAssistant( void ) { return NULL; }

	//virtual int		GetWeaponID(void) const			{ return TF_WEAPON_GRENADE_FLARE; }

	void	SetScorer( CBaseEntity *pScorer );

	void	SetCritical( bool bCritical ) { m_bCritical = bCritical; }
	virtual int		GetDamageType();

	float	GetProjectileSpeed( void ) { return 2000.0f; }

	// Overrides.
	virtual void	Explode( trace_t *pTrace, CBaseEntity *pOther );
#else

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	CreateTrails( void );

#endif

private:
#ifdef GAME_DLL
	CBaseHandle m_Scorer;
	CNetworkVar( bool,	m_bCritical );
#else
	bool		m_bCritical;
#endif

};

#endif //TF_PROJECTILE_FLARE_H