//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Dropped DM weapon
//
//=============================================================================//
#ifndef TF_DROPPED_WEAPON
#define TF_DROPPED_WEAPON
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "tf_item.h"
#include "items.h"

class CTFDroppedWeapon : public CItem
{
public:
	DECLARE_CLASS( CTFDroppedWeapon, CItem );
	DECLARE_SERVERCLASS();

	CTFDroppedWeapon();

	virtual void	Spawn( void );

	bool	MyTouch( CBasePlayer *pPlayer );
	virtual bool	ValidTouch( CBaseEntity *pPlayer );
	void	SetWeaponID( unsigned int nWeaponID ){ m_nWeaponID = nWeaponID; }
	virtual void EndTouch( CBaseEntity *pOther );
	void	RemovalThink( void );
	float	GetCreationTime( void ) { return m_flCreationTime; }

	static CTFDroppedWeapon *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, const char *pszModelName, unsigned int nWeaponID );


private:
	float m_flCreationTime;
	float m_flRemoveTime;
	int m_nWeaponID;
};

#endif