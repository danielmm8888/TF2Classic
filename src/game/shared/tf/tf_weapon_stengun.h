//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//
//=============================================================================
#ifndef TF_WEAPON_STENGUN_H
#define TF_WEAPON_STENGUN_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

// Client specific.
#ifdef CLIENT_DLL
#define CTFStenGun C_TFStenGun
#endif

//=============================================================================
//
// TF Weapon Sub-machine gun.
//
class CTFStenGun : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFStenGun, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFStenGun() {}
	~CTFStenGun() {}

	virtual int		GetWeaponID(void) const			{ return TF_WEAPON_STENGUN; }

private:

	CTFStenGun( const CTFStenGun & ) {}
};


#endif // TF_WEAPON_SMG_H