//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: The Speedshooter powerup. A powerup which increases your firing and
//			reload time for a certain (yet undetermined) period of time
//
//=============================================================================//

#ifndef POWERUP_SPEEDSHOOTER_H
#define POWERUP_SPEEDSHOOTER_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_basedmpowerup.h"

//=============================================================================

class CTFPowerupSpeedshooter : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupSpeedshooter, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupSpeedshooter();

	void	Spawn( void );
	void	Precache( void );
	bool	MyTouch( CBasePlayer *pPlayer );

	virtual const char *GetPowerupModel( void ) { return "models/class_menu/random_class_icon.mdl"; }
	virtual const char *GetPickupSound( void ) { return "HealthKit.Touch"; }

	virtual int	GetEffectDuration( void ) { return 15; }
	virtual int	GetCondition( void ) { return TF_COND_POWERUP_FASTRELOAD; }

	powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }
};

#endif // POWERUP_SPEEDSHOOTER_H


