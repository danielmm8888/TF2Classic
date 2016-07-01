//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_tranq.h"
#include "tf_fx_shared.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Tranq tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFTranq, tf_weapon_tranq )

//=============================================================================
//
// Weapon Tranq functions.
//
void CTFTranq::Precache()
{
	BaseClass::Precache();
#ifndef CLIENT_DLL
	PrecacheTeamParticles( "tranq_tracer_teamcolor_%s", true );
	PrecacheTeamParticles( "tranq_tracer_teamcolor_%s_crit", true );
	PrecacheParticleSystem( "tranq_distortion_trail" );
	PrecacheParticleSystem( "tranq_tracer_red" );
#endif
}