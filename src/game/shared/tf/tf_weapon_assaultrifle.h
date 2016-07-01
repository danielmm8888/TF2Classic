//=============================================================================
//
//
//=============================================================================
#ifndef TF_WEAPON_SMG_H
#define TF_WEAPON_SMG_H
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
// TF Weapon Assault Rifle
//
class CTFAssaultRifle : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFAssaultRifle, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFAssaultRifle() {}
	~CTFAssaultRifle() {}

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_ASSAULTRIFLE; }

private:

	CTFAssaultRifle( const CTFAssaultRifle & ) {}
};

#endif // TF_WEAPON_SMG_H