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

// Client specific.
#ifdef CLIENT_DLL
#define CTFProjectile_Flare C_TFProjectile_Flare
#endif

class CTFProjectile_Flare : public CTFBaseRocket
{
public:
	DECLARE_CLASS( CTFProjectile_Flare, CTFBaseRocket );
	DECLARE_DATADESC();
	DECLARE_NETWORKCLASS();

	CTFProjectile_Flare();
	~CTFProjectile_Flare();

#ifdef GAME_DLL

	static CTFProjectile_Flare *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pScorer = NULL );
	virtual void	Spawn();
	virtual void	Precache();

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_FLAREGUN; }
	virtual float	GetRocketSpeed( void );

	// Overrides.
	virtual void	Explode( trace_t *pTrace, CBaseEntity *pOther );

#else

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	CreateTrails( void );

#endif

};

#endif //TF_PROJECTILE_FLARE_H
