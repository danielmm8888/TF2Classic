//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Deathmatch weapon spawning entity.
//
//=============================================================================//
#ifndef ENTITY_WEAPONSPAWN_H
#define ENTITY_WEAPONSPAWN_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_powerup.h"

//=============================================================================

class CWeaponSpawner : public CTFPowerup
{
public:
	DECLARE_CLASS(CWeaponSpawner, CTFPowerup);
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	void	Spawn(void);
	void	Precache(void);
	bool	MyTouch(CBasePlayer *pPlayer);

	virtual const char *GetPowerupModel(void) { return "models/items/medkit_large.mdl"; }

	powerupsize_t	GetPowerupSize(void) { return POWERUP_FULL; }

	int		m_iWeaponNumber;

private:
	CTFWeaponInfo *pWeaponInfo;
};

#endif // ENTITY_HEALTHKIT_H


