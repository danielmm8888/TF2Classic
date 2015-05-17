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
#define CTFSMG_Scout C_TFSMG_Scout
#define CTFSMG_DM C_TFSMG_DM
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

// Server specific.
//#ifdef GAME_DLL
//	DECLARE_DATADESC();
//#endif

	CTFSMG() {}
	~CTFSMG() {}

	virtual int		GetWeaponID( void ) const			{ return TF_WEAPON_SMG; }

private:

	CTFSMG( const CTFSMG & ) {}
};

class CTFSMG_Scout : public CTFSMG
{
public:
	DECLARE_CLASS(CTFSMG_Scout, CTFSMG);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	virtual int		GetWeaponID(void) const			{ return TF_WEAPON_SMG_SCOUT; }
};

class CTFSMG_DM : public CTFSMG
{
public:
	DECLARE_CLASS(CTFSMG_DM, CTFSMG);
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	virtual int		GetWeaponID(void) const			{ return TF_WEAPON_SMG_DM; }
};


#endif // TF_WEAPON_SMG_H