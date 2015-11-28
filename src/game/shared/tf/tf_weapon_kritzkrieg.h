//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_KRITZKRIEG_H
#define TF_WEAPON_KRITZKRIEG_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"
#include "tf_weapon_medigun.h"

#if defined( CLIENT_DLL )
#define CWeaponKritzkrieg C_WeaponKritzkrieg
#endif

extern ConVar tf_medigun_lagcomp;

//=========================================================
// Beam healing gun
//=========================================================
class CWeaponKritzkrieg : public CTFWeaponBaseGun
{
	DECLARE_CLASS( CWeaponKritzkrieg, CTFWeaponBaseGun );
public:
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CWeaponKritzkrieg(void);
	~CWeaponKritzkrieg(void);

	virtual void	Precache();

	virtual bool	Deploy(void);
	virtual bool	Holster(CBaseCombatWeapon *pSwitchingTo);
	virtual void	UpdateOnRemove(void);
	virtual void	ItemHolsterFrame(void);
	virtual void	ItemPostFrame(void);
	virtual bool	Lower(void);
	virtual void	PrimaryAttack(void);
	virtual void	SecondaryAttack(void);
	virtual void	WeaponIdle(void);
	virtual void	DrainCharge(void);
	void			AddCharge(void);
	virtual void	WeaponReset(void);

	virtual float	GetTargetRange(void);
	virtual float	GetStickRange(void);
	virtual float	GetHealRate(void);
	virtual bool	AppliesModifier(void) { return true; }

	virtual int		GetWeaponID(void) const			{ return TF_WEAPON_KRITZKRIEG; }

	bool			IsReleasingCharge(void) { return (m_bChargeRelease && !m_bHolstered); }

	CBaseEntity		*GetHealTarget(void) { return m_hHealingTarget.Get(); }

#if defined( CLIENT_DLL )
	// Stop all sounds being output.
	void			StopHealSound(bool bStopHealingSound = true, bool bStopNoTargetSound = true);

	virtual void	OnDataChanged(DataUpdateType_t updateType);
	virtual void	ClientThink();
	void			UpdateEffects(void);
	void			ForceHealingTargetUpdate(void) { m_bUpdateHealingTargets = true; }

	void			ManageChargeEffect(void);
#else

	void			HealTargetThink(void);

#endif

	float			GetChargeLevel(void) { return m_flChargeLevel; }

private:
	bool					FindAndHealTargets(void);
	void					MaintainTargetInSlot();
	void					FindNewTargetForSlot();
	void					RemoveHealingTarget(bool bStopHealingSelf = false);
	bool					HealingTarget(CBaseEntity *pTarget);
	bool					CouldHealTarget(CBaseEntity *pTarget);
	bool					AllowedToHealTarget(CBaseEntity *pTarget);

public:

#ifdef GAME_DLL
	CNetworkHandle(CBaseEntity, m_hHealingTarget);
#else
	CNetworkHandle(C_BaseEntity, m_hHealingTarget);
#endif

protected:
	// Networked data.
	CNetworkVar(bool, m_bHealing);
	CNetworkVar(bool, m_bAttacking);

	double					m_flNextBuzzTime;
	float					m_flHealEffectLifetime;	// Count down until the healing effect goes off.
	float					m_flReleaseStartedAt;

	CNetworkVar(bool, m_bHolstered);
	CNetworkVar(bool, m_bChargeRelease);
	CNetworkVar(float, m_flChargeLevel);

	float					m_flNextTargetCheckTime;
	bool					m_bCanChangeTarget; // used to track the PrimaryAttack key being released for AutoHeal mode

#ifdef GAME_DLL
	CDamageModifier			m_DamageModifier;		// This attaches to whoever we're healing.
	bool					m_bHealingSelf;
#endif

#ifdef CLIENT_DLL
	bool					m_bPlayingSound;
	bool					m_bUpdateHealingTargets;
	struct healingtargeteffects_t
	{
		C_BaseEntity		*pTarget;
		CNewParticleEffect	*pEffect;
	};
	healingtargeteffects_t m_hHealingTargetEffect;

	float					m_flFlashCharge;
	bool					m_bOldChargeRelease;

	CNewParticleEffect	*m_pChargeEffect;
	CSoundPatch			*m_pChargedSound;
#endif

private:
	CWeaponKritzkrieg(const CWeaponKritzkrieg &);
};

#endif // TF_WEAPON_KRITZKRIEG_H
