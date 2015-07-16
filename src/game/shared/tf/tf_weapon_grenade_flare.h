//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. ========//
//
// Purpose: Flare used by the flaregun.
//
//=============================================================================//
#ifndef TF_WEAPON_GRENADE_FLARE_H
#define TF_WEAPON_GRENADE_FLARE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_grenadeproj.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFGrenadeFlareProjectile C_TFGrenadeFlareProjectile
#endif

class CTFGrenadeFlareProjectile : public CTFWeaponBaseGrenadeProj
{
public:
	DECLARE_CLASS(CTFGrenadeFlareProjectile, CTFWeaponBaseGrenadeProj);
	DECLARE_NETWORKCLASS();

	CTFGrenadeFlareProjectile();
	virtual int		GetWeaponID(void) const			{ return TF_WEAPON_GRENADE_FLARE; }


#ifdef CLIENT_DLL

	//virtual void	OnDataChanged(DataUpdateType_t updateType);
	//virtual const char *GetTrailParticleName(void);
	virtual int		DrawModel(int flags);
	//virtual void	Simulate(void);

#else
	DECLARE_DATADESC();

	// Overrides.
	virtual void	Spawn();
	virtual void	Precache();
	virtual void	FlareTouch( CBaseEntity *pOther );
	virtual void	Detonate();
	virtual void	VPhysicsCollision( int index, gamevcollisionevent_t *pEvent );

	// Creation.
	static CTFGrenadeFlareProjectile *Create( const Vector &position, const QAngle &angles, const Vector &velocity, 
		                                         const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, const CTFWeaponInfo &weaponInfo );

#endif

	float		m_flCreationTime;

};

#endif //TF_WEAPON_GRENADE_FLARE_H