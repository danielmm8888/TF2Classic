//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_compound_bow.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#include "tf_gamestats.h"
#endif

#define TF_BOW_MIN_CHARGE_DAMAGE 50.0f
#define TF_BOW_MIN_CHARGE_VEL 1800
#define TF_BOW_MAX_CHARGE_VEL 2600
#define TF_BOW_MAX_CHARGE_TIME 1.0f
#define TF_BOW_CHARGE_TIRED_TIME 5.0f
#define TF_BOW_TIRED_SPREAD 6.0f

IMPLEMENT_NETWORKCLASS_ALIASED( TFCompoundBow, DT_WeaponCompoundBow )

BEGIN_NETWORK_TABLE( CTFCompoundBow, DT_WeaponCompoundBow )
#ifdef CLIENT_DLL
	RecvPropTime( RECVINFO( m_flChargeBeginTime ) ),
#else
	SendPropTime( SENDINFO( m_flChargeBeginTime ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFCompoundBow )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_flChargeBeginTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_compound_bow, CTFCompoundBow );
PRECACHE_WEAPON_REGISTER( tf_weapon_compound_bow );

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFCompoundBow )
END_DATADESC()
#endif

CTFCompoundBow::CTFCompoundBow()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::Precache( void )
{
	PrecacheScriptSound( "ArrowLight" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we holster
//-----------------------------------------------------------------------------
bool CTFCompoundBow::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	LowerBow();

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: Reset the charge when we deploy
//-----------------------------------------------------------------------------
bool CTFCompoundBow::Deploy( void )
{
	m_flChargeBeginTime = 0;

	return BaseClass::Deploy();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::WeaponReset( void )
{
	BaseClass::WeaponReset();

	LowerBow();
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::PrimaryAttack( void )
{
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

		SendWeaponAnim( ACT_ITEM2_VM_CHARGE );
		
		CTFPlayer *pOwner = GetTFPlayerOwner();
		if ( pOwner )
		{
			WeaponSound( SPECIAL1 );

			pOwner->m_Shared.AddCond( TF_COND_AIMING );
			pOwner->TeamFortress_SetSpeed();
		}
	}
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::SecondaryAttack( void )
{
	LowerBow();
}

// ---------------------------------------------------------------------------- -
// Purpose: Cancel shot charging.
//-----------------------------------------------------------------------------
void CTFCompoundBow::LowerBow( void )
{
	if ( m_flChargeBeginTime == 0.0f )
		return;

	m_flChargeBeginTime = 0.0f;

	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner )
	{
		pOwner->m_Shared.RemoveCond( TF_COND_AIMING );
		pOwner->TeamFortress_SetSpeed();
	}

	// Delay the next fire so they don't immediately start charging again.
	m_flNextPrimaryAttack = gpGlobals->curtime + 1.0f;

	SendWeaponAnim( ACT_ITEM2_VM_DRYFIRE );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::WeaponIdle( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();

	if ( m_flChargeBeginTime > 0 && pOwner->GetAmmoCount( GetPrimaryAmmoType() ) > 0 )
	{
		FireArrow();
	}
	else
	{
		BaseClass::WeaponIdle();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::ItemPostFrame( void )
{
	BaseClass::ItemPostFrame();

	if ( m_flChargeBeginTime != 0.0f )
	{
		float flChargeTime = gpGlobals->curtime - m_flChargeBeginTime;
		if ( flChargeTime >= TF_BOW_CHARGE_TIRED_TIME )
		{
			// Play flinching animation if it was charged for too long.
			if ( GetActivity() == ACT_ITEM2_VM_IDLE_2 )
			{
				SendWeaponAnim( ACT_ITEM2_VM_CHARGE_IDLE_3 );
			}
			else if ( IsViewModelSequenceFinished() )
			{
				SendWeaponAnim( ACT_ITEM2_VM_IDLE_3 );
			}
		}
		else if ( IsViewModelSequenceFinished() )
		{
			SendWeaponAnim( ACT_ITEM2_VM_IDLE_2 );
		}
	}
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::FireArrow( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	CalcIsAttackCritical();

	SendWeaponAnim( ACT_VM_PRIMARYATTACK );

	pPlayer->SetAnimation( PLAYER_ATTACK1 );
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_ATTACK_PRIMARY );

	pPlayer->m_Shared.RemoveCond( TF_COND_AIMING );
	pPlayer->TeamFortress_SetSpeed();

	FireProjectile( pPlayer );

#if !defined( CLIENT_DLL ) 
	pPlayer->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon( pPlayer, IsCurrentAttackACrit() );
#endif

	// Set next attack times.
	float flDelay = m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
	CALL_ATTRIB_HOOK_FLOAT( flDelay, mult_postfiredelay );
	m_flNextPrimaryAttack = gpGlobals->curtime + flDelay;

	SetWeaponIdleTime( gpGlobals->curtime + SequenceDuration() );

	m_flChargeBeginTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFCompoundBow::GetProjectileDamage( void )
{
	float flMaxChargeDamage = BaseClass::GetProjectileDamage();

	return RemapValClamped( ( gpGlobals->curtime - m_flChargeBeginTime ),
		0.0f,
		TF_BOW_MAX_CHARGE_TIME,
		TF_BOW_MIN_CHARGE_DAMAGE,
		TF_BOW_MIN_CHARGE_DAMAGE + flMaxChargeDamage );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFCompoundBow::GetProjectileSpeed( void )
{
	return RemapValClamped( ( gpGlobals->curtime - m_flChargeBeginTime ),
		0.0f,
		TF_BOW_MAX_CHARGE_TIME,
		TF_BOW_MIN_CHARGE_VEL,
		TF_BOW_MAX_CHARGE_VEL );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFCompoundBow::GetProjectileGravity( void )
{
	return RemapValClamped( ( gpGlobals->curtime - m_flChargeBeginTime ),
		0.0f,
		TF_BOW_MAX_CHARGE_TIME,
		0.5f,
		0.1f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCompoundBow::GetProjectileFireSetup( CTFPlayer *pPlayer, Vector vecOffset, Vector *vecSrc, QAngle *angForward, bool bHitTeammates, bool bUseHitboxes )
{
	BaseClass::GetProjectileFireSetup( pPlayer, vecOffset, vecSrc, angForward, bHitTeammates, bUseHitboxes );

	// Make the shot wildly inaccurate if charged for too long.
	if ( gpGlobals->curtime - m_flChargeBeginTime >= TF_BOW_CHARGE_TIRED_TIME )
	{
		float flSpread = TF_BOW_TIRED_SPREAD;
		angForward->x += RandomFloat( -flSpread, flSpread );
		angForward->y += RandomFloat( -flSpread, flSpread );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFCompoundBow::CalcIsAttackCriticalHelper( void )
{
	// No random critical hits.
	return false;
}

float CTFCompoundBow::GetChargeMaxTime( void )
{
	return TF_BOW_MAX_CHARGE_TIME;
}
