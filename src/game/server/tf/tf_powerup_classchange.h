//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Lets you join a specified class for a short duration of the time
//
//=============================================================================//

#ifndef POWERUP_CLASSCHANGE_H
#define POWERUP_CLASSCHANGE_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_basedmpowerup.h"

//=============================================================================

class CTFPowerupClasschange : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupClasschange, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupClasschange();

	virtual int	GetCondition( void ) { return TF_COND_POWERUP_CLASSCHANGE; }

private:
	int		m_nClassIndex;

};

#endif // POWERUP_CLASSCHANGE_H


