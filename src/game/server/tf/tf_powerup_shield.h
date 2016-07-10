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

class CTFPowerupShield : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupShield, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupShield();

	virtual const char *GetDefaultPowerupModel( void ) { return "models/items/powerup_uber.mdl"; }
	virtual const char *GetDefaultPickupSound( void ) { return "PowerupUber.Touch"; }

	virtual int	GetCondition( void ) { return TF_COND_POWERUP_SHIELD; }
};

#endif // POWERUP_SHORTUBER_H


