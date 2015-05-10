//=============================================================================//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "ai_basenpc_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#ifdef TF_CLASSIC
extern ConVar tf_max_health_boost;

int CAI_BaseNPC::GetMaxBuffedHealth( void )
{
	float flBoostMax = GetMaxHealth() * tf_max_health_boost.GetFloat();

	int iRoundDown = floor( flBoostMax / 5 );
	iRoundDown = iRoundDown * 5;

	return iRoundDown;
}
#endif