//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: FILL IN
//
//=============================================================================//

#ifndef POWERUP_SPEEDBOOST_H
#define POWERUP_SPEEDBOOST_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_basedmpowerup.h"

//=============================================================================

class CTFPowerupSpeedBoost : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupSpeedBoost, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupSpeedBoost();

	virtual const char *GetDefaultPowerupModel( void ) { return "models/items/powerup_speed.mdl"; }
	virtual const char *GetDefaultPickupSound( void ) { return "PowerupSpeedBoost.Touch"; }

	virtual int	GetCondition( void ) { return TF_COND_POWERUP_SPEEDBOOST; }
};

#endif // POWERUP_SPEEDBOOST_H
