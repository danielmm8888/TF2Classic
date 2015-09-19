//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Teleporter Object
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"

#include "tf_obj_teleporter.h"
#include "engine/IEngineSound.h"
#include "tf_player.h"
#include "tf_team.h"
#include "tf_gamerules.h"
#include "world.h"
#include "explode.h"
#include "particle_parse.h"
#include "tf_gamestats.h"
#include "tf_weapon_sniperrifle.h"
#include "tf_fx.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Ground placed version
#define TELEPORTER_MODEL_ENTRANCE_PLACEMENT	"models/buildables/teleporter_blueprint_enter.mdl"
#define TELEPORTER_MODEL_EXIT_PLACEMENT		"models/buildables/teleporter_blueprint_exit.mdl"
#define TELEPORTER_MODEL_BUILDING			"models/buildables/teleporter.mdl"
#define TELEPORTER_MODEL_LIGHT				"models/buildables/teleporter_light.mdl"

#define TELEPORTER_MINS			Vector( -24, -24, 0)
#define TELEPORTER_MAXS			Vector( 24, 24, 12)	

IMPLEMENT_SERVERCLASS_ST( CObjectTeleporter, DT_ObjectTeleporter )
	SendPropInt( SENDINFO(m_iState), 5 ),
	SendPropTime( SENDINFO(m_flRechargeTime) ),
	SendPropInt( SENDINFO(m_iTimesUsed), 6 ),
	SendPropFloat( SENDINFO(m_flYawToExit), 8, 0, 0.0, 360.0f ),
END_SEND_TABLE()

BEGIN_DATADESC( CObjectTeleporter )
	DEFINE_THINKFUNC( TeleporterThink ),
	DEFINE_ENTITYFUNC( TeleporterTouch ),
END_DATADESC()

PRECACHE_REGISTER( obj_teleporter );

#define TELEPORTER_MAX_HEALTH	150

#define TELEPORTER_THINK_CONTEXT				"TeleporterContext"

#define BUILD_TELEPORTER_DAMAGE					25		// how much damage an exploding teleporter can do

#define BUILD_TELEPORTER_FADEOUT_TIME			0.25	// time to teleport a player out (teleporter with full health)
#define BUILD_TELEPORTER_FADEIN_TIME			0.25	// time to teleport a player in (teleporter with full health)

#define BUILD_TELEPORTER_NEXT_THINK				0.05

#define BUILD_TELEPORTER_PLAYER_OFFSET			20		// how far above the origin of the teleporter to place a player

#define BUILD_TELEPORTER_EFFECT_TIME			12.0	// seconds that player glows after teleporting

ConVar tf_teleporter_fov_start( "tf_teleporter_fov_start", "120", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY, "Starting FOV for teleporter zoom.", true, 1, false, 0 );
ConVar tf_teleporter_fov_time( "tf_teleporter_fov_time", "0.5", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY, "How quickly to restore FOV after teleport.", true, 0.0, false, 0 );

LINK_ENTITY_TO_CLASS( obj_teleporter,	CObjectTeleporter );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CObjectTeleporter::CObjectTeleporter()
{
	SetMaxHealth( TELEPORTER_MAX_HEALTH );
	m_iHealth = TELEPORTER_MAX_HEALTH;
	UseClientSideAnimation();
	SetType( OBJ_TELEPORTER );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectTeleporter::Spawn()
{
	SetSolid( SOLID_BBOX );
	
	m_takedamage = DAMAGE_NO;

	SetState( TELEPORTER_STATE_BUILDING );

	m_flNextEnemyTouchHint = gpGlobals->curtime;

	m_flYawToExit = 0;

	if (GetObjectMode() == TELEPORTER_TYPE_ENTRANCE)
		SetModel(TELEPORTER_MODEL_ENTRANCE_PLACEMENT);
	else
		SetModel(TELEPORTER_MODEL_EXIT_PLACEMENT);

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Receive a teleporting player 
//-----------------------------------------------------------------------------
void CObjectTeleporter::TeleporterReceive( CTFPlayer *pPlayer, float flDelay )
{
	if ( !pPlayer )
		return;

	SetTeleportingPlayer( pPlayer );

	Vector origin = GetAbsOrigin();
	CPVSFilter filter( origin );

	int iTeam = pPlayer->GetTeamNumber();
	if ( pPlayer->IsPlayerClass( TF_CLASS_SPY ) && pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
	{
		iTeam = pPlayer->m_Shared.GetDisguiseTeam();
	}

	switch( iTeam )
	{
	case TF_TEAM_RED:
		TE_TFParticleEffect( filter, 0.0, "teleportedin_red", origin, vec3_angle );
		break;
	case TF_TEAM_BLUE:
		TE_TFParticleEffect( filter, 0.0, "teleportedin_blue", origin, vec3_angle );
		break;
	case TF_TEAM_GREEN:
		TE_TFParticleEffect( filter, 0.0, "teleportedin_green", origin, vec3_angle );
		break;
	case TF_TEAM_YELLOW:
		TE_TFParticleEffect( filter, 0.0, "teleportedin_yellow", origin, vec3_angle );
		break;
	default:
		break;
	}

	EmitSound( "Building_Teleporter.Receive" );

	SetState( TELEPORTER_STATE_RECEIVING );
	m_flMyNextThink = gpGlobals->curtime + BUILD_TELEPORTER_FADEOUT_TIME;

	m_iTimesUsed++;
}

//-----------------------------------------------------------------------------
// Teleport the passed player to our destination
//-----------------------------------------------------------------------------
void CObjectTeleporter::TeleporterSend( CTFPlayer *pPlayer )
{
	if ( !pPlayer )
		return;

	SetTeleportingPlayer( pPlayer );
	pPlayer->m_Shared.AddCond( TF_COND_SELECTED_TO_TELEPORT );

	Vector origin = GetAbsOrigin();
	CPVSFilter filter( origin );

	int iTeam = pPlayer->GetTeamNumber();
	if ( pPlayer->IsPlayerClass( TF_CLASS_SPY ) && pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
	{
		iTeam = pPlayer->m_Shared.GetDisguiseTeam();
	}

	switch( iTeam )
	{
	case TF_TEAM_RED:
		TE_TFParticleEffect( filter, 0.0, "teleported_red", origin, vec3_angle );
		TE_TFParticleEffect( filter, 0.0, "player_sparkles_red", origin, vec3_angle, pPlayer, PATTACH_POINT );
		break;
	case TF_TEAM_BLUE:
		TE_TFParticleEffect( filter, 0.0, "teleported_blue", origin, vec3_angle );
		TE_TFParticleEffect( filter, 0.0, "player_sparkles_blue", origin, vec3_angle, pPlayer, PATTACH_POINT );
		break;
	case TF_TEAM_GREEN:
		TE_TFParticleEffect(filter, 0.0, "teleported_green", origin, vec3_angle);
		TE_TFParticleEffect(filter, 0.0, "player_sparkles_green", origin, vec3_angle, pPlayer, PATTACH_POINT);
		break;
	case TF_TEAM_YELLOW:
		TE_TFParticleEffect(filter, 0.0, "teleported_yellow", origin, vec3_angle);
		TE_TFParticleEffect(filter, 0.0, "player_sparkles_yellow", origin, vec3_angle, pPlayer, PATTACH_POINT);
		break;
	default:
		break;
	}

	EmitSound( "Building_Teleporter.Send" );

	SetState( TELEPORTER_STATE_SENDING );
	m_flMyNextThink = gpGlobals->curtime + 0.1;

	m_iTimesUsed++;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectTeleporter::SetModel( const char *pModel )
{
	BaseClass::SetModel( pModel );

	// Reset this after model change
	UTIL_SetSize( this, TELEPORTER_MINS, TELEPORTER_MAXS );

	CreateBuildPoints();

	ReattachChildren();

	m_iDirectionBodygroup = FindBodygroupByName( "teleporter_direction" );
	m_iBlurBodygroup = FindBodygroupByName( "teleporter_blur" );

	if ( m_iBlurBodygroup >= 0 )
	{
		SetBodygroup( m_iBlurBodygroup, 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Start building the object
//-----------------------------------------------------------------------------
bool CObjectTeleporter::StartBuilding( CBaseEntity *pBuilder )
{
	SetModel( TELEPORTER_MODEL_BUILDING );

	return BaseClass::StartBuilding( pBuilder );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool CObjectTeleporter::IsPlacementPosValid( void )
{
	bool bResult = BaseClass::IsPlacementPosValid();

	if ( !bResult )
	{
		return false;
	}

	// m_vecBuildOrigin is the proposed build origin

	// start above the teleporter position
	Vector vecTestPos = m_vecBuildOrigin;
	vecTestPos.z += TELEPORTER_MAXS.z;

	// make sure we can fit a player on top in this pos
	trace_t tr;
	UTIL_TraceHull( vecTestPos, vecTestPos, VEC_HULL_MIN, VEC_HULL_MAX, MASK_SOLID | CONTENTS_PLAYERCLIP, this, COLLISION_GROUP_PLAYER_MOVEMENT, &tr );

	return ( tr.fraction >= 1.0 );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CObjectTeleporter::StartUpgrading(void)
{
	BaseClass::StartUpgrading( );

	SetState( TELEPORTER_STATE_UPGRADING );
}

void CObjectTeleporter::FinishUpgrading( void )
{
	SetState( TELEPORTER_STATE_IDLE );

	//if ( GetMatchingTeleporter() )
		//CopyUpgradeStateToMatch( GetMatchingTeleporter(), false );

	BaseClass::FinishUpgrading();
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CObjectTeleporter::OnGoActive( void )
{
	CTFPlayer *pBuilder = GetBuilder();

	Assert( pBuilder );

	if ( !pBuilder )
		return;

	SetModel( TELEPORTER_MODEL_LIGHT );
	SetActivity( ACT_OBJ_IDLE );

	SetContextThink( &CObjectTeleporter::TeleporterThink, gpGlobals->curtime + 0.1, TELEPORTER_THINK_CONTEXT );
	SetTouch( &CObjectTeleporter::TeleporterTouch );

	SetState( TELEPORTER_STATE_IDLE );

	BaseClass::OnGoActive();

	SetPlaybackRate( 0.0f );
	m_flLastStateChangeTime = 0.0f;	// used as a flag to initialize the playback rate to 0 in the first DeterminePlaybackRate
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectTeleporter::Precache()
{
	BaseClass::Precache();

	// Precache Object Models
	int iModelIndex;

	PrecacheModel( TELEPORTER_MODEL_ENTRANCE_PLACEMENT );
	PrecacheModel( TELEPORTER_MODEL_EXIT_PLACEMENT );	

	iModelIndex = PrecacheModel( TELEPORTER_MODEL_BUILDING );
	PrecacheGibsForModel( iModelIndex );

	iModelIndex = PrecacheModel( TELEPORTER_MODEL_LIGHT );
	PrecacheGibsForModel( iModelIndex );

	// Precache Sounds
	PrecacheScriptSound( "Building_Teleporter.Ready" );
	PrecacheScriptSound( "Building_Teleporter.Send" );
	PrecacheScriptSound( "Building_Teleporter.Receive" );
	PrecacheScriptSound( "Building_Teleporter.Spin" );

	PrecacheParticleSystem( "teleporter_red_charged" );
	PrecacheParticleSystem( "teleporter_blue_charged" );
	PrecacheParticleSystem( "teleporter_green_charged");
	PrecacheParticleSystem( "teleporter_yellow_charged");
	PrecacheParticleSystem( "teleporter_red_entrance" );
	PrecacheParticleSystem( "teleporter_blue_entrance" );
	PrecacheParticleSystem( "teleporter_green_entrance" );
	PrecacheParticleSystem( "teleporter_yellow_entrance" );
	PrecacheParticleSystem( "teleporter_red_exit" );
	PrecacheParticleSystem( "teleporter_blue_exit" );
	PrecacheParticleSystem( "teleporter_green_exit" );
	PrecacheParticleSystem( "teleporter_yellow_exit" );
	PrecacheParticleSystem( "teleporter_arms_circle_red" );
	PrecacheParticleSystem( "teleporter_arms_circle_blue" );
	PrecacheParticleSystem( "teleporter_arms_circle_green" );
	PrecacheParticleSystem( "teleporter_arms_circle_yellow" );
	PrecacheParticleSystem( "tpdamage_1" );
	PrecacheParticleSystem( "tpdamage_2" );
	PrecacheParticleSystem( "tpdamage_3" );
	PrecacheParticleSystem( "tpdamage_4" );
	PrecacheParticleSystem( "teleported_red" );
	PrecacheParticleSystem( "player_sparkles_red" );
	PrecacheParticleSystem( "teleported_blue" );
	PrecacheParticleSystem( "player_sparkles_blue" );
	PrecacheParticleSystem( "teleported_green" );
	PrecacheParticleSystem( "player_sparkles_green" );
	PrecacheParticleSystem( "teleported_yellow" );
	PrecacheParticleSystem( "player_sparkles_yellow" );
	PrecacheParticleSystem( "teleportedin_red" );
	PrecacheParticleSystem( "teleportedin_blue" );
	PrecacheParticleSystem( "teleportedin_green" );
	PrecacheParticleSystem( "teleportedin_yellow" );
}

//-----------------------------------------------------------------------------
//
//-----------------------------------------------------------------------------
void CObjectTeleporter::TeleporterTouch( CBaseEntity *pOther )
{
	if ( IsDisabled() )
	{
		return;
	}

	// if it's not a player, ignore
	if ( !pOther->IsPlayer() )
		return;

	CTFPlayer *pPlayer = ToTFPlayer( pOther );

	CTFPlayer *pBuilder = GetBuilder();

	Assert( pBuilder );

	if ( !pBuilder )
	{
		return;
	}

	// is this an entrance and do we have an exit?
	if ( GetObjectMode() == TELEPORTER_TYPE_ENTRANCE )
	{		
		if ( ( m_iState == TELEPORTER_STATE_READY ) )
		{
			// are we able to teleport?
			if ( !PlayerCanBeTeleported( pPlayer ) )
			{
				if ( pPlayer->HasTheFlag() )
				{
					// If they have the flag, print a warning that you can't tele with the flag
					CSingleUserRecipientFilter filter( pPlayer );
					TFGameRules()->SendHudNotification( filter, HUD_NOTIFY_NO_TELE_WITH_FLAG );
				}

				return;
			}

			// get the velocity of the player touching the teleporter
			if ( pPlayer->GetAbsVelocity().Length() < 5.0 )
			{
				CObjectTeleporter *pDest = GetMatchingTeleporter();

				if ( pDest )
				{
					TeleporterSend( pPlayer );
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Receive a teleporting player 
//-----------------------------------------------------------------------------
bool CObjectTeleporter::IsMatchingTeleporterReady( void )
{
	if ( m_hMatchingTeleporter.Get() == NULL )
	{
		m_hMatchingTeleporter = FindMatch();
	}

	if ( m_hMatchingTeleporter &&
		m_hMatchingTeleporter->GetState() != TELEPORTER_STATE_BUILDING && 
		!m_hMatchingTeleporter->IsDisabled() )
		return true;

	return false;
}

bool CObjectTeleporter::PlayerCanBeTeleported( CTFPlayer *pSender )
{
	bool bResult = false;
	//CTFPlayer *pBuilder = GetBuilder();

	if ( pSender )
	{
		if (!pSender->HasTheFlag())
		{
			int iTeamNumber = pSender->GetTeamNumber();

			// Don't teleport enemies (unless it's a spy)
			if ( GetTeamNumber() != pSender->GetTeamNumber() && pSender->IsPlayerClass( TF_CLASS_SPY ) )
				iTeamNumber = pSender->m_Shared.GetDisguiseTeam();

			if ( GetTeamNumber() == iTeamNumber )
			{
				bResult = true;
			}
		}
	}

	return bResult;
}

bool CObjectTeleporter::IsSendingPlayer( CTFPlayer *pSender )
{
	bool bResult = false;

	if ( pSender && m_hTeleportingPlayer.Get() )
	{
		bResult = m_hTeleportingPlayer.Get() == pSender;
	}
	return bResult;
}

CObjectTeleporter *CObjectTeleporter::CopyUpgradeStateToMatch( CObjectTeleporter *pObjToCopyTo, bool bCopyTo )
{
	// I don't even know

	CObjectTeleporter *pObjToCopyFrom = this;

	/*	while ( true )
	{
		pObjToCopyFrom = pObjToCopyTo;

		if ( !pObjToCopyTo )
			break;

		if (!bCopyTo)
		{*/
		pObjToCopyTo->m_iUpgradeMetal = m_iUpgradeMetal;
		pObjToCopyTo->m_iMaxHealth = m_iMaxHealth;
		pObjToCopyTo->m_iUpgradeMetalRequired = m_iUpgradeMetalRequired;
		pObjToCopyTo->m_iUpgradeLevel = m_iUpgradeLevel;	

			/**(pObjToCopyTo + 632) = *(this + 632);
			*(pObjToCopyTo + 629) = *(this + 629);
			*(pObjToCopyTo + 630) = *(this + 630);
			*(pObjToCopyTo + 631) = *(this + 631);
			*(pObjToCopyTo + 633) = *(this + 633);
			*(pObjToCopyTo + 634) = *(this + 634);*/
			/*return pObjToCopyFrom;
		}
		bCopyTo = false;
	}*/

	return pObjToCopyFrom;
}

bool CObjectTeleporter::CheckUpgradeOnHit( CTFPlayer *pPlayer )
{
	bool bUpgradeSuccesfull = false; 

	if ( BaseClass::CheckUpgradeOnHit( pPlayer ) )
	{
		CObjectTeleporter *pMatchingTeleporter = GetMatchingTeleporter(); 

		if ( pMatchingTeleporter )
		{
			pMatchingTeleporter->m_iUpgradeMetal = m_iUpgradeMetal;
			if ( pMatchingTeleporter->m_iUpgradeLevel != m_iUpgradeLevel )
			{
				pMatchingTeleporter->StartUpgrading();
			}
			//CopyUpgradeStateToMatch( pMatchingTeleporter, false );
		}

		bUpgradeSuccesfull = true;
	}

	return bUpgradeSuccesfull;
}


/*

InitializeMapPlacedObject*/

CObjectTeleporter *CObjectTeleporter::GetMatchingTeleporter( void )
{
	return m_hMatchingTeleporter.Get();
}


bool CObjectTeleporter::InputWrenchHit( CTFPlayer *pPlayer, CTFWrench *pWrench, Vector vecHitPos )
{
	if ( HasSapper() && GetMatchingTeleporter() )
	{
		// do damage to any attached buildings
		CTakeDamageInfo info( pPlayer, pPlayer, 65, DMG_CLUB, TF_DMG_WRENCH_FIX );

		IHasBuildPoints *pBPInterface = dynamic_cast< IHasBuildPoints * >( GetMatchingTeleporter() );
		int iNumObjects = pBPInterface->GetNumObjectsOnMe();
		for ( int iPoint=0; iPoint < iNumObjects; iPoint++ )
		{
			CBaseObject *pObject = GetBuildPointObject( iPoint );

			if ( pObject && pObject->IsHostileUpgrade() )
				pObject->TakeDamage( info );

		}
	}

	return BaseClass::InputWrenchHit( pPlayer, pWrench, vecHitPos );
}

void CObjectTeleporter::DeterminePlaybackRate( void )
{
	float flPlaybackRate = GetPlaybackRate();

	bool bWasBelowFullSpeed = ( flPlaybackRate < 1.0f );

	if ( IsBuilding() )
	{
		// Default half rate, author build anim as if one player is building
		SetPlaybackRate( GetConstructionMultiplier() * 0.5 );	
	}
	else if ( IsPlacing() )
	{
		SetPlaybackRate( 1.0f );
	}
	else
	{
		float flFrameTime = 0.1;	// BaseObjectThink delay

		switch( m_iState )
		{
		case TELEPORTER_STATE_READY:	
			{
				// spin up to 1.0 from whatever we're at, at some high rate
				flPlaybackRate = Approach( 1.0f, flPlaybackRate, 0.5f * flFrameTime );
			}
			break;

		case TELEPORTER_STATE_RECHARGING:
			{
				// Recharge - spin down to low and back up to full speed over 10 seconds

				// 0 -> 4, spin to low
				// 4 -> 6, stay at low
				// 6 -> 10, spin to 1.0

				float flTimeSinceChange = gpGlobals->curtime - m_flLastStateChangeTime;

				float flLowSpinSpeed = 0.15f;

				if ( flTimeSinceChange <= 4.0f )
				{
					flPlaybackRate = RemapVal( gpGlobals->curtime,
						m_flLastStateChangeTime,
						m_flLastStateChangeTime + 4.0f,
						1.0f,
						flLowSpinSpeed );
				}
				else if ( flTimeSinceChange > 4.0f && flTimeSinceChange <= 6.0f )
				{
					flPlaybackRate = flLowSpinSpeed;
				}
				else
				{
					flPlaybackRate = RemapVal( gpGlobals->curtime,
						m_flLastStateChangeTime + 6.0f,
						m_flLastStateChangeTime + 10.0f,
						flLowSpinSpeed,
						1.0f );
				}
			}		
			break;

		default:
			{
				if ( m_flLastStateChangeTime <= 0.0f )
				{
					flPlaybackRate = 0.0f;
				}
				else
				{
					// lost connect - spin down to 0.0 from whatever we're at, slowish rate
					flPlaybackRate = Approach( 0.0f, flPlaybackRate, 0.25f * flFrameTime );
				}
			}
			break;
		}

		SetPlaybackRate( flPlaybackRate );
	}

	bool bBelowFullSpeed = ( GetPlaybackRate() < 1.0f );

	if ( m_iBlurBodygroup >= 0 && bBelowFullSpeed != bWasBelowFullSpeed )
	{
		if ( bBelowFullSpeed )
		{
			SetBodygroup( m_iBlurBodygroup, 0 );	// turn off blur bodygroup
		}
		else
		{
			SetBodygroup( m_iBlurBodygroup, 1 );	// turn on blur bodygroup
		}
	}

	StudioFrameAdvance();
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CObjectTeleporter::TeleporterThink( void )
{
	SetContextThink( &CObjectTeleporter::TeleporterThink, gpGlobals->curtime + BUILD_TELEPORTER_NEXT_THINK, TELEPORTER_THINK_CONTEXT );

	// At any point, if our match is not ready, revert to IDLE
	if ( IsDisabled() || IsMatchingTeleporterReady() == false )
	{
		if ( GetState() != TELEPORTER_STATE_IDLE && !IsUpgrading() )
		{
			SetState( TELEPORTER_STATE_IDLE );
			ShowDirectionArrow( false );
			m_iUpgradeLevel = 1;
			// We need to adjust for any damage received if we downgraded
			float iHealthPercentage = GetHealth() / GetMaxHealthForCurrentLevel();
			SetMaxHealth( GetMaxHealthForCurrentLevel() );
			SetHealth( (int)floorf( GetMaxHealthForCurrentLevel() * iHealthPercentage ) );
			m_iUpgradeMetal = 0;
		}
		return;
	}

	if ( m_flMyNextThink && m_flMyNextThink > gpGlobals->curtime )
		return;

	// pMatch is not NULL and is not building
	CObjectTeleporter *pMatch = GetMatchingTeleporter();

	Assert( pMatch );
	Assert( pMatch->m_iState != TELEPORTER_STATE_BUILDING );

	switch ( m_iState )
	{
	// Teleporter is not yet active, do nothing
	case TELEPORTER_STATE_BUILDING:
	case TELEPORTER_STATE_UPGRADING:
		ShowDirectionArrow( false );
		break;

	default:
	case TELEPORTER_STATE_IDLE:
		// Do we have a match that is active?
		if ( IsMatchingTeleporterReady() && !IsUpgrading() )
		{
			SetState( TELEPORTER_STATE_READY );
			EmitSound( "Building_Teleporter.Ready" );

			if ( GetObjectMode() == TELEPORTER_TYPE_ENTRANCE )
			{
				ShowDirectionArrow( true );
			}
		}
		break;

	case TELEPORTER_STATE_READY:
		break;

	case TELEPORTER_STATE_SENDING:
		{
			pMatch->TeleporterReceive( m_hTeleportingPlayer, 1.0 );

			m_flRechargeTime = gpGlobals->curtime + ( BUILD_TELEPORTER_FADEOUT_TIME + BUILD_TELEPORTER_FADEIN_TIME + g_iTeleporterRechargeTimes[ GetUpgradeLevel() - 1] );
		
			// change state to recharging...
			SetState( TELEPORTER_STATE_RECHARGING );
		}
		break;

	case TELEPORTER_STATE_RECEIVING:
		{
			// get the position we'll move the player to
			Vector newPosition = GetAbsOrigin();
			newPosition.z += TELEPORTER_MAXS.z + 1;

			// Telefrag anyone in the way
			CBaseEntity *pEnts[256];
			Vector mins, maxs;
			Vector expand( 4, 4, 4 );

			mins = newPosition + VEC_HULL_MIN - expand;
			maxs = newPosition + VEC_HULL_MAX + expand;

			CTFPlayer *pTeleportingPlayer = m_hTeleportingPlayer.Get();

			// move the player
			if ( pTeleportingPlayer )
			{
				CUtlVector<CBaseEntity*> hPlayersToKill;
				bool bClear = true;

				// Telefrag any players in the way
				int numEnts = UTIL_EntitiesInBox( pEnts, 256, mins,	maxs, 0 );
				if ( numEnts )
				{
					//Iterate through the list and check the results
					for ( int i = 0; i < numEnts && bClear; i++ )
					{
						if ( pEnts[i] == NULL )
							continue;

						if ( pEnts[i] == this )
							continue;

						// kill players
						if ( pEnts[i]->IsPlayer() )
						{
							if ( !pTeleportingPlayer->InSameTeam(pEnts[i]) )
							{
								hPlayersToKill.AddToTail( pEnts[i] );
							}
							continue;
						}

						if ( pEnts[i]->IsBaseObject() )
							continue;

						// Solid entities will prevent a teleport
						if ( pEnts[i]->IsSolid() && pEnts[i]->ShouldCollide( pTeleportingPlayer->GetCollisionGroup(), MASK_ALL ) &&
							 g_pGameRules->ShouldCollide( pTeleportingPlayer->GetCollisionGroup(), pEnts[i]->GetCollisionGroup() ) )
						{
							// We're going to teleport into something solid. Abort & destroy this exit.
							bClear = false;
						}
					}
				}

				if ( bClear )
				{
					// Telefrag all enemy players we've found
					for ( int player = 0; player < hPlayersToKill.Count(); player++ )
					{
						hPlayersToKill[player]->TakeDamage( CTakeDamageInfo( this, pTeleportingPlayer, 1000, DMG_CRUSH ) );
					}

					pTeleportingPlayer->Teleport( &newPosition, &(GetAbsAngles()), &vec3_origin );

					// Unzoom if we are a sniper zoomed!
					if ( ( pTeleportingPlayer->GetPlayerClass()->GetClassIndex() == TF_CLASS_SNIPER ) &&
						pTeleportingPlayer->m_Shared.InCond( TF_COND_AIMING ) )
					{
						CTFWeaponBase *pWpn = pTeleportingPlayer->GetActiveTFWeapon();

						if ( pWpn && pWpn->GetWeaponID() == TF_WEAPON_SNIPERRIFLE )
						{
							CTFSniperRifle *pRifle = static_cast<CTFSniperRifle*>( pWpn );
							pRifle->ToggleZoom();
						}
					}

					pTeleportingPlayer->SetFOV( pTeleportingPlayer, 0, tf_teleporter_fov_time.GetFloat(), tf_teleporter_fov_start.GetInt() );

					color32 fadeColor = {255,255,255,100};
					UTIL_ScreenFade( pTeleportingPlayer, fadeColor, 0.25, 0.4, FFADE_IN );
				}
				else
				{
					DetonateObject();
				}
			}			

			SetState( TELEPORTER_STATE_RECEIVING_RELEASE );

			m_flMyNextThink = gpGlobals->curtime + ( BUILD_TELEPORTER_FADEIN_TIME );
		}
		break;

	case TELEPORTER_STATE_RECEIVING_RELEASE:
		{
			CTFPlayer *pTeleportingPlayer = m_hTeleportingPlayer.Get();

			if ( pTeleportingPlayer )
			{
				int iPlayerTeam = pTeleportingPlayer->GetTeamNumber();
				if ( pTeleportingPlayer->IsPlayerClass( TF_CLASS_SPY ) && pTeleportingPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
				{
					iPlayerTeam = pTeleportingPlayer->m_Shared.GetDisguiseTeam();
				}

				int iTeam = GetBuilder() ? GetBuilder()->GetTeamNumber() : GetTeamNumber();

				// Don't show teleporter trail for players with different color.
				if ( iPlayerTeam == iTeam )
				{
					pTeleportingPlayer->TeleportEffect();
				}

				pTeleportingPlayer->m_Shared.RemoveCond( TF_COND_SELECTED_TO_TELEPORT );
				CTF_GameStats.Event_PlayerUsedTeleport( GetBuilder(), pTeleportingPlayer );

				IGameEvent * event = gameeventmanager->CreateEvent( "player_teleported" );
				if ( event )
				{
					event->SetInt( "userid", pTeleportingPlayer->GetUserID() );

					if ( GetBuilder() )
						event->SetInt( "builderid", GetBuilder()->GetUserID() );

					Vector vecOrigin = GetAbsOrigin();
					Vector vecDestinationOrigin = GetMatchingTeleporter()->GetAbsOrigin();
					Vector vecDifference = Vector( vecOrigin.x - vecDestinationOrigin.x, vecOrigin.y - vecDestinationOrigin.y, vecOrigin.z - vecDestinationOrigin.z );
					
					float flDist = sqrtf( pow( vecDifference.x, 2 ) + pow( vecDifference.y, 2 ) + pow( vecDifference.z, 2 ) );

					event->SetFloat( "dist", flDist );	

					gameeventmanager->FireEvent( event, true );
				}

				pTeleportingPlayer->SpeakConceptIfAllowed( MP_CONCEPT_TELEPORTED );
			}

			// reset the pointers to the player now that we're done teleporting
			SetTeleportingPlayer( NULL );
			pMatch->SetTeleportingPlayer( NULL );

			SetState( TELEPORTER_STATE_RECHARGING );

			m_flMyNextThink = gpGlobals->curtime + ( g_iTeleporterRechargeTimes[ GetUpgradeLevel() - 1 ] );
		}
		break;

	case TELEPORTER_STATE_RECHARGING:
		// If we are finished recharging, go active
		if ( gpGlobals->curtime > m_flRechargeTime )
		{
			SetState( TELEPORTER_STATE_READY );
			EmitSound( "Building_Teleporter.Ready" );
		}
		break;
	}
}

int CObjectTeleporter::GetBaseHealth( void )
{
	return 150;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
bool CObjectTeleporter::IsUpgrading( void ) const
{
	return ( m_iState == TELEPORTER_STATE_UPGRADING );
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
char *CObjectTeleporter::GetPlacementModel( void )
{
	if ( GetObjectMode() == TELEPORTER_TYPE_ENTRANCE )
		return TELEPORTER_MODEL_ENTRANCE_PLACEMENT;

	return TELEPORTER_MODEL_EXIT_PLACEMENT;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
int CObjectTeleporter::GetMaxUpgradeLevel(void)
{
	return 3;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CObjectTeleporter::FinishedBuilding( void )
{
	BaseClass::FinishedBuilding();

	SetActivity( ACT_OBJ_RUNNING );
	SetPlaybackRate( 0.0f );
}

void CObjectTeleporter::SetState( int state )
{
	if ( state != m_iState )
	{
		m_iState = state;
		m_flLastStateChangeTime = gpGlobals->curtime;
	}
}

void CObjectTeleporter::ShowDirectionArrow( bool bShow )
{
	if ( bShow != m_bShowDirectionArrow )
	{
		if ( m_iDirectionBodygroup >= 0 )
		{
			SetBodygroup( m_iDirectionBodygroup, bShow ? 1 : 0 );
		}
			
		m_bShowDirectionArrow = bShow;

		if ( bShow )
		{
			CObjectTeleporter *pMatch = GetMatchingTeleporter();

			Assert( pMatch );

			Vector vecToOwner = pMatch->GetAbsOrigin() - GetAbsOrigin();
			QAngle angleToExit;
			VectorAngles( vecToOwner, Vector(0,0,1), angleToExit );
			angleToExit -= GetAbsAngles();

			// pose param is flipped and backwards, adjust.
			m_flYawToExit = anglemod( -angleToExit.y + 180 );
		}
	}
}

int CObjectTeleporter::DrawDebugTextOverlays(void) 
{
	int text_offset = BaseClass::DrawDebugTextOverlays();

	if (m_debugOverlays & OVERLAY_TEXT_BIT) 
	{
		CObjectTeleporter *pMatch = GetMatchingTeleporter();

		char tempstr[512];

		// match
		Q_snprintf( tempstr, sizeof( tempstr ), "Match Found: %s", ( pMatch != NULL ) ? "Yes" : "No" );
		EntityText(text_offset,tempstr,0);
		text_offset++;

		// state
		Q_snprintf( tempstr, sizeof( tempstr ), "State: %d", m_iState );
		EntityText(text_offset,tempstr,0);
		text_offset++;

		// recharge time
		if ( gpGlobals->curtime < m_flRechargeTime )
		{
			float flPercent = ( m_flRechargeTime - gpGlobals->curtime ) / g_iTeleporterRechargeTimes[ GetUpgradeLevel() - 1 ];

			Q_snprintf( tempstr, sizeof( tempstr ), "Recharging: %.1f", flPercent );
			EntityText(text_offset,tempstr,0);
			text_offset++;
		}
	}
	return text_offset;
}

bool CObjectTeleporter::Command_Repair( CTFPlayer *pActivator )
{
	bool bRepaired = false;
	int iAmountToHeal = 0;
	int iRepairCost = 0;

	if ( GetHealth() < GetMaxHealth() )
	{
		iAmountToHeal = min( 100, GetMaxHealth() - GetHealth() );

		// repair the building
		iRepairCost = ceil( (float)( iAmountToHeal ) * 0.2f );

		TRACE_OBJECT( UTIL_VarArgs( "%0.2f CObjectDispenser::Command_Repair ( %d / %d ) - cost = %d\n", gpGlobals->curtime, 
			GetHealth(),
			GetMaxHealth(),
			iRepairCost ) );

		if ( iRepairCost > 0 )
		{
			if ( iRepairCost > pActivator->GetBuildResources() )
			{
				iRepairCost = pActivator->GetBuildResources();
			}

			pActivator->RemoveBuildResources( iRepairCost );

			float flNewHealth = min( GetMaxHealth(), GetHealth() + ( iRepairCost * 5 ) );
			SetHealth( flNewHealth );

			bRepaired = (iRepairCost > 0);

			CObjectTeleporter *pMatchingTeleporter = GetMatchingTeleporter();

			if ( pMatchingTeleporter )
			{
				float flNewHealth = min( pMatchingTeleporter->GetMaxHealth(), pMatchingTeleporter->GetHealth() + (iRepairCost * 5) );
				pMatchingTeleporter->SetHealth( flNewHealth );
			}
		}
	}
	else if ( GetMatchingTeleporter() ) // See if the other teleporter needs repairing
	{
		CObjectTeleporter *pMatchingTeleporter = GetMatchingTeleporter();
		if ( pMatchingTeleporter->GetHealth() < pMatchingTeleporter->GetMaxHealth() )
		{
			iAmountToHeal = min( 100, pMatchingTeleporter->GetMaxHealth() - pMatchingTeleporter->GetHealth() );

			// repair the building
			iRepairCost = ceil( (float)(iAmountToHeal)* 0.2f );

			TRACE_OBJECT( UTIL_VarArgs( "%0.2f CObjectDispenser::Command_Repair ( %d / %d ) - cost = %d\n", gpGlobals->curtime, 
				pMatchingTeleporter->GetHealth(),
				pMatchingTeleporter->GetMaxHealth(),
				iRepairCost ) );

			if ( iRepairCost > 0 )
			{
				if ( iRepairCost > pActivator->GetBuildResources() )
				{
					iRepairCost = pActivator->GetBuildResources();
				}

				pActivator->RemoveBuildResources( iRepairCost );

				float flNewHealth = min( pMatchingTeleporter->GetMaxHealth(), pMatchingTeleporter->GetHealth() + (iRepairCost * 5) );
				pMatchingTeleporter->SetHealth( flNewHealth );

				bRepaired = (iRepairCost > 0);
			}
		}
	}

	return bRepaired;
}


CObjectTeleporter* CObjectTeleporter::FindMatch( void )
{
	int iObjMode = GetObjectMode();
	int iOppositeMode = ( iObjMode == TELEPORTER_TYPE_ENTRANCE ) ? TELEPORTER_TYPE_EXIT : TELEPORTER_TYPE_ENTRANCE;

	CObjectTeleporter *pMatch = NULL;

	CTFPlayer *pBuilder = GetBuilder();

	Assert( pBuilder );

	if ( !pBuilder )
	{
		return NULL;
	}

	int i;
	int iNumObjects = pBuilder->GetObjectCount();
	for ( i=0;i<iNumObjects;i++ )
	{
		CBaseObject *pObj = pBuilder->GetObject(i);

		if ( pObj && pObj->GetType() == GetType() && pObj->GetObjectMode() == iOppositeMode && !pObj->IsDisabled() )
		{
			pMatch = ( CObjectTeleporter * )pObj;
			CopyUpgradeStateToMatch( pMatch, false );
			break;
		}
	}

	return pMatch;
}
