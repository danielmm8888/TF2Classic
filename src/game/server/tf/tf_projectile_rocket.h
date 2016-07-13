//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket Projectile
//
//=============================================================================
#ifndef TF_PROJECTILE_ROCKET_H
#define TF_PROJECTILE_ROCKET_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_rocket.h"


//=============================================================================
//
// Generic rocket.
//
class CTFProjectile_Rocket : public CTFBaseRocket
{
public:
	DECLARE_CLASS( CTFProjectile_Rocket, CTFBaseRocket );
	DECLARE_NETWORKCLASS();

	// Creation.
	static CTFProjectile_Rocket *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner = NULL, CBaseEntity *pScorer = NULL );
	virtual void Spawn();
	virtual void Precache();
};

#endif	//TF_PROJECTILE_ROCKET_H
