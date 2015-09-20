//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_chainsaw.h"
#include "decals.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Chainsaw tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFChainsaw, DT_TFWeaponChainsaw )

BEGIN_NETWORK_TABLE( CTFChainsaw, DT_TFWeaponChainsaw )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFChainsaw )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_chainsaw, CTFChainsaw );
PRECACHE_WEAPON_REGISTER( tf_weapon_chainsaw );

//=============================================================================
//
// Weapon Chainsaw functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFChainsaw::CTFChainsaw()
{
}


void CTFChainsaw::PrimaryAttack(void)
{
	// Get the current player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	// Set the weapon usage mode - primary, secondary.
	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;
	m_bConnected = false;

	bIsAttacking = true;

	BaseClass::PrimaryAttack();
}





