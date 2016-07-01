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
CREATE_SIMPLE_WEAPON_TABLE( TFChainsaw, tf_weapon_chainsaw )

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


void CTFChainsaw::PrimaryAttack( void )
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





