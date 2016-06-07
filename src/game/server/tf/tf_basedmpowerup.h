//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Base class for Deathmatch powerups 
//
//=============================================================================//

#ifndef BASE_DM_POWERUP_H
#define BASE_DM_POWERUP_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_powerup.h"

//=============================================================================

class CTFBaseDMPowerup : public CTFPowerup
{
public:
	DECLARE_CLASS( CTFBaseDMPowerup, CTFPowerup );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CTFBaseDMPowerup();

	void	Spawn( void );
	void	Precache( void );
	bool	MyTouch( CBasePlayer *pPlayer );
	float	GetRespawnDelay( void );

	virtual const char *GetDefaultPickupSound( void ) { return "Healthkit.Touch"; }
	virtual const char *GetDefaultPowerupModel( void ) { return "models/class_menu/random_class_icon.mdl"; }

	virtual int	GetEffectDuration( void ) { return 0; }
	virtual int GetCondition( void ) { return TF_COND_AIMING; }

	powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }

private:
	string_t	m_strPickupSound;
	float		m_flRespawnTime;
};

#endif // BASE_DM_POWERUP_H
