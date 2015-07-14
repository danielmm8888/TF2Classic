//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_PIPE_H
#define TF_WEAPON_PIPE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFPipe C_TFPipe
#endif

//=============================================================================
//
// Fists weapon class.
//
class CTFPipe : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS(CTFPipe, CTFWeaponBaseMelee);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFPipe() {}
	virtual int			GetWeaponID(void) const			{ return TF_WEAPON_PIPE; }

	virtual void PrimaryAttack();
	virtual void SecondaryAttack();

	virtual void SendPlayerAnimEvent(CTFPlayer *pPlayer);

	virtual void DoViewModelAnimation(void);

	void Punch(void);

private:

	CTFPipe(const CTFPipe &) {}
};

#endif // TF_WEAPON_PIPE_H
