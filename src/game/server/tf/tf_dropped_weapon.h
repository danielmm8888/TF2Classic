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
#include "tf_weaponbase.h"

class CTFDroppedWeapon : public CItem
{
public:
	DECLARE_CLASS( CTFDroppedWeapon, CItem );
	DECLARE_SERVERCLASS();

	CTFDroppedWeapon();

	virtual void	Spawn( void );

	bool	MyTouch( CBasePlayer *pPlayer );
	virtual bool	ValidTouch( CBasePlayer *pPlayer );
	void	SetItem( CEconItemView *pItem ){ m_Item = *pItem; }
	virtual void EndTouch( CBaseEntity *pOther );
	void	RemovalThink( void );
	float	GetCreationTime( void ) { return m_flCreationTime; }
	void	SetClip( int iClip ) { m_iClip = iClip; }
	void	SetAmmo( int iAmmo ) { m_iAmmo = iAmmo; }
	void	SetMaxAmmo( int iAmmo ) { m_iMaxAmmo = iAmmo; }

	static CTFDroppedWeapon *Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CTFWeaponBase *pWeapon );

private:
	float m_flCreationTime;
	float m_flRemoveTime;
	CEconItemView m_Item;
	
	int m_iClip;
	CNetworkVar( int, m_iAmmo );
	CNetworkVar( int, m_iMaxAmmo );
};

#endif