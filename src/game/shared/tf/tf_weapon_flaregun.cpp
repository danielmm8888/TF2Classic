//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_flaregun.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#include "tf_gamestats.h"
#endif

IMPLEMENT_NETWORKCLASS_ALIASED(TFFlareGun, DT_WeaponFlareGun)

BEGIN_NETWORK_TABLE(CTFFlareGun, DT_WeaponFlareGun)
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA(CTFFlareGun)
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS(tf_weapon_flaregun, CTFFlareGun);
PRECACHE_WEAPON_REGISTER(tf_weapon_flaregun);

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC(CTFFlareGun)
END_DATADESC()
#endif

#define TF_FLARE_MIN_VEL 1200

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFFlareGun::CTFFlareGun()
{
	m_bReloadsSingly = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlareGun::Spawn(void)
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Primary fire function
//-----------------------------------------------------------------------------
void CTFFlareGun::PrimaryAttack(void)
{
	// Check for ammunition.
	if (m_iClip1 <= 0 && m_iClip1 != -1)
		return;

	// Are we capable of firing again?
	if (m_flNextPrimaryAttack > gpGlobals->curtime)
		return;

	if (!CanAttack())
		return;

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	LaunchProjectile();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFlareGun::LaunchProjectile(void)
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = ToTFPlayer(GetPlayerOwner());
	if (!pPlayer)
		return;

	CalcIsAttackCritical();

	SendWeaponAnim(ACT_VM_PRIMARYATTACK);

	pPlayer->SetAnimation(PLAYER_ATTACK1);
	pPlayer->DoAnimationEvent(PLAYERANIMEVENT_ATTACK_PRIMARY);

	FireProjectile(pPlayer);

#if !defined( CLIENT_DLL ) 
	pPlayer->SpeakWeaponFire();
	CTF_GameStats.Event_PlayerFiredWeapon(pPlayer, IsCurrentAttackACrit());
#endif

	// Set next attack times.
	m_flNextPrimaryAttack = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData(m_iWeaponMode).m_flTimeFireDelay;

	SetWeaponIdleTime(gpGlobals->curtime + SequenceDuration());

	// Check the reload mode and behave appropriately.
	if (m_bReloadsSingly)
	{
		m_iReloadMode.Set(TF_RELOAD_START);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the projectile speed used by FireProjectile in CTFWeaponBase
//-----------------------------------------------------------------------------
float CTFFlareGun::GetProjectileSpeed(void)
{
	return TF_FLARE_MIN_VEL;
}

