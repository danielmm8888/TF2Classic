//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: FILL IN
//
//=============================================================================//

#ifndef POWERUP_SHORTUBER_H
#define POWERUP_SHORTUBER_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_basedmpowerup.h"

//=============================================================================

class CTFPowerupShortUber : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupShortUber, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupShortUber();

	void	Spawn( void );
	void	Precache( void );
	bool	MyTouch( CBasePlayer *pPlayer );

	virtual const char *GetDefaultPowerupModel( void ) { return "models/items/powerup_uber.mdl"; }
	virtual const char *GetDefaultPickupSound( void ) { return "PowerupUber.Touch"; }

	virtual int	GetCondition( void ) { return TF_COND_POWERUP_SHORTUBER; }

	powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }
};

#endif // POWERUP_SHORTUBER_H


