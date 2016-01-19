//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Sniper Rifle
//
//=============================================================================//
#ifndef TF_WEAPON_HUNTERRIFLE_H
#define TF_WEAPON_HUNTERRIFLE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

#if defined( CLIENT_DLL )
#define CTFHunterRifle C_TFHunterRifle
#endif

//=============================================================================
//
// Sniper Rifle class.
//
class CTFHunterRifle : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFHunterRifle, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFHunterRifle(){ m_iWeaponMode = TF_WEAPON_PRIMARY_MODE; m_bReloadsSingly = false; };
	~CTFHunterRifle(){};

	virtual int	GetWeaponID( void ) const			{ return TF_WEAPON_SNIPERRIFLE; }

	virtual void Spawn();
	void		 ResetTimers( void );

	virtual bool CanHolster( void ) const;
	virtual bool Holster( CBaseCombatWeapon *pSwitchingTo );

	void		 HandleZooms( void );
	virtual void ItemPostFrame( void );
	virtual bool Lower( void );
	virtual float GetProjectileDamage( void );
	virtual float GetWeaponSpread(void);
	virtual int	GetDamageType() const;

	virtual void WeaponReset( void );
	virtual bool Reload(void);
	virtual bool CanAutoReload( void ) { return false; }

	virtual bool CanFireCriticalShot( bool bIsHeadshot = false );

#ifdef CLIENT_DLL
	float GetHUDDamagePerc( void );
#endif

	bool IsZoomed( void );

private:

private:
	// Auto-rezooming handling
	void SetRezoom( bool bRezoom, float flDelay );

	void Zoom( void );
	void ZoomOutIn( void );
	void ZoomIn( void );
	void ZoomOut( void );
	void Fire( CTFPlayer *pPlayer );

private:

	CNetworkVar(float, m_flChargedSpread);
	CNetworkVar(float, m_flChargedDamage);

	// Handles rezooming after the post-fire unzoom
	float m_flUnzoomTime;
	float m_flRezoomTime;
	bool m_bRezoomAfterShot;

	CTFHunterRifle( const CTFHunterRifle & );
};

#endif // TF_WEAPON_HUNTERRIFLE_H
