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
	float	GetEffectDuration( void ) { return m_flEffectDuration; }
	void	SetEffectDuration( float flTime ) { m_flEffectDuration = flTime; }

	virtual const char *GetDefaultPickupSound( void ) { return "HealthKit.Touch"; }
	virtual const char *GetDefaultPowerupModel( void ) { return "models/class_menu/random_class_icon.mdl"; }

	virtual int GetCondition( void ) { return TF_COND_AIMING; }

	powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }

	static CTFBaseDMPowerup *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, const char *pszClassname, float flDuration );

protected:
	float		m_flEffectDuration;

private:
	string_t	m_strPickupSound;
	float		m_flRespawnTime;
};

#endif // BASE_DM_POWERUP_H
