//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======
//
// Purpose: A remake of Pyro's flaregun from live TF2s
//
//=============================================================================
#ifndef TF_WEAPON_LUNCHBOX_H
#define TF_WEAPON_LUNCHBOX_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase.h"

#ifdef CLIENT_DLL
#define CTFLunchBox C_TFLunchBox
#endif

class CTFLunchBox : public CTFWeaponBase
{
public:
	DECLARE_CLASS( CTFLunchBox, CTFWeaponBase );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	virtual int GetWeaponID() const { return TF_WEAPON_LUNCHBOX; }

	virtual bool	ShouldBlockPrimaryFire( void ) { return true; }

	virtual void	PrimaryAttack( void );
	virtual void	SecondaryAttack( void );

	virtual bool	HasChargeBar( void ) { return true; }
	virtual float	InternalGetEffectBarRechargeTime( void ) { return 30.0f; }

#ifdef GAME_DLL
	virtual void	Precache( void );
	virtual void	ApplyBiteEffects( void );
#endif

private:
#ifdef GAME_DLL
	EHANDLE m_hDroppedLunch;
#endif
};

#endif // TF_WEAPON_LUNCHBOX_H
