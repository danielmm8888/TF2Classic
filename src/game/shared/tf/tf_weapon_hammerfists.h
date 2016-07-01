//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_HAMMERFISTS_H
#define TF_WEAPON_HAMMERFISTS_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_melee.h"

#ifdef CLIENT_DLL
#define CTFHammerfists C_TFHammerfists
#endif

//=============================================================================
//
// Fists weapon class.
//
class CTFHammerfists : public CTFWeaponBaseMelee
{
public:

	DECLARE_CLASS( CTFHammerfists, CTFWeaponBaseMelee );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFHammerfists();
	~CTFHammerfists();

	virtual void		PrimaryAttack();
	virtual void		SecondaryAttack();

	virtual void		SendPlayerAnimEvent( CTFPlayer *pPlayer );

	virtual bool		Holster( CBaseCombatWeapon *pSwitchingTo );

	virtual void		DoViewModelAnimation( void );

	virtual int			GetWeaponID( void ) const { return TF_WEAPON_HAMMERFISTS; }

private:
	void Punch( void );

};

#endif // TF_WEAPON_HAMMERFISTS_H
