//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#ifndef TF_WEAPON_ASSAULTRIFLE_H
#define TF_WEAPON_ASSAULTRIFLE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFAssaultRifle C_TFAssaultRifle
#endif

//=============================================================================
//
// TF Weapon Sub-machine gun.
//
class CTFAssaultRifle : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS(CTFAssaultRifle, CTFWeaponBaseGun);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFAssaultRifle() {}
	~CTFAssaultRifle() {}

	virtual int		GetWeaponID(void) const			{ return TF_WEAPON_ASSAULTRIFLE; }

private:

	CTFAssaultRifle(const CTFAssaultRifle &) {}
};


#endif // TF_WEAPON_SMG_H