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

	virtual void	PrimaryAttack( void );
	virtual void	SecondaryAttack( void );

#ifdef GAME_DLL
	virtual void	Precache( void );
	virtual void	ApplyBiteEffects( void );
#endif
};

#endif // TF_WEAPON_LUNCHBOX_H
