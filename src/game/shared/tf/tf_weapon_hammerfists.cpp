//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_hammerfists.h"
#include "decals.h"

#ifdef GAME_DLL
#include "tf_player.h"
#else
#include "c_tf_player.h"
#endif

CREATE_SIMPLE_WEAPON_TABLE( TFHammerfists, tf_weapon_hammerfists )

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFHammerfists::CTFHammerfists()
{

}

//-----------------------------------------------------------------------------
// Purpose: Destructor.
//-----------------------------------------------------------------------------
CTFHammerfists::~CTFHammerfists()
{

}


// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFHammerfists::PrimaryAttack( void )
{
	if ( !CanAttack() )
		return;

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	Punch();
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFHammerfists::SecondaryAttack()
{
	if ( !CanAttack() )
		return;

	m_iWeaponMode = TF_WEAPON_SECONDARY_MODE;

	Punch();
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFHammerfists::Punch( void )
{
	// Get the current player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	// Swing the weapon.
	Swing( pPlayer );

	m_flNextSecondaryAttack = m_flNextPrimaryAttack;

#if !defined( CLIENT_DLL ) 
	// We might or might not add Merc voice lines for ragemode, so just leave this in
	pPlayer->SpeakWeaponFire();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHammerfists::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	// Don't allow players to holster the weapon. It isn't meant to be holstered.
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Allow melee weapons to send different anim events
// Input  :  - 
//-----------------------------------------------------------------------------
void CTFHammerfists::SendPlayerAnimEvent( CTFPlayer *pPlayer )
{
	if ( IsCurrentAttackACrit() )
	{
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_SECONDARY );
	}
	else
	{
		pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHammerfists::DoViewModelAnimation( void )
{
	Activity act;

	if ( IsCurrentAttackACritical() )
	{
		act = ACT_VM_SWINGHARD;
	}
	else
	{
		act = ( m_iWeaponMode == TF_WEAPON_PRIMARY_MODE ) ? ACT_VM_HITLEFT : ACT_VM_HITRIGHT;
	}

	SendWeaponAnim( act );
}