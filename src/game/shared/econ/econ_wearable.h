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

#define MAX_WEARABLES_SENT_FROM_SERVER 5

#if defined( CLIENT_DLL )
#define CEconWearable C_EconWearable
#endif

enum
{
	UEFF_NONE = 0,
	UEFF_SUPERRARE_BURNING1,
	UEFF_SUPERRARE_CIRCLING_HEART,
	UEFF_SUPERRARE_CIRCLING_PEACESIGN,
	UEFF_SUPERRARE_CIRCLING_TF,
	UEFF_SUPERRARE_CONFETTI_GREEN,
	UEFF_SUPERRARE_GREENENERGY,
	UEFF_SUPERRARE_GHOSTS,
	UEFF_SUPERRARE_FLIES,
	UEFF_SUPERRARE_CONFETTI_PURPLE,
	UEFF_SUPERRARE_PURPLEENERGY,
	UEFF_SUPERRARE_BURNING2,
	UEFF_SUPERRARE_PLASMA1,
	UEFF_SUPERRARE_PLASMA2,
	UEFF_SUPERRARE_BEAMS1,

	UEFF_UNUSUAL_ORBIT_CARDS,

	UEFF_UTAUNT_FIREWORK,
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CEconWearable : public CEconEntity
{
	DECLARE_CLASS( CEconWearable, CEconEntity );
	DECLARE_NETWORKCLASS();

public:

	virtual void	Spawn( void );
	virtual int		GetSkin( void );
#ifdef GAME_DLL
	virtual void	SetSpecialParticleEffect( int iParticleType ) { m_nParticleType = iParticleType; }
	virtual void	Equip( CBasePlayer *pPlayer );
	virtual void	UnEquip( CBasePlayer *pPlayer );
#else
	virtual void	OnDataChanged( DataUpdateType_t type );
	virtual char*	GetParticleNameFromEnum( void );
#endif

private:

#ifdef GAME_DLL
	CNetworkVar( int, m_nParticleType );
#else
	int m_nParticleType;
	CNewParticleEffect *m_pUnusualParticle;
#endif

};

#endif