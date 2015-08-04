//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Base class for Deathmatch powerups 
//
//=============================================================================//
#include "cbase.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_TFBaseDMPowerup : public C_BaseAnimating
{
public:
	DECLARE_CLASS(C_TFBaseDMPowerup, C_BaseAnimating);
	DECLARE_CLIENTCLASS();

	void	Spawn();
	void	ClientThink();

private:
	QAngle		qAngle;
};

IMPLEMENT_CLIENTCLASS_DT(C_TFBaseDMPowerup, DT_TFBaseDMPowerup, CTFBaseDMPowerup)
END_RECV_TABLE()

void C_TFBaseDMPowerup::Spawn()
{
	BaseClass::Spawn();
	qAngle = GetAbsAngles();
	ClientThink();
}

void C_TFBaseDMPowerup::ClientThink()
{
	qAngle.y += 90 * gpGlobals->frametime;
	if (qAngle.y >= 360)
		qAngle.y -= 360;

	SetAbsAngles(qAngle);

	SetNextClientThink(CLIENT_THINK_ALWAYS);
}
