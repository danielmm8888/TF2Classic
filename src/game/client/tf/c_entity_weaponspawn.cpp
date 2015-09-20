//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Deathmatch weapon spawning entity.
//
//=============================================================================//
#include "cbase.h"
#include "glow_outline_effect.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_WeaponSpawner : public C_BaseAnimating
{
public:
	DECLARE_CLASS(C_WeaponSpawner, C_BaseAnimating);
	DECLARE_CLIENTCLASS();

	void	Spawn();
	void	ClientThink();
	void	HandleGlowEffect();

private:
	QAngle		qAngle;
};

LINK_ENTITY_TO_CLASS(tf_weaponspawner, C_WeaponSpawner);

IMPLEMENT_CLIENTCLASS_DT(C_WeaponSpawner, DT_WeaponSpawner, CWeaponSpawner)
END_RECV_TABLE()

void C_WeaponSpawner::Spawn()
{
	BaseClass::Spawn();
	qAngle = GetAbsAngles();
	ClientThink();
}

void C_WeaponSpawner::ClientThink()
{
	qAngle.y += 90 * gpGlobals->frametime;
	if (qAngle.y >= 360)
		qAngle.y -= 360;

	SetAbsAngles(qAngle);

	HandleGlowEffect();

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

void C_WeaponSpawner::HandleGlowEffect()
{
	if (g_GlowObjectManager.HasGlowEffect(this))
		return;

	g_GlowObjectManager.RegisterGlowObject(this, Vector(1.0f, 0.94f, 0.0f), 1.0f, false, true, 0);
}



