//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: The ragemode powerup (name is a W.I.P). Equips the mercenary with
//			the Hammerfist weapon for a short period of time.
//
//=============================================================================//

#ifndef POWERUP_RAGEMODE_H
#define POWERUP_RAGEMODE_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_basedmpowerup.h"

//=============================================================================

class CTFPowerupRagemode : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupRagemode, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupRagemode();

	void	Spawn(void);
	void	Precache(void);
	bool	MyTouch(CBasePlayer *pPlayer);

	virtual int	GetCondition( void ) { return TF_COND_POWERUP_RAGEMODE; }

	powerupsize_t	GetPowerupSize(void) { return POWERUP_FULL; }
};

#endif // POWERUP_RAGEMODE_H


