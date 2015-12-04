//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//========================================================================//

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
	RecvPropString( RECVINFO( m_ParticleName ) ),
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

void CEconWearable::UpdateWearableBodyGroups( CBasePlayer *pPlayer )
{
	EconItemVisuals *visual = &m_Item.GetStaticData()->visual;
 	for ( unsigned int i = 0; i < visual->player_bodygroups.Count(); i++ )
	{
		const char *szBodyGroupName = visual->player_bodygroups.GetElementName(i);

		if ( szBodyGroupName )
		{
			int iBodyGroup = pPlayer->FindBodygroupByName( szBodyGroupName );
			int iBodyGroupValue = visual->player_bodygroups.Element(i);

			pPlayer->SetBodygroup( iBodyGroup, iBodyGroupValue );
		}
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

		IHasAttributes *pAttrib = pPlayer->GetHasAttributesInterfacePtr();

		if ( pAttrib )
		{
			pAttrib->GetAttributeManager()->AddProvider( this );
		}
	}
}

void CEconWearable::UnEquip( CBasePlayer *pPlayer )
{
	if ( pPlayer )
	{
		StopFollowingEntity();

		IHasAttributes *pAttrib = pPlayer->GetHasAttributesInterfacePtr();

		if ( pAttrib )
		{
			pAttrib->GetAttributeManager()->RemoveProvider( this );
		}
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
