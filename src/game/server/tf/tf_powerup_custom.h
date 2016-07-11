//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: A powerup entity which lets you select the powerup, duration, model
//			and pickup sound.
//
//			Initially we didn't want people to change any of these things on 
//			the regular powerup entities, as we felt like it'd cause people 
//			to make unbalanced maps. However, later we realized that by doing so
//			we were massively hindering the creativity of map makers, which is
//			why we decided to make this separate entity
//		
//=============================================================================//

#ifndef POWERUP_CUSTOM_H
#define POWERUP_CUSTOM_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_basedmpowerup.h"

//=============================================================================

class CTFPowerupCustom : public CTFBaseDMPowerup
{
public:
	DECLARE_CLASS( CTFPowerupCustom, CTFBaseDMPowerup );
	DECLARE_DATADESC();

	CTFPowerupCustom();

	virtual int	GetCondition( void ) { return m_iPowerupCondition; }

private:
	int		m_iPowerupCondition;
};

#endif // POWERUP_CUSTOM_H
