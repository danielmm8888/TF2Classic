//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: A class that has the ability to magically make money out of thin air
//
//===========================================================================//

#include "cbase.h"
#include "econ_wearable.h"
#include "tf_shareddefs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


IMPLEMENT_NETWORKCLASS_ALIASED( EconWearable, DT_EconWearable )

BEGIN_NETWORK_TABLE( CEconWearable, DT_EconWearable )
#ifdef GAME_DLL
	SendPropString( SENDINFO( m_ParticleName ) ),
#else
	RecvPropString(RECVINFO(m_ParticleName)),
#endif
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( econ_wearable, CEconWearable )

void CEconWearable::Spawn( void )
{
	BaseClass::Spawn();
	AddEffects( EF_BONEMERGE );
	AddEffects( EF_BONEMERGE_FASTCULL );
	SetCollisionGroup( COLLISION_GROUP_WEAPON );
	SetBlocksLOS( false );
}

int CEconWearable::GetSkin( void )
{
	switch ( GetTeamNumber() )
	{
		case TF_TEAM_BLUE:
			return 1;
			break;

		case TF_TEAM_RED:
			return 0;
			break;

		case TF_TEAM_GREEN:
			return 2;
			break;

		case TF_TEAM_YELLOW:
			return 3;
			break;

		default:
			return 0;
			break;
	}
}

void CEconWearable::SetParticle(const char* name)
{
#ifdef GAME_DLL
	Q_snprintf(m_ParticleName.GetForModify(), PARTICLE_MODIFY_STRING_SIZE, name);
#else
	Q_snprintf(m_ParticleName, PARTICLE_MODIFY_STRING_SIZE, name);
#endif
}

#ifdef GAME_DLL
void CEconWearable::Equip( CBasePlayer *pPlayer )
{
	if ( pPlayer )
	{
		FollowEntity( pPlayer, true );
		ChangeTeam( pPlayer->GetTeamNumber() );
	}
}

void CEconWearable::UnEquip( CBasePlayer *pPlayer )
{
	if ( pPlayer )
	{
		StopFollowingEntity();
	}
}
#else

void CEconWearable::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );
	if ( type == DATA_UPDATE_DATATABLE_CHANGED )
	{
		if (Q_stricmp(m_ParticleName, "") && !m_pUnusualParticle)
		{
			m_pUnusualParticle = ParticleProp()->Create(m_ParticleName, PATTACH_ABSORIGIN_FOLLOW);
		}
	}
}

#endif
