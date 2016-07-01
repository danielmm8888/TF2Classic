//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
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
#define CTFSMG C_TFSMG
#define CTFSMG_Primary C_TFSMG_Primary
#endif

//=============================================================================
//
// TF Weapon Sub-machine gun.
//
class CTFSMG : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFSMG, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFSMG() {}
	~CTFSMG() {}

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_SMG; }

private:

	CTFSMG( const CTFSMG & ) {}
};

class CTFSMG_Primary : public CTFSMG
{
public:
	DECLARE_CLASS( CTFSMG_Primary, CTFSMG );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();
};

#endif // TF_WEAPON_SMG_H