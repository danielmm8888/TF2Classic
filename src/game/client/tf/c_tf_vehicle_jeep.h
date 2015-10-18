//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//
#ifndef C_TF_VEHICLE_H
#define C_TF_VEHICLE_H
#pragma once

#include "cbase.h"
#include "c_prop_vehicle.h"
#include "flashlighteffect.h"


class C_TFVehicle : public C_PropVehicleDriveable
{

	DECLARE_CLASS( C_TFVehicle, C_PropVehicleDriveable );

public:

	DECLARE_CLIENTCLASS();
	DECLARE_INTERPOLATION();

	C_TFVehicle();
	~C_TFVehicle();

public:

	void UpdateViewAngles( C_BasePlayer *pLocalPlayer, CUserCmd *pCmd );
	void DampenEyePosition( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles );

	void OnEnteredVehicle( C_BasePlayer *pPlayer );
	void Simulate( void );

private:

	void DampenForwardMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime );
	void DampenUpMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime );
	void ComputePDControllerCoefficients( float *pCoefficientsOut, float flFrequency, float flDampening, float flDeltaTime );

private:

	Vector		m_vecLastEyePos;
	Vector		m_vecLastEyeTarget;
	Vector		m_vecEyeSpeed;
	Vector		m_vecTargetSpeed;

	float		m_flViewAngleDeltaTime;

	float		m_flJeepFOV;
	CHeadlightEffect *m_pHeadlight;

	CNewParticleEffect *m_pSmokeParticle;
	CNewParticleEffect *m_pFireParticle;

	char	m_iszSmokeParticleName[128];
	char	m_iszFireParticleName[128];

	EHANDLE		m_hPlayerPassenger;
	bool		m_bHeadlightIsOn;
	bool		m_bEnableDamageEffects;
};

#endif // C_TF_VEHICLE_H
