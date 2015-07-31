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

#include "tf_powerup.h"

//=============================================================================

class CTFPowerupCritdamage : public CTFPowerup
{
public:
	DECLARE_CLASS(CTFPowerupCritdamage, CTFPowerup);

	void	Spawn(void);
	void	Precache(void);
	bool	MyTouch(CBasePlayer *pPlayer);

	virtual const char *GetPowerupModel(void) { return "models/items/medkit_large.mdl"; }

	powerupsize_t	GetPowerupSize(void) { return POWERUP_FULL; }
};

#endif // POWERUP_CRITDAMAGE_H


