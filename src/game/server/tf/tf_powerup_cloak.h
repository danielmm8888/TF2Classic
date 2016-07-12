//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: FILL IN
//
//=============================================================================//

#ifndef POWERUP_CLOAK_H
#define POWERUP_CLOAK_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_basedmpowerup.h"

//=============================================================================

class CTFPowerupCloak : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupCloak, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupCloak();

	virtual const char *GetDefaultPickupSound( void ) { return NULL; }

	virtual int	GetCondition( void ) { return TF_COND_POWERUP_CLOAK; }
};

#endif // POWERUP_CLOAK_H
