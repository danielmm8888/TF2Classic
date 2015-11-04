//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef C_TF_PROJECTILE_ROCKET_H
#define C_TF_PROJECTILE_ROCKET_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_rocket.h"

#define CTFProjectile_Rocket C_TFProjectile_Rocket

//-----------------------------------------------------------------------------
// Purpose: Rocket projectile.
//-----------------------------------------------------------------------------
class C_TFProjectile_Rocket : public C_TFBaseRocket
{
	DECLARE_CLASS( C_TFProjectile_Rocket, C_TFBaseRocket );
public:
	DECLARE_NETWORKCLASS();

	C_TFProjectile_Rocket();
	~C_TFProjectile_Rocket();

	virtual void	OnDataChanged(DataUpdateType_t updateType);

	virtual void	CreateRocketTrails( void );
	virtual const char *GetTrailParticleName( void ) { return "rockettrail"; }

private:
	bool	m_bCritical;
	bool	m_bForceUpdate;
};

#endif // C_TF_PROJECTILE_ROCKET_H
