//=============================================================================//
//
// Purpose: Plasma blast used by MLG.
//
//=============================================================================//
#ifndef TF_PROJECTILE_PLASMA_H
#define TF_PROJECTILE_PLASMA_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_rocket.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFProjectile_Plasma C_TFProjectile_Plasma
#define CTFProjectile_PlasmaBomblet C_TFProjectile_PlasmaBomblet
#endif

class CTFProjectile_Plasma : public CTFBaseRocket
{
public:
	DECLARE_CLASS( CTFProjectile_Plasma, CTFBaseRocket );
	DECLARE_NETWORKCLASS();

	CTFProjectile_Plasma();
	~CTFProjectile_Plasma();

#ifdef GAME_DLL

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_DISPLACER; }

	static CTFProjectile_Plasma *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pScorer = NULL );
	virtual void	Spawn();
	virtual void	Precache();

	// Overrides.
	virtual void	Explode( trace_t *pTrace, CBaseEntity *pOther );
	virtual bool	IsDeflectable( void ) { return false; }

#else

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	CreateTrails( void );

#endif
};


class CTFProjectile_PlasmaBomblet : public CTFBaseRocket
{
	DECLARE_CLASS( CTFProjectile_PlasmaBomblet, CTFBaseRocket );
	DECLARE_NETWORKCLASS();

#ifdef GAME_DLL
	static CTFProjectile_PlasmaBomblet *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pScorer = NULL );

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_DISPLACER; }

	virtual void	Spawn( void );
	virtual void	Precache();
	virtual void	Explode( trace_t *pTrace, CBaseEntity *pOther );

	virtual bool	IsDeflectable( void ) { return false; }
	virtual bool	UseStockSelfDamage( void ) { return false; }
	virtual float	GetSelfDamageRadius( void ) { return 0.0f; }

#else

	virtual void	OnDataChanged( DataUpdateType_t updateType );
	virtual void	CreateTrails( void );

#endif
};

#endif // TF_PROJECTILE_PLASMA_H