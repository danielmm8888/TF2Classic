//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Critical Damage: Crits for 30/15 seconds with a crit glow. 
// Looks like an amplifier device that attaches onto weapons somehow, 
// looks similar to a car battery. 
// Crit glow effect would match the player’s merc color or be blue.
//
//=============================================================================//

#ifndef POWERUP_CRITDAMAGE_H
#define POWERUP_CRITDAMAGE_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_basedmpowerup.h"

//=============================================================================

class CTFPowerupCritdamage : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupCritdamage, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupCritdamage();

	virtual const char *GetDefaultPowerupModel( void ) { return "models/items/powerup_crit.mdl"; }
	virtual const char *GetDefaultPickupSound( void ) { return "PowerupCrit.Touch"; }

	virtual int	GetCondition( void ) { return TF_COND_POWERUP_CRITDAMAGE; }
};

#endif // POWERUP_CRITDAMAGE_H
