//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF AmmoPack.
//
//=============================================================================//
#ifndef TF_POWERUP_H
#define TF_POWERUP_H

#ifdef _WIN32
#pragma once
#endif

#include "items.h"

enum powerupsize_t
{
	POWERUP_TINY,
	POWERUP_SMALL,
	POWERUP_MEDIUM,
	POWERUP_FULL,
	POWERUP_MEGA,

	POWERUP_SIZES,
};

extern float PackRatios[POWERUP_SIZES];

//=============================================================================
//
// CTF Powerup class.
//

class CTFPowerup : public CItem
{
public:
	DECLARE_CLASS( CTFPowerup, CItem );

	DECLARE_DATADESC();

	CTFPowerup();

	void			Spawn( void );
	CBaseEntity*	Respawn( void );
	virtual void	Materialize( void );
	virtual bool	ValidTouch( CBasePlayer *pPlayer );
	virtual bool	MyTouch( CBasePlayer *pPlayer );

	bool			IsDisabled( void );
	void			SetDisabled( bool bDisabled );

	virtual float	GetRespawnDelay( void ) { return g_pGameRules->FlItemRespawnTime( this ); }

	void			DropSingleInstance( const Vector &vecVelocity, CBaseCombatCharacter *pOwner, float flUnknown, float flRestTime );

	// Input handlers
	void			InputEnable( inputdata_t &inputdata );
	void			InputDisable( inputdata_t &inputdata );
	void			InputToggle( inputdata_t &inputdata );

	virtual powerupsize_t	GetPowerupSize( void ) { return POWERUP_FULL; }

public:
	CNetworkVarForDerived( bool, m_bDisabled );
	CNetworkVarForDerived( bool, m_bRespawning );
};

#endif // TF_POWERUP_H


