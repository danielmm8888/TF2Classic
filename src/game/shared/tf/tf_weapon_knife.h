//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: Weapon Knife Class
//
//=============================================================================
#ifndef TF_WEAPON_KNIFE_H
#define TF_WEAPON_KNIFE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFKnife C_TFKnife
#endif

//=============================================================================
//
// Knife class.
//
class CTFKnife : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFKnife, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFKnife();

	virtual int			GetWeaponID( void ) const			{ return TF_WEAPON_KNIFE; }

	virtual bool		Deploy( void );
	virtual void		ItemPostFrame( void );
	virtual void		PrimaryAttack( void );

	virtual float		GetMeleeDamage( CBaseEntity *pTarget, int &iCustomDamage );

	virtual void		SendPlayerAnimEvent( CTFPlayer *pPlayer );

	bool				IsBehindAndFacingTarget( CBaseEntity *pTarget );

	virtual bool		CalcIsAttackCriticalHelper( void );

	virtual void		DoViewModelAnimation( void );
	virtual bool		SendWeaponAnim( int iActivity );

	void				BackstabVMThink( void );

private:
	EHANDLE				m_hBackstabVictim;
	CNetworkVar( bool, m_bReadyToBackstab );

	CTFKnife( const CTFKnife & ) {}
};

#endif // TF_WEAPON_KNIFE_H
