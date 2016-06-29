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
	DECLARE_CLASS( CWeaponSpawner, CTFPowerup );
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CWeaponSpawner();

	void	Spawn( void );
	void	Precache( void );
	virtual bool KeyValue( const char *szKeyName, const char *szValue );
	virtual CBaseEntity* Respawn( void );
	virtual void	Materialize( void );
	bool	MyTouch( CBasePlayer *pPlayer );
	void	EndTouch( CBaseEntity *pOther );
	float	GetRespawnDelay( void );

private:
	CEconItemView m_Item;

	CNetworkVar( bool, m_bStaticSpawner );
	CNetworkVar( bool, m_bOutlineDisabled );

	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_bDisabled );
	IMPLEMENT_NETWORK_VAR_FOR_DERIVED( m_bRespawning );

	CNetworkVar( float, m_flRespawnTime );
	CNetworkVar( float, m_flRespawnAtTime );

	int		m_nWeaponID;
	int		m_nItemID;
};

#endif // ENTITY_HEALTHKIT_H


