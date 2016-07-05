//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: TODO: Redo this into a base grenade throwing weapon.
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_mirv.h"
#include "tf_fx_shared.h"
#include "tf_weapon_grenade_mirv.h"
#include "in_buttons.h"
#include "datacache/imdlcache.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include "prediction.h"
// Server specific.
#else
#include "tf_player.h"
#include "tf_gamestats.h"
#endif

#define TF_MIRV_MIN_CHARGE_VEL 600
#define TF_MIRV_MAX_CHARGE_VEL 2400
#define TF_MIRV_MAX_CHARGE_TIME 4.0f

//=============================================================================
//
// Weapon Mirv tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFWeaponMirv, DT_WeaponMirv )

BEGIN_NETWORK_TABLE( CTFWeaponMirv, DT_WeaponMirv )
#ifdef CLIENT_DLL
	RecvPropTime( RECVINFO( m_flChargeBeginTime ) ),
#else
	SendPropTime( SENDINFO( m_flChargeBeginTime ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFWeaponMirv )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_flChargeBeginTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_grenade_mirv, CTFWeaponMirv );
PRECACHE_WEAPON_REGISTER( tf_weapon_grenade_mirv );

//=============================================================================
//
// Weapon Mirv functions.
//

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFWeaponMirv::CTFWeaponMirv()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFWeaponMirv::~CTFWeaponMirv()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponMirv::Precache( void )
{
	PrecacheScriptSound( GetChargeSound() );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponMirv::Spawn( void )
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we holster
//-----------------------------------------------------------------------------
bool CTFWeaponMirv::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	m_flChargeBeginTime = 0;

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we deploy
//-----------------------------------------------------------------------------
bool CTFWeaponMirv::Deploy( void )
{
	m_flChargeBeginTime = 0;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponMirv::WeaponReset( void )
{
	BaseClass::WeaponReset();

	m_flChargeBeginTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponMirv::ItemPostFrame( void )
{
	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponMirv::PrimaryAttack( void )
{
	// Check for ammunition.
	if ( m_iClip1 <= 0 && UsesClipsForAmmo1() )
		return;

	// Are we capable of firing again?
	if ( m_flNextPrimaryAttack > gpGlobals->curtime )
		return;

	if ( !CanAttack() )
	{
		m_flChargeBeginTime = 0;
		return;
	}

	if ( m_flChargeBeginTime <= 0 )
	{
		// Set the weapon mode.
		m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

		// save that we had the attack button down
		m_flChargeBeginTime = gpGlobals->curtime;

		SendWeaponAnim( ACT_VM_PULLBACK );
	}
	else
	{
		float flTotalChargeTime = gpGlobals->curtime - m_flChargeBeginTime;

		if ( flTotalChargeTime >= TF_MIRV_MAX_CHARGE_TIME )
		{
			LaunchGrenade();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponMirv::WeaponIdle( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner && m_flChargeBeginTime > 0 && pOwner->GetAmmoCount( m_iPrimaryAmmoType ) > 0 )
	{
		LaunchGrenade();
	}
	else
	{
		BaseClass::WeaponIdle();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponMirv::LaunchGrenade( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return;

	CalcIsAttackCritical();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	pPlayer->SetAnimation( PLAYER_ATTACK1 );
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	CTFWeaponBaseGrenadeProj *pProjectile = static_cast<CTFWeaponBaseGrenadeProj*>( FireProjectile( pPlayer ) );

	if ( pProjectile )
	{
#ifdef GAME_DLL
		pProjectile->SetDetonateTimerLength( TF_MIRV_TIMER );
#endif
	}

#if !defined( CLIENT_DLL ) 
	pPlayer->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon( pPlayer, IsCurrentAttackACrit() );
#endif

	// Set next attack times.
	float flDelay = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
	CALL_ATTRIB_HOOK_FLOAT( flDelay, mult_postfiredelay );
	m_flNextPrimaryAttack = gpGlobals->curtime + flDelay;

	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	// Check the reload mode and behave appropriately.
	if ( m_bReloadsSingly )
	{
		m_iReloadMode.Set( TF_RELOAD_START );
	}

	m_flChargeBeginTime = 0;

	StartEffectBarRegen();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFWeaponMirv::GetProjectileSpeed( void )
{
	float flCharge = gpGlobals->curtime - m_flChargeBeginTime;
	return RemapValClamped( flCharge, 0.0f, TF_MIRV_MAX_CHARGE_TIME, TF_MIRV_MIN_CHARGE_VEL, TF_MIRV_MAX_CHARGE_VEL );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFWeaponMirv::GetChargeMaxTime( void )
{
	return TF_MIRV_MAX_CHARGE_TIME;
}
