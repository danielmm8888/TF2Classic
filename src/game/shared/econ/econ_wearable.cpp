//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//========================================================================//

#include "cbase.h"
#include "econ_wearable.h"

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

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconWearable::Spawn( void )
{
	GetAttributeContainer()->InitializeAttributes( this );

	Precache();
	SetModel( m_Item.GetPlayerDisplayModel() );

	BaseClass::Spawn();

	AddEffects( EF_BONEMERGE );
	AddEffects( EF_BONEMERGE_FASTCULL );
	SetCollisionGroup( COLLISION_GROUP_WEAPON );
	SetBlocksLOS( false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CEconWearable::GetSkin( void )
{
	switch ( GetTeamNumber() )
	{
	case TF_TEAM_RED:
		return 0;
		break;

	case TF_TEAM_BLUE:
		return 1;
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

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconWearable::UpdateWearableBodyGroups( CBasePlayer *pPlayer )
{
	EconItemVisuals *visual = GetItem()->GetStaticData()->GetVisuals( GetTeamNumber() );
	for ( unsigned int i = 0; i < visual->player_bodygroups.Count(); i++ )
	{
		const char *szBodyGroupName = visual->player_bodygroups.GetElementName( i );

		if ( szBodyGroupName )
		{
			int iBodyGroup = pPlayer->FindBodygroupByName( szBodyGroupName );
			int iBodyGroupValue = visual->player_bodygroups.Element( i );

			pPlayer->SetBodygroup( iBodyGroup, iBodyGroupValue );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconWearable::SetParticle( const char* name )
{
#ifdef GAME_DLL
	Q_snprintf( m_ParticleName.GetForModify(), PARTICLE_MODIFY_STRING_SIZE, name );
#else
	Q_snprintf(m_ParticleName, PARTICLE_MODIFY_STRING_SIZE, name);
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconWearable::GiveTo( CBaseEntity *pEntity )
{
#ifdef GAME_DLL
	CBasePlayer *pPlayer = ToBasePlayer( pEntity );

	if ( pPlayer )
	{
		pPlayer->EquipWearable( this );
	}
#endif
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconWearable::Equip( CBasePlayer *pPlayer )
{
	if ( pPlayer )
	{
		FollowEntity( pPlayer, true );
		SetOwnerEntity( pPlayer );
		ChangeTeam( pPlayer->GetTeamNumber() );

		ReapplyProvision();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconWearable::UnEquip( CBasePlayer *pPlayer )
{
	if ( pPlayer )
	{
		StopFollowingEntity();

		SetOwnerEntity( NULL );
		ReapplyProvision();
	}
}

#else
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
ShadowType_t CEconWearable::ShadowCastType( void )
{
	if ( ShouldDraw() )
	{
		return SHADOWS_RENDER_TO_TEXTURE_DYNAMIC;
	}

	return SHADOWS_NONE;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CEconWearable::ShouldDraw( void )
{
	CBasePlayer *pOwner = ToBasePlayer( GetOwnerEntity() );

	if ( !pOwner )
		return false;

	if ( !pOwner->ShouldDrawThisPlayer() )
		return false;

	if ( !pOwner->IsAlive() )
		return false;

	return BaseClass::ShouldDraw();
}

#endif
