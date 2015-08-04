//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. ========//
//
// Purpose: Flare used by the flaregun.
//
//=============================================================================//
#include "cbase.h"
#include "tf_weaponbase.h"
#include "tf_gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "tf_weapon_grenade_flare.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "IEffects.h"
// Server specific.
#else
#include "tf_player.h"
#include "items.h"
#include "tf_weaponbase_grenadeproj.h"
#include "soundent.h"
#include "KeyValues.h"
#include "IEffects.h"
#include "props.h"
#include "func_respawnroom.h"
#endif

#ifdef GAME_DLL

#define TF_WEAPON_FLARE_MODEL		"models/weapons/w_models/w_grenade_grenadelauncher.mdl"
#define TF_WEAPON_FLARE_DETONATE_TIME 99.0f

BEGIN_DATADESC(CTFGrenadeFlareProjectile)
END_DATADESC()

LINK_ENTITY_TO_CLASS(tf_projectile_flare, CTFGrenadeFlareProjectile);
PRECACHE_WEAPON_REGISTER(tf_projectile_flare);

#endif
IMPLEMENT_NETWORKCLASS_ALIASED(TFGrenadeFlareProjectile, DT_TFProjectile_Flare)
BEGIN_NETWORK_TABLE(CTFGrenadeFlareProjectile, DT_TFProjectile_Flare)
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFGrenadeFlareProjectile::CTFGrenadeFlareProjectile()
{

}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeFlareProjectile::Precache()
{
	PrecacheModel(TF_WEAPON_FLARE_MODEL);
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function
//-----------------------------------------------------------------------------
void CTFGrenadeFlareProjectile::Spawn()
{
	SetModel(TF_WEAPON_FLARE_MODEL);
	SetDetonateTimerLength(TF_WEAPON_FLARE_DETONATE_TIME);
	SetTouch(&CTFGrenadeFlareProjectile::FlareTouch);
	BaseClass::Spawn();

	m_flCreationTime = gpGlobals->curtime;

	// We want to get touch functions called so we can damage enemy players
	AddSolidFlags(FSOLID_TRIGGER);
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeFlareProjectile::Detonate()
{
	if (ShouldNotDetonate())
	{
		RemoveGrenade();
		return;
	}

	// Old napalmnade-style flare code by iamgoofball.
	/*float flRadius = 180;

	// Light everyone on fire in the radius.
	Vector vecOrigin = GetAbsOrigin() + Vector(0, 0, 0);

	CBaseEntity *pListOfNearbyEntities[32];
	int iNumberOfNearbyEntities = UTIL_EntitiesInSphere(pListOfNearbyEntities, 32, vecOrigin, flRadius, FL_CLIENT);
	for (int i = 0; i < iNumberOfNearbyEntities; i++)
	{
		CTFPlayer *pPlayer = ToTFPlayer(pListOfNearbyEntities[i]);
		CTFPlayer *pOwner = dynamic_cast <CTFPlayer*>(GetThrower());

		if (pPlayer && pPlayer->GetTeamNumber() && pPlayer->GetTeamNumber() != GetTeamNumber() || pPlayer == pOwner)
		{
			CTakeDamageInfo info(pOwner, pPlayer, 10, DMG_IGNITE, TF_DMG_CUSTOM_BURNING);
			info.SetReportedPosition(pOwner->GetAbsOrigin());

			pPlayer->TakeDamage(info);
		}
	}*/

	//Add some fancy impact effects here

	UTIL_Remove(this);
	return;
	
	//BaseClass::Detonate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeFlareProjectile::FlareTouch(CBaseEntity *pOther)
{
	if (pOther == GetThrower())
		return;

	// Verify a correct "other."
	if (!pOther->IsSolid() || pOther->IsSolidFlagSet(FSOLID_VOLUME_CONTENTS))
	return;

	// Handle hitting skybox (disappear).
	trace_t pTrace;
	Vector velDir = GetAbsVelocity();
	VectorNormalize(velDir);
	Vector vecSpot = GetAbsOrigin() - velDir * 32;
	UTIL_TraceLine(vecSpot, vecSpot + velDir * 64, MASK_SOLID, this, COLLISION_GROUP_NONE, &pTrace);

	if (pTrace.fraction < 1.0 && pTrace.surface.flags & SURF_SKY)
	{
		UTIL_Remove(this);
		return;
	}

	// Blow up if we hit an enemy we can damage
	if (pOther->IsPlayer() && (pOther->GetTeamNumber() != GetTeamNumber()) && (pOther->m_takedamage != DAMAGE_NO))
	{
		CTFPlayer *pOwner = dynamic_cast <CTFPlayer*>(GetThrower());
		CTFPlayer *pPlayer = dynamic_cast <CTFPlayer*>(pOther);
		CTakeDamageInfo info(pOwner, pPlayer, 10, DMG_IGNITE, TF_DMG_CUSTOM_BURNING);
		info.SetReportedPosition(pOwner->GetAbsOrigin());
		pPlayer->TakeDamage(info);
	}
	Detonate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeFlareProjectile::VPhysicsCollision(int index, gamevcollisionevent_t *pEvent)
{
	BaseClass::VPhysicsCollision(index, pEvent);

	int otherIndex = !index;
	CBaseEntity *pHitEntity = pEvent->pEntities[otherIndex];

	if (!pHitEntity)
		return;

	if (pHitEntity->IsWorld())
		Detonate();

	return;
}
#else
//-----------------------------------------------------------------------------
// Purpose: Don't draw if we haven't yet gone past our original spawn point
// Input  : flags - 
//-----------------------------------------------------------------------------
int CTFGrenadeFlareProjectile::DrawModel(int flags)
{
	if ( gpGlobals->curtime < ( m_flCreationTime + 0.1 ) )
		return 0;

	return BaseClass::DrawModel( flags );
}
#endif

#ifdef GAME_DLL
CTFGrenadeFlareProjectile* CTFGrenadeFlareProjectile::Create(const Vector &position, const QAngle &angles,
	const Vector &velocity, const AngularImpulse &angVelocity,
	CBaseCombatCharacter *pOwner, const CTFWeaponInfo &weaponInfo)
{
	CTFGrenadeFlareProjectile *pGrenade = static_cast<CTFGrenadeFlareProjectile*>(CBaseEntity::CreateNoSpawn("tf_projectile_flare", position, angles, pOwner));
	if (pGrenade)
	{
		DispatchSpawn(pGrenade);

		pGrenade->InitGrenade(velocity, angVelocity, pOwner, weaponInfo);

		pGrenade->ApplyLocalAngularVelocityImpulse(angVelocity);
	}

	return pGrenade;
}
#endif