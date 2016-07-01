//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_wrench.h"
#include "decals.h"
#include "baseobject_shared.h"

// Client specific.
#ifdef CLIENT_DLL
	#include "c_tf_player.h"
// Server specific.
#else
	#include "tf_player.h"
	#include "variant_t.h"
#endif

//=============================================================================
//
// Weapon Wrench tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFWrench, tf_weapon_wrench )

//=============================================================================
//
// Weapon Wrench functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFWrench::CTFWrench()
{
}

#ifdef GAME_DLL
void CTFWrench::OnFriendlyBuildingHit( CBaseObject *pObject, CTFPlayer *pPlayer, Vector vecHitPos )
{
	// Did this object hit do any work? repair or upgrade?
	bool bUsefulHit = pObject->InputWrenchHit( pPlayer, this, vecHitPos );

	CDisablePredictionFiltering disabler;

	if ( bUsefulHit )
	{
		// play success sound
		WeaponSound( SPECIAL1 );
	}
	else
	{
		// play failure sound
		WeaponSound( SPECIAL2 );
	}
}
#endif

void CTFWrench::Smack( void )
{
	// see if we can hit an object with a higher range

	// Get the current player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	// Setup a volume for the melee weapon to be swung - approx size, so all melee behave the same.
	static Vector vecSwingMins( -18, -18, -18 );
	static Vector vecSwingMaxs( 18, 18, 18 );

	// Setup the swing range.
	Vector vecForward; 
	AngleVectors( pPlayer->EyeAngles(), &vecForward );
	Vector vecSwingStart = pPlayer->Weapon_ShootPosition();
	Vector vecSwingEnd = vecSwingStart + vecForward * 70;

	// only trace against objects

	// See if we hit anything.
	trace_t trace;	

	CTraceFilterIgnorePlayers traceFilter( NULL, COLLISION_GROUP_NONE );
	UTIL_TraceLine( vecSwingStart, vecSwingEnd, MASK_SOLID, &traceFilter, &trace );
	if ( trace.fraction >= 1.0 )
	{
		UTIL_TraceHull( vecSwingStart, vecSwingEnd, vecSwingMins, vecSwingMaxs, MASK_SOLID, &traceFilter, &trace );
	}

	// We hit, setup the smack.
	if ( trace.fraction < 1.0f &&
		 trace.m_pEnt &&
		 trace.m_pEnt->IsBaseObject() &&
		 trace.m_pEnt->GetTeamNumber() == pPlayer->GetTeamNumber() )
	{
#ifdef GAME_DLL
		OnFriendlyBuildingHit( dynamic_cast< CBaseObject * >( trace.m_pEnt ), pPlayer, trace.endpos );
#endif
	}
	else
	{
		// if we cannot, Smack as usual for player hits
		BaseClass::Smack();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFWrench::GetMeleeDamage( CBaseEntity *pTarget, int &iCustomDamage )
{
	float flDamage = BaseClass::GetMeleeDamage( pTarget, iCustomDamage );
	iCustomDamage = TF_DMG_WRENCH_FIX;

	return flDamage;
}
