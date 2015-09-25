//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A class that has the ability to magically make money out of thin air
//
//===========================================================================//

#ifndef ECON_WEARABLE_H
#define ECON_WEARABLE_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#ifdef CLIENT_DLL
#include "particles_new.h"
#endif

#define MAX_WEARABLES_SENT_FROM_SERVER	5
#define PARTICLE_MODIFY_STRING_SIZE		128

#if defined( CLIENT_DLL )
#define CEconWearable C_EconWearable
#endif


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CEconWearable : public CEconEntity
{
	DECLARE_CLASS( CEconWearable, CEconEntity );
	DECLARE_NETWORKCLASS();

public:

	virtual void	Spawn( void );
	virtual int		GetSkin(void);
	virtual void	SetParticle(const char* name);
#ifdef GAME_DLL
	virtual void	Equip( CBasePlayer *pPlayer );
	virtual void	UnEquip( CBasePlayer *pPlayer );
#else
	virtual void	OnDataChanged(DataUpdateType_t type);
#endif

private:

#ifdef GAME_DLL
	CNetworkString(m_ParticleName, PARTICLE_MODIFY_STRING_SIZE);
#else
	char m_ParticleName[PARTICLE_MODIFY_STRING_SIZE];
	CNewParticleEffect *m_pUnusualParticle;
#endif

};

#endif