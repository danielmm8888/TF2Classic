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
	SendPropInt( SENDINFO( m_nParticleType ) ),
#else
	RecvPropInt( RECVINFO( m_nParticleType ) ),
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
		if ( !m_pUnusualParticle )
		{
			m_pUnusualParticle = ParticleProp()->Create( GetParticleNameFromEnum(), PATTACH_ABSORIGIN_FOLLOW );
		}
	}
}

char* CEconWearable::GetParticleNameFromEnum( void )
{
	switch ( m_nParticleType )
	{
		case UEFF_SUPERRARE_BURNING1:
			return "superrare_burning1";

		case UEFF_SUPERRARE_CIRCLING_HEART:
			return "superrare_circling_heart";

		case UEFF_SUPERRARE_GREENENERGY:
			return "superrare_greenenergy";

		case UEFF_UNUSUAL_ORBIT_CARDS:
			if ( GetTeamNumber() == TF_TEAM_BLUE )
				return "unusual_orbit_cards_teamcolor_blue";
			else if ( GetTeamNumber() == TF_TEAM_RED )
				return "unusual_orbit_cards_teamcolor_red";

		case UEFF_UTAUNT_FIREWORK:
			if ( GetTeamNumber() == TF_TEAM_BLUE )
				return "utaunt_firework_teamcolor_blue";
			else if ( GetTeamNumber() == TF_TEAM_RED )
				return "utaunt_firework_teamcolor_red";

		default:
			return "";
	}
}

#endif
