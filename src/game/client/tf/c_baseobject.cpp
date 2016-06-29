//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Clients CBaseObject
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_baseobject.h"
#include "c_tf_player.h"
#include "hud.h"
#include "c_tf_team.h"
#include "engine/IEngineSound.h"
#include "particles_simple.h"
#include "functionproxy.h"
#include "IEffects.h"
#include "model_types.h"
#include "particlemgr.h"
#include "particle_collision.h"
#include "c_tf_weapon_builder.h"
#include "ivrenderview.h"
#include "ObjectControlPanel.h"
#include "engine/ivmodelinfo.h"
#include "c_te_effect_dispatch.h"
#include "toolframework_client.h"
#include "tf_hud_building_status.h"
#include "cl_animevent.h"
#include "eventlist.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// forward declarations
void ToolFramework_RecordMaterialParams( IMaterial *pMaterial );

#define MAX_VISIBLE_BUILDPOINT_DISTANCE		(400 * 400)

// Remove aliasing of name due to shared code
#undef CBaseObject

IMPLEMENT_CLIENTCLASS_DT( C_BaseObject, DT_BaseObject, CBaseObject )
	RecvPropInt( RECVINFO( m_iHealth ) ),
	RecvPropInt( RECVINFO( m_iMaxHealth ) ),
	RecvPropBool( RECVINFO( m_bHasSapper ) ),
	RecvPropInt( RECVINFO( m_iObjectType ) ),
	RecvPropBool( RECVINFO( m_bBuilding ) ),
	RecvPropBool( RECVINFO( m_bPlacing ) ),
	RecvPropBool( RECVINFO( m_bCarried ) ),
	RecvPropBool( RECVINFO( m_bCarryDeploy ) ),
	RecvPropBool( RECVINFO( m_bMiniBuilding ) ),
	RecvPropFloat( RECVINFO( m_flPercentageConstructed ) ),
	RecvPropInt( RECVINFO( m_fObjectFlags ) ),
	RecvPropEHandle( RECVINFO( m_hBuiltOnEntity ) ),
	RecvPropBool( RECVINFO( m_bDisabled ) ),
	RecvPropEHandle( RECVINFO( m_hBuilder ) ),
	RecvPropVector( RECVINFO( m_vecBuildMaxs ) ),
	RecvPropVector( RECVINFO( m_vecBuildMins ) ),
	RecvPropInt( RECVINFO( m_iDesiredBuildRotations ) ),
	RecvPropBool( RECVINFO( m_bServerOverridePlacement ) ),
	RecvPropInt( RECVINFO( m_iUpgradeLevel ) ),
	RecvPropInt( RECVINFO( m_iUpgradeMetal ) ),
	RecvPropInt( RECVINFO( m_iUpgradeMetalRequired ) ),
	RecvPropInt( RECVINFO( m_iHighestUpgradeLevel ) ),
	RecvPropInt( RECVINFO( m_iObjectMode ) ),
	RecvPropBool( RECVINFO( m_bDisposableBuilding ) ),
	RecvPropBool( RECVINFO( m_bWasMapPlaced ) ),
END_RECV_TABLE()

ConVar cl_obj_test_building_damage( "cl_obj_test_building_damage", "-1", FCVAR_CHEAT, "debug building damage", true, -1, true, BUILDING_DAMAGE_LEVEL_CRITICAL );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_BaseObject::C_BaseObject(  )
{
	m_YawPreviewState = YAW_PREVIEW_OFF;
	m_bBuilding = false;
	m_bPlacing = false;
	m_bCarried = false;
	m_bCarryDeploy = false;
	m_flPercentageConstructed = 0;
	m_fObjectFlags = 0;

	m_flCurrentBuildRotation = 0;

	m_damageLevel = BUILDING_DAMAGE_LEVEL_NONE;

	m_iLastPlacementPosValid = -1;
	m_iOldUpgradeLevel = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_BaseObject::~C_BaseObject( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::Spawn( void )
{
	BaseClass::Spawn();

	m_bServerOverridePlacement = true;	// assume valid at the start
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::UpdateOnRemove( void )
{
	StopAnimGeneratedSounds();

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::PreDataUpdate( DataUpdateType_t updateType )
{
	BaseClass::PreDataUpdate( updateType );

	m_iOldHealth = m_iHealth;
	m_hOldOwner = GetOwner();
	m_bWasActive = ShouldBeActive();
	m_bWasBuilding = m_bBuilding;
	m_bOldDisabled = m_bDisabled;
	m_bWasPlacing = m_bPlacing;
	m_bWasCarried = m_bCarried;

	m_nObjectOldSequence = GetSequence();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::OnDataChanged( DataUpdateType_t updateType )
{
	if (updateType == DATA_UPDATE_CREATED)
	{
		CreateBuildPoints();
	}

	BaseClass::OnDataChanged( updateType );

	// Did we just finish building?
	if ( m_bWasBuilding && !m_bBuilding )
	{
		FinishedBuilding();
	}

	// Did we just go active?
	bool bShouldBeActive = ShouldBeActive();
	if ( !m_bWasActive && bShouldBeActive )
	{
		OnGoActive();
	}
	else if ( m_bWasActive && !bShouldBeActive )
	{
		OnGoInactive();
	}

	if ( m_bDisabled != m_bOldDisabled )
	{
		if ( m_bDisabled )
		{
			OnStartDisabled();
		}
		else
		{
			OnEndDisabled();
		}
	}

	if ( ( !IsBuilding() && m_iHealth != m_iOldHealth ) )
	{
		// recalc our damage particle state
		BuildingDamageLevel_t damageLevel = CalculateDamageLevel();

		if ( damageLevel != m_damageLevel )
		{
			UpdateDamageEffects( damageLevel );

			m_damageLevel = damageLevel;
		}
	}

	if ( m_bWasBuilding && !m_bBuilding )
	{
		// Force update damage effect when finishing construction.
		BuildingDamageLevel_t damageLevel = CalculateDamageLevel();
		UpdateDamageEffects( damageLevel );
		m_damageLevel = damageLevel;
	}

	// Kill all particles when getting picked up.
	if ( !m_bWasCarried && m_bCarried )
	{
		ParticleProp()->StopParticlesInvolving( this );
	}

	if ( m_iHealth > m_iOldHealth && m_iHealth == m_iMaxHealth )
	{
		// If we were just fully healed, remove all decals
		RemoveAllDecals();
	}

	if ( GetOwner() == C_TFPlayer::GetLocalTFPlayer() )
	{
		IGameEvent *event = gameeventmanager->CreateEvent( "building_info_changed" );
		if ( event )
		{
			event->SetInt( "building_type", GetType() );
			event->SetInt( "object_mode", GetObjectMode() );
			gameeventmanager->FireEventClientSide( event );
		}
	}

	if ( IsPlacing() && GetSequence() != m_nObjectOldSequence )
	{
		// Ignore server sequences while placing
		OnPlacementStateChanged( m_iLastPlacementPosValid > 0 );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::SetDormant( bool bDormant )
{
	if ( !IsDormant() && bDormant )
	{
		// Going dormant so kill damage effects.
		UpdateDamageEffects( BUILDING_DAMAGE_LEVEL_NONE );
	}
	BaseClass::SetDormant( bDormant );
	//ENTITY_PANEL_ACTIVATE( "analyzed_object", !bDormant );
}

#define TF_OBJ_BODYGROUPTURNON			1
#define TF_OBJ_BODYGROUPTURNOFF			0

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : origin - 
//			angles - 
//			event - 
//			*options - 
//-----------------------------------------------------------------------------
void C_BaseObject::FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options )
{
	switch ( event )
	{
	default:
		{
			BaseClass::FireEvent( origin, angles, event, options );
		}
		break;
	case TF_OBJ_PLAYBUILDSOUND:
		{
			EmitSound( options );
		}
		break;
	case TF_OBJ_ENABLEBODYGROUP:
		{
			int index = FindBodygroupByName( options );
			if ( index >= 0 )
			{
				SetBodygroup( index, TF_OBJ_BODYGROUPTURNON );
			}
		}
		break;
	case TF_OBJ_DISABLEBODYGROUP:
		{
			int index = FindBodygroupByName( options );
			if ( index >= 0 )
			{
				SetBodygroup( index, TF_OBJ_BODYGROUPTURNOFF );
			}
		}
		break;
	case TF_OBJ_ENABLEALLBODYGROUPS:
	case TF_OBJ_DISABLEALLBODYGROUPS:
		{
			// Start at 1, because body 0 is the main .mdl body...
			// Is this the way we want to do this?
			int count = GetNumBodyGroups();
			for ( int i = 1; i < count; i++ )
			{
				int subpartcount = GetBodygroupCount( i );
				if ( subpartcount == 2 )
				{
					SetBodygroup( i, 
						( event == TF_OBJ_ENABLEALLBODYGROUPS ) ?
						TF_OBJ_BODYGROUPTURNON : TF_OBJ_BODYGROUPTURNOFF );
				}
				else
				{
					DevMsg( "TF_OBJ_ENABLE/DISABLEBODY GROUP:  %s has a group with %i subparts, should be exactly 2\n",
						GetClassname(), subpartcount );
				}
			}
		}
		break;
	}
}


const char* C_BaseObject::GetStatusName() const
{
	return GetObjectInfo( GetType() )->m_pStatusName;
}

void C_BaseObject::GetStatusText( wchar_t *pStatus, int iMaxStatusLen )
{
	wchar_t wszName[128];
	g_pVGuiLocalize->ConvertANSIToUnicode( GetStatusName(),  wszName, sizeof(wszName) );

	g_pVGuiLocalize->ConstructString( pStatus, iMaxStatusLen, L"%s1", 1, wszName );
}

//-----------------------------------------------------------------------------
// Purpose: placement state has changed, update the model
//-----------------------------------------------------------------------------
void C_BaseObject::OnPlacementStateChanged( bool bValidPlacement )
{
	if ( bValidPlacement )
	{
		SetActivity( ACT_OBJ_PLACING );
	}
	else
	{
		SetActivity( ACT_OBJ_IDLE );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::Simulate( void )
{
	if ( IsPlacing() && !MustBeBuiltOnAttachmentPoint() )
	{
		int iValidPlacement = ( IsPlacementPosValid() && ServerValidPlacement() ) ? 1 : 0;

		if ( m_iLastPlacementPosValid != iValidPlacement )
		{
			m_iLastPlacementPosValid = iValidPlacement;
			OnPlacementStateChanged( m_iLastPlacementPosValid > 0 );
		}

		// We figure out our own placement pos, but we still leave it to the server to 
		// do collision with other entities and nobuild triggers, so that sets the 
		// placement animation

		SetLocalOrigin( m_vecBuildOrigin );
		InvalidateBoneCache();

		// Clear out our origin and rotation interpolation history
		// so we don't pop when we teleport in the actual position from the server

		CInterpolatedVar< Vector > &interpolator = GetOriginInterpolator();
		interpolator.ClearHistory();

		CInterpolatedVar<QAngle> &rotInterpolator = GetRotationInterpolator();
		rotInterpolator.ClearHistory();
	}	

	BaseClass::Simulate();
}

//-----------------------------------------------------------------------------
// Purpose: Return false if the server is telling us we can't place right now
// could be due to placing in a nobuild or respawn room
//-----------------------------------------------------------------------------
bool C_BaseObject::ServerValidPlacement( void )
{
	return m_bServerOverridePlacement;
}

bool C_BaseObject::WasLastPlacementPosValid( void )
{
	if ( MustBeBuiltOnAttachmentPoint() )
	{
		return ( !IsEffectActive(EF_NODRAW) );
	}
	
	return ( m_iLastPlacementPosValid > 0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int C_BaseObject::DrawModel( int flags )
{
	int drawn;

	// If we're a brush-built, map-defined object chain up to baseentity draw
	if ( modelinfo->GetModelType( GetModel() ) == mod_brush )
	{
		drawn = CBaseEntity::DrawModel(flags);
	}
	else
	{
		drawn = BaseClass::DrawModel(flags);
	}

	HighlightBuildPoints( flags );

	return drawn;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::HighlightBuildPoints( int flags )
{
	C_TFPlayer *pLocal = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocal )
		return;

	if ( !GetNumBuildPoints() || !InLocalTeam() )
		return;

	C_TFWeaponBuilder *pBuilderWpn = dynamic_cast< C_TFWeaponBuilder * >( pLocal->GetActiveWeaponForSelection() );
	if ( !pBuilderWpn )
		return;
	if ( !pBuilderWpn->IsPlacingObject() )
		return;
	C_BaseObject *pPlacementObj = pBuilderWpn->GetPlacementModel();
	if ( !pPlacementObj || pPlacementObj == this )
		return;

	// Near enough?
	if ( (GetAbsOrigin() - pLocal->GetAbsOrigin()).LengthSqr() < MAX_VISIBLE_BUILDPOINT_DISTANCE )
	{
		bool bRestoreModel = false;
		Vector vecPrevAbsOrigin = pPlacementObj->GetAbsOrigin();
		QAngle vecPrevAbsAngles = pPlacementObj->GetAbsAngles();

		Vector orgColor;
		render->GetColorModulation( orgColor.Base() );
		float orgBlend = render->GetBlend();

		bool bSameTeam = ( pPlacementObj->GetTeamNumber() == GetTeamNumber() );

		if ( pPlacementObj->IsHostileUpgrade() && bSameTeam )
		{
			// Don't hilight hostile upgrades on friendly objects
			return;
		}
		else if ( !bSameTeam )
		{
			// Don't hilight upgrades on enemy objects
			return;
		}

		// Any empty buildpoints?
		for ( int i = 0; i < GetNumBuildPoints(); i++ )
		{
			// Can this object build on this point?
			if ( CanBuildObjectOnBuildPoint( i, pPlacementObj->GetType() ) )
			{
				Vector vecBPOrigin;
				QAngle vecBPAngles;
				if ( GetBuildPoint(i, vecBPOrigin, vecBPAngles) )
				{
					pPlacementObj->InvalidateBoneCaches();

					Vector color( 0, 255, 0 );
					render->SetColorModulation(	color.Base() );
					float frac = fmod( gpGlobals->curtime, 3 );
					frac *= 2 * M_PI;
					frac = cos( frac );
					render->SetBlend( (175 + (int)( frac * 75.0f )) / 255.0 );

					// FIXME: This truly sucks! The bone cache should use
					// render location for this computation instead of directly accessing AbsAngles
					// Necessary for bone cache computations to work
					pPlacementObj->SetAbsOrigin( vecBPOrigin );
					pPlacementObj->SetAbsAngles( vecBPAngles );

					modelrender->DrawModel( 
						flags, 
						pPlacementObj,
						pPlacementObj->GetModelInstance(),
						pPlacementObj->index, 
						pPlacementObj->GetModel(),
						vecBPOrigin,
						vecBPAngles,
						pPlacementObj->m_nSkin,
						pPlacementObj->m_nBody,
						pPlacementObj->m_nHitboxSet
						);

					bRestoreModel = true;
				}
			}
		}

		if ( bRestoreModel )
		{
			pPlacementObj->SetAbsOrigin(vecPrevAbsOrigin);
			pPlacementObj->SetAbsAngles(vecPrevAbsAngles);
			pPlacementObj->InvalidateBoneCaches();

			render->SetColorModulation( orgColor.Base() );
			render->SetBlend( orgBlend );
		}
	}
}

//-----------------------------------------------------------------------------
// Builder preview...
//-----------------------------------------------------------------------------
void C_BaseObject::ActivateYawPreview( bool enable )
{
	m_YawPreviewState = enable ? YAW_PREVIEW_ON : YAW_PREVIEW_WAITING_FOR_UPDATE;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::PreviewYaw( float yaw )
{
	m_fYawPreview = yaw;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C_BaseObject::IsPreviewingYaw() const
{
	return m_YawPreviewState != YAW_PREVIEW_OFF;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
BuildingDamageLevel_t C_BaseObject::CalculateDamageLevel( void )
{
	float flPercentHealth = (float)m_iHealth / (float)m_iMaxHealth;

	BuildingDamageLevel_t damageLevel = BUILDING_DAMAGE_LEVEL_NONE;

	if ( flPercentHealth < 0.25 )
	{
		damageLevel = BUILDING_DAMAGE_LEVEL_CRITICAL;
	}
	else if ( flPercentHealth < 0.45 )
	{
		damageLevel = BUILDING_DAMAGE_LEVEL_HEAVY;
	}
	else if ( flPercentHealth < 0.65 )
	{
		damageLevel = BUILDING_DAMAGE_LEVEL_MEDIUM;
	}
	else if ( flPercentHealth < 0.85 )
	{
		damageLevel = BUILDING_DAMAGE_LEVEL_LIGHT;
	}

	if ( cl_obj_test_building_damage.GetInt() >= 0 )
	{
		damageLevel = (BuildingDamageLevel_t)cl_obj_test_building_damage.GetInt();
	}

	return damageLevel;
}

/*
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::Release( void )
{
	// Remove any reticles on this entity
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( pPlayer )
	{
		pPlayer->Remove_Target( this );
	}

	BaseClass::Release();
}
*/

//-----------------------------------------------------------------------------
// Ownership: 
//-----------------------------------------------------------------------------
C_TFPlayer *C_BaseObject::GetOwner()
{
	return m_hBuilder;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool C_BaseObject::IsOwnedByLocalPlayer() const
{
	if ( !m_hBuilder )
		return false;

	return ( m_hBuilder == C_TFPlayer::GetLocalTFPlayer() );
}

//-----------------------------------------------------------------------------
// Purpose: Add entity to visibile entities list
//-----------------------------------------------------------------------------
void C_BaseObject::AddEntity( void )
{
	// If set to invisible, skip. Do this before resetting the entity pointer so it has 
	// valid data to decide whether it's visible.
	if ( !ShouldDraw() )
	{
		return;
	}

	// Update the entity position
	//UpdatePosition();

	// Yaw preview
	if (m_YawPreviewState != YAW_PREVIEW_OFF)
	{
		// This piece of code makes it so we keep using the preview
		// until we get a network update which matches the update value
		if (m_YawPreviewState == YAW_PREVIEW_WAITING_FOR_UPDATE)
		{
			if (fmod( fabs(GetLocalAngles().y - m_fYawPreview), 360.0f) < 1.0f)
			{
				m_YawPreviewState = YAW_PREVIEW_OFF;
			}
		}

		if (GetLocalOrigin().y != m_fYawPreview)
		{
			SetLocalAnglesDim( Y_INDEX, m_fYawPreview );
			InvalidateBoneCache();
		}
	}

	// Create flashlight effects, etc.
	CreateLightEffects();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::Select( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	pPlayer->SetSelectedObject( this );
}

//-----------------------------------------------------------------------------
// Sends client commands back to the server: 
//-----------------------------------------------------------------------------
void C_BaseObject::SendClientCommand( const char *pCmd )
{
	char szbuf[128];
	Q_snprintf( szbuf, sizeof( szbuf ), "objcmd %d %s", entindex(), pCmd );
  	engine->ClientCmd(szbuf);
}


//-----------------------------------------------------------------------------
// Purpose: Get a text description for the object target
//-----------------------------------------------------------------------------
const char *C_BaseObject::GetTargetDescription( void ) const
{
	return GetStatusName();
}

//-----------------------------------------------------------------------------
// Purpose: Get a text description for the object target (more verbose)
//-----------------------------------------------------------------------------
const char *C_BaseObject::GetIDString(void)
{
	m_szIDString[0] = 0;
	RecalculateIDString();
	return m_szIDString;
}


//-----------------------------------------------------------------------------
// It's a valid ID target when it's building 
//-----------------------------------------------------------------------------
bool C_BaseObject::IsValidIDTarget( void )
{
	return InSameTeam( C_TFPlayer::GetLocalTFPlayer() ) && m_bBuilding;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::RecalculateIDString( void )
{
	// Subclasses may have filled this out with a string
	if ( !m_szIDString[0] )
	{
		Q_strncpy( m_szIDString, GetTargetDescription(), sizeof(m_szIDString) );
	}

	// Have I taken damage?
	if ( m_iHealth < m_iMaxHealth )
	{
		char szHealth[ MAX_ID_STRING ];
		if ( m_bBuilding )
		{
			Q_snprintf( szHealth, sizeof(szHealth), "\nConstruction at %.0f percent\nHealth at %.0f percent", (m_flPercentageConstructed * 100), ceil(((float)m_iHealth / (float)m_iMaxHealth) * 100) );
		}
		else
		{
			Q_snprintf( szHealth, sizeof(szHealth), "\nHealth at %.0f percent", ceil(((float)m_iHealth / (float)m_iMaxHealth) * 100) );
		}
		Q_strncat( m_szIDString, szHealth, sizeof(m_szIDString), COPY_ALL_CHARACTERS );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Player has waved his crosshair over this entity. Display appropriate hints.
//-----------------------------------------------------------------------------
void C_BaseObject::DisplayHintTo( C_BasePlayer *pPlayer )
{
	bool bHintPlayed = false;

	C_TFPlayer *pTFPlayer = ToTFPlayer(pPlayer);
	if ( InSameTeam( pPlayer ) )
	{
		// We're looking at a friendly object. 

		if ( HasSapper() )
		{
			bHintPlayed = pPlayer->HintMessage( HINT_OBJECT_HAS_SAPPER, true, true );
		}

		if ( pTFPlayer->IsPlayerClass( TF_CLASS_ENGINEER ) )
		{
			// I'm an engineer.

			// If I'm looking at a constructing object, let me know I can help build it (but not 
			// if I built it myself, since I've already got that hint from the wrench).
			if ( !bHintPlayed && IsBuilding() && GetBuilder() != pTFPlayer )
			{
				bHintPlayed = pPlayer->HintMessage( HINT_ENGINEER_USE_WRENCH_ONOTHER, false, true );
			}

			// If it's damaged, I can repair it
			if ( !bHintPlayed && !IsBuilding() && GetHealth() < GetMaxHealth() )
			{
				bHintPlayed = pPlayer->HintMessage( HINT_ENGINEER_REPAIR_OBJECT, false, true );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Does this object have a sapper on it
//-----------------------------------------------------------------------------
bool C_BaseObject::HasSapper( void )
{
	return m_bHasSapper;
}

void C_BaseObject::OnStartDisabled()
{
}

void C_BaseObject::OnEndDisabled()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_BaseObject::GetTargetIDString( wchar_t *sIDString, int iMaxLenInBytes )
{
	sIDString[0] = '\0';

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pLocalPlayer )
		return;

	if ( InSameTeam( pLocalPlayer ) || pLocalPlayer->IsPlayerClass( TF_CLASS_SPY ) || pLocalPlayer->GetTeamNumber() == TEAM_SPECTATOR )
	{
		wchar_t wszBuilderName[ MAX_PLAYER_NAME_LENGTH ];

		const char *pszStatusName = GetStatusName();
		wchar_t *wszObjectName = g_pVGuiLocalize->Find( pszStatusName );

		if ( !wszObjectName )
		{
			wszObjectName = L"";
		}

		C_BasePlayer *pBuilder = GetOwner();

		if ( pBuilder )
		{
			g_pVGuiLocalize->ConvertANSIToUnicode( pBuilder->GetPlayerName(), wszBuilderName, sizeof(wszBuilderName) );
		}
		else
		{
			wszBuilderName[0] = '\0';
		}

		// building or live, show health
		const char *printFormatString;
		
		if ( GetObjectInfo(GetType())->m_AltModes.Count() > 0 )
		{
			printFormatString = "#TF_playerid_object_mode";

			pszStatusName = GetObjectInfo( GetType() )->m_AltModes.Element( m_iObjectMode * 3 + 1 );
			wchar_t *wszObjectModeName = g_pVGuiLocalize->Find( pszStatusName );

			if ( !wszObjectModeName )
			{
				wszObjectModeName = L"";
			}

			g_pVGuiLocalize->ConstructString( sIDString, iMaxLenInBytes, g_pVGuiLocalize->Find(printFormatString),
				4,
				wszObjectName,
				wszBuilderName,
				wszObjectModeName);
		}
		else
		{
			if ( m_bMiniBuilding )
				printFormatString = "#TF_playerid_object_mini";
			else
				printFormatString = "#TF_playerid_object";

			g_pVGuiLocalize->ConstructString( sIDString, iMaxLenInBytes, g_pVGuiLocalize->Find( printFormatString ),
				3,
				wszObjectName,
				wszBuilderName );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_BaseObject::GetTargetIDDataString( wchar_t *sDataString, int iMaxLenInBytes )
{
	sDataString[0] = '\0';

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return;

	wchar_t wszBuilderName[ MAX_PLAYER_NAME_LENGTH ];
	wchar_t wszObjectName[ 32 ];
	wchar_t wszUpgradeProgress[ 32 ];
	wchar_t wszLevel[ 5 ];

	_snwprintf(wszLevel, ARRAYSIZE(wszLevel) - 1, L"%d", m_iUpgradeLevel);

	g_pVGuiLocalize->ConvertANSIToUnicode( GetStatusName(), wszObjectName, sizeof(wszObjectName) );

	C_BasePlayer *pBuilder = GetOwner();

	if ( pBuilder )
	{
		g_pVGuiLocalize->ConvertANSIToUnicode( pBuilder->GetPlayerName(), wszBuilderName, sizeof(wszBuilderName) );
	}
	else
	{
		wszBuilderName[0] = '\0';
	}

	if (m_iUpgradeLevel >= m_iHighestUpgradeLevel)
	{
		const char *printFormatString = "#TF_playerid_object_level";

		g_pVGuiLocalize->ConstructString(sDataString, iMaxLenInBytes, g_pVGuiLocalize->Find(printFormatString),
			1,
			wszLevel);
	}
	else
	{
		// level 1 and 2 show upgrade progress
		_snwprintf(wszUpgradeProgress, ARRAYSIZE(wszUpgradeProgress) - 1, L"%d / %d", m_iUpgradeMetal, m_iUpgradeMetalRequired);
		wszUpgradeProgress[ARRAYSIZE(wszUpgradeProgress) - 1] = '\0';

		const char *printFormatString = "#TF_playerid_object_upgrading_level";		

		g_pVGuiLocalize->ConstructString(sDataString, iMaxLenInBytes, g_pVGuiLocalize->Find(printFormatString),
			2,
			wszLevel,
			wszUpgradeProgress);
	}



}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int C_BaseObject::GetDisplayPriority( void )
{
	return GetObjectInfo( GetType() )->m_iDisplayPriority;	
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *C_BaseObject::GetHudStatusIcon( void )
{
	return GetObjectInfo( GetType() )->m_pHudStatusIcon;	
}

ConVar cl_obj_fake_alert( "cl_obj_fake_alert", "0", 0, "", true, BUILDING_HUD_ALERT_NONE, true, MAX_BUILDING_HUD_ALERT_LEVEL-1 );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
BuildingHudAlert_t C_BaseObject::GetBuildingAlertLevel( void )
{
	float flHealthPercent = GetHealth() / GetMaxHealth();

	BuildingHudAlert_t alertLevel = BUILDING_HUD_ALERT_NONE;

	if ( HasSapper() )
	{
		alertLevel = BUILDING_HUD_ALERT_SAPPER;
	}
	else if ( !IsBuilding() && flHealthPercent < 0.33 )
	{
		alertLevel = BUILDING_HUD_ALERT_VERY_LOW_HEALTH;
	}
	else if ( !IsBuilding() && flHealthPercent < 0.66 )
	{
		alertLevel = BUILDING_HUD_ALERT_LOW_HEALTH;
	}

	BuildingHudAlert_t iFakeAlert = (BuildingHudAlert_t)cl_obj_fake_alert.GetInt();

	if ( iFakeAlert > BUILDING_HUD_ALERT_NONE &&
		iFakeAlert < MAX_BUILDING_HUD_ALERT_LEVEL )
	{
		alertLevel = iFakeAlert;
	}

	return alertLevel;
}

//-----------------------------------------------------------------------------
// Purpose: find the anim events that may have started sounds, and stop them.
//-----------------------------------------------------------------------------
void C_BaseObject::StopAnimGeneratedSounds( void )
{
	MDLCACHE_CRITICAL_SECTION();

	CStudioHdr *pStudioHdr = GetModelPtr();
	if ( !pStudioHdr )
		return;

	mstudioseqdesc_t &seqdesc = pStudioHdr->pSeqdesc( GetSequence() );

	float flCurrentCycle = GetCycle();

	mstudioevent_t *pevent = GetEventIndexForSequence( seqdesc );

	for (int i = 0; i < (int)seqdesc.numevents; i++)
	{
		if ( pevent[i].cycle < flCurrentCycle )
		{
			if ( pevent[i].event == CL_EVENT_SOUND || pevent[i].event == AE_CL_PLAYSOUND )
			{
				StopSound( entindex(), pevent[i].options );
			}
		}
	}
}

//============================================================================================================
// POWER PROXY
//============================================================================================================
class CObjectPowerProxy : public CResultProxy
{
public:
	bool Init( IMaterial *pMaterial, KeyValues *pKeyValues );
	void OnBind( void *pC_BaseEntity );

private:
	CFloatInput	m_Factor;
};

bool CObjectPowerProxy::Init( IMaterial *pMaterial, KeyValues *pKeyValues )
{
	if (!CResultProxy::Init( pMaterial, pKeyValues ))
		return false;

	if (!m_Factor.Init( pMaterial, pKeyValues, "scale", 1 ))
		return false;

	return true;
}

void CObjectPowerProxy::OnBind( void *pRenderable )
{
	// Find the view angle between the player and this entity....
	IClientRenderable *pRend = (IClientRenderable *)pRenderable;
	C_BaseEntity *pEntity = pRend->GetIClientUnknown()->GetBaseEntity();
	C_BaseObject *pObject = dynamic_cast<C_BaseObject*>(pEntity);
	if (!pObject)
		return;

	SetFloatResult(  m_Factor.GetFloat() );

	if ( ToolsEnabled() )
	{
		ToolFramework_RecordMaterialParams( GetMaterial() );
	}
}

EXPOSE_INTERFACE( CObjectPowerProxy, IMaterialProxy, "ObjectPower" IMATERIAL_PROXY_INTERFACE_VERSION );

//-----------------------------------------------------------------------------
// Control screen 
//-----------------------------------------------------------------------------
class CBasicControlPanel : public CObjectControlPanel
{
	DECLARE_CLASS( CBasicControlPanel, CObjectControlPanel );

public:
	CBasicControlPanel( vgui::Panel *parent, const char *panelName );
};


DECLARE_VGUI_SCREEN_FACTORY( CBasicControlPanel, "basic_control_panel" );


//-----------------------------------------------------------------------------
// Constructor: 
//-----------------------------------------------------------------------------
CBasicControlPanel::CBasicControlPanel( vgui::Panel *parent, const char *panelName )
	: BaseClass( parent, "CBasicControlPanel" ) 
{
}
