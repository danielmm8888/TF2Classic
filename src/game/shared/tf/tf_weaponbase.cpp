﻿//========= Copyright © 1996-2004, Valve LLC, All rights reserved. ============
//
//	Weapons.
//
//=============================================================================
#include "cbase.h"
#include "in_buttons.h"
#include "takedamageinfo.h"
#include "tf_weaponbase.h"
#include "ammodef.h"
#include "tf_gamerules.h"
#include "eventlist.h"
#include "tf_viewmodel.h"
#include "econ_itemschema.h"

// Server specific.
#if !defined( CLIENT_DLL )
#include "tf_player.h"
// Client specific.
#else
#include "vgui/ISurface.h"
#include "vgui_controls/Controls.h"
#include "c_tf_player.h"
#include "tf_viewmodel.h"
#include "hud_crosshair.h"
#include "c_tf_playerresource.h"
#include "clientmode_tf.h"
#include "r_efx.h"
#include "dlight.h"
#include "effect_dispatch_data.h"
#include "c_te_effect_dispatch.h"
#include "toolframework_client.h"

// for spy material proxy
#include "proxyentity.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#endif

extern ConVar tf_useparticletracers;

#ifdef CLIENT_DLL
extern ConVar tf2c_model_muzzleflash;
extern ConVar tf2c_muzzlelight;
#endif

ConVar tf_weapon_criticals( "tf_weapon_criticals", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Whether or not random crits are enabled." );

//=============================================================================
//
// Global functions.
//

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool IsAmmoType( int iAmmoType, const char *pAmmoName )
{
	return GetAmmoDef()->Index( pAmmoName ) == iAmmoType;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void FindHullIntersection( const Vector &vecSrc, trace_t &tr, const Vector &mins, const Vector &maxs, CBaseEntity *pEntity )
{
	int	i, j, k;
	trace_t tmpTrace;
	Vector vecEnd;
	float distance = 1e6f;
	Vector minmaxs[2] = {mins, maxs};
	Vector vecHullEnd = tr.endpos;

	vecHullEnd = vecSrc + ((vecHullEnd - vecSrc)*2);
	UTIL_TraceLine( vecSrc, vecHullEnd, MASK_SOLID, pEntity, COLLISION_GROUP_NONE, &tmpTrace );
	if ( tmpTrace.fraction < 1.0 )
	{
		tr = tmpTrace;
		return;
	}

	for ( i = 0; i < 2; i++ )
	{
		for ( j = 0; j < 2; j++ )
		{
			for ( k = 0; k < 2; k++ )
			{
				vecEnd.x = vecHullEnd.x + minmaxs[i][0];
				vecEnd.y = vecHullEnd.y + minmaxs[j][1];
				vecEnd.z = vecHullEnd.z + minmaxs[k][2];

				UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID, pEntity, COLLISION_GROUP_NONE, &tmpTrace );
				if ( tmpTrace.fraction < 1.0 )
				{
					float thisDistance = (tmpTrace.endpos - vecSrc).Length();
					if ( thisDistance < distance )
					{
						tr = tmpTrace;
						distance = thisDistance;
					}
				}
			}
		}
	}
}

//=============================================================================
//
// TFWeaponBase tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFWeaponBase, DT_TFWeaponBase )

BEGIN_NETWORK_TABLE( CTFWeaponBase, DT_TFWeaponBase )
// Client specific.
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bLowered ) ),
	RecvPropInt( RECVINFO( m_iReloadMode ) ),
	RecvPropBool( RECVINFO( m_bResetParity ) ), 
	RecvPropBool( RECVINFO( m_bReloadedThroughAnimEvent ) ),
// Server specific.
#else
	SendPropBool( SENDINFO( m_bLowered ) ),
	SendPropBool( SENDINFO( m_bResetParity ) ),
	SendPropInt( SENDINFO( m_iReloadMode ), 4, SPROP_UNSIGNED ),
	SendPropBool( SENDINFO( m_bReloadedThroughAnimEvent ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFWeaponBase ) 
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_bLowered, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_iReloadMode, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bReloadedThroughAnimEvent, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_base, CTFWeaponBase );

// Server specific.
#if !defined( CLIENT_DLL )

BEGIN_DATADESC( CTFWeaponBase )
//DEFINE_FUNCTION( FallThink )
END_DATADESC()

// Client specific
#else

ConVar cl_crosshaircolor( "cl_crosshaircolor", "0", FCVAR_CLIENTDLL | FCVAR_ARCHIVE );
ConVar cl_dynamiccrosshair( "cl_dynamiccrosshair", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE );
ConVar cl_scalecrosshair( "cl_scalecrosshair", "1", FCVAR_CLIENTDLL | FCVAR_ARCHIVE );
ConVar cl_crosshairalpha( "cl_crosshairalpha", "200", FCVAR_CLIENTDLL | FCVAR_ARCHIVE );

int g_iScopeTextureID = 0;
int g_iScopeDustTextureID = 0;

#endif

//=============================================================================
//
// TFWeaponBase shared functions.
//

// -----------------------------------------------------------------------------
// Purpose: Constructor.
// -----------------------------------------------------------------------------
CTFWeaponBase::CTFWeaponBase()
{
	SetPredictionEligible( true );

	// Nothing collides with these, but they get touch calls.
	AddSolidFlags( FSOLID_TRIGGER );

	// Weapons can fire underwater.
	m_bFiresUnderwater = true;
	m_bAltFiresUnderwater = true;

	// Initialize the weapon modes.
	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;
	m_iReloadMode.Set( TF_RELOAD_START );

	m_iAltFireHint = 0;
	m_bInAttack = false;
	m_bInAttack2 = false;
	m_flCritTime = 0;
	m_flLastCritCheckTime = 0;
	m_iLastCritCheckFrame = 0;
	m_bCurrentAttackIsCrit = false;
	m_iCurrentSeed = -1;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBase::Spawn()
{
	// Base class spawn.
	BaseClass::Spawn();

	// Set this here to allow players to shoot dropped weapons.
	SetCollisionGroup( COLLISION_GROUP_WEAPON );

	// Get the weapon information.
	WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot( GetClassname() );
	Assert( hWpnInfo != GetInvalidWeaponInfoHandle() );
	CTFWeaponInfo *pWeaponInfo = dynamic_cast<CTFWeaponInfo*>( GetFileWeaponInfoFromHandle( hWpnInfo ) );
	Assert( pWeaponInfo && "Failed to get CTFWeaponInfo in weapon spawn" );
	m_pWeaponInfo = pWeaponInfo;

	if ( GetPlayerOwner() )
	{
		ChangeTeam( GetPlayerOwner()->GetTeamNumber() );
	}

#ifdef GAME_DLL
	// Move it up a little bit, otherwise it'll be at the guy's feet, and its sound origin 
	// will be in the ground so its EmitSound calls won't do anything.
	Vector vecOrigin = GetAbsOrigin();
	SetAbsOrigin( Vector( vecOrigin.x, vecOrigin.y, vecOrigin.z + 5.0f ) );
#endif

	m_szTracerName[0] = '\0';
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBase::FallInit( void )
{
	if (TFGameRules() && TFGameRules()->IsDeathmatch())
		SetPickupTouch();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
void CTFWeaponBase::Precache()
{
	BaseClass::Precache();

	if ( GetMuzzleFlashModel() )
	{
		PrecacheModel( GetMuzzleFlashModel() );
	}

	const CTFWeaponInfo *pTFInfo = &GetTFWpnData();

	// Precache the DM viewmodel (if we have one)
	if (pTFInfo->m_szViewModelDM[0] != '\0')
	{
		PrecacheModel(pTFInfo->m_szViewModelDM);
	}

	if ( pTFInfo->m_szExplosionSound && pTFInfo->m_szExplosionSound[0] )
	{
		CBaseEntity::PrecacheScriptSound( pTFInfo->m_szExplosionSound );
	}

	if ( pTFInfo->m_szBrassModel[0] )
	{
		PrecacheModel( pTFInfo->m_szBrassModel );
	}

	if ( pTFInfo->m_szMuzzleFlashParticleEffect && pTFInfo->m_szMuzzleFlashParticleEffect[0] )
	{
		PrecacheParticleSystem( pTFInfo->m_szMuzzleFlashParticleEffect );
	}

	if ( pTFInfo->m_szExplosionEffect && pTFInfo->m_szExplosionEffect[0] )
	{
		PrecacheParticleSystem( pTFInfo->m_szExplosionEffect );
	}

	if ( pTFInfo->m_szExplosionPlayerEffect && pTFInfo->m_szExplosionPlayerEffect[0] )
	{
		PrecacheParticleSystem( pTFInfo->m_szExplosionPlayerEffect );
	}

	if ( pTFInfo->m_szExplosionWaterEffect && pTFInfo->m_szExplosionWaterEffect[0] )
	{
		PrecacheParticleSystem( pTFInfo->m_szExplosionWaterEffect );
	}

	if ( pTFInfo->m_szTracerEffect && pTFInfo->m_szTracerEffect[0] )
	{
		char pTracerEffect[128];
		char pTracerEffectCrit[128];

		Q_snprintf( pTracerEffect, sizeof(pTracerEffect), "%s_red", pTFInfo->m_szTracerEffect );
		Q_snprintf( pTracerEffectCrit, sizeof(pTracerEffectCrit), "%s_red_crit", pTFInfo->m_szTracerEffect );
		PrecacheParticleSystem( pTracerEffect );
		PrecacheParticleSystem( pTracerEffectCrit );

		Q_snprintf( pTracerEffect, sizeof(pTracerEffect), "%s_blue", pTFInfo->m_szTracerEffect );
		Q_snprintf( pTracerEffectCrit, sizeof(pTracerEffectCrit), "%s_blue_crit", pTFInfo->m_szTracerEffect );
		PrecacheParticleSystem( pTracerEffect );
		PrecacheParticleSystem( pTracerEffectCrit );

		Q_snprintf(pTracerEffect, sizeof(pTracerEffect), "%s_green", pTFInfo->m_szTracerEffect);
		Q_snprintf(pTracerEffectCrit, sizeof(pTracerEffectCrit), "%s_green_crit", pTFInfo->m_szTracerEffect);
		PrecacheParticleSystem(pTracerEffect);
		PrecacheParticleSystem(pTracerEffectCrit);

		Q_snprintf(pTracerEffect, sizeof(pTracerEffect), "%s_yellow", pTFInfo->m_szTracerEffect);
		Q_snprintf(pTracerEffectCrit, sizeof(pTracerEffectCrit), "%s_yellow_crit", pTFInfo->m_szTracerEffect);
		PrecacheParticleSystem(pTracerEffect);
		PrecacheParticleSystem(pTracerEffectCrit);

		Q_snprintf(pTracerEffect, sizeof(pTracerEffect), "%s_dm", pTFInfo->m_szTracerEffect);
		Q_snprintf(pTracerEffectCrit, sizeof(pTracerEffectCrit), "%s_dm_crit", pTFInfo->m_szTracerEffect);
		PrecacheParticleSystem(pTracerEffect);
		PrecacheParticleSystem(pTracerEffectCrit);
	}
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
const CTFWeaponInfo &CTFWeaponBase::GetTFWpnData() const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CTFWeaponInfo *pTFInfo = dynamic_cast< const CTFWeaponInfo* >( pWeaponInfo );
	Assert( pTFInfo );
	return *pTFInfo;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
int CTFWeaponBase::GetWeaponID( void ) const
{
	Assert( false ); 
	return TF_WEAPON_NONE; 
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
bool CTFWeaponBase::IsWeapon( int iWeapon ) const
{ 
	return GetWeaponID() == iWeapon; 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFWeaponBase::GetWorldModel(void) const
{
	const char* szModelName = NULL;
	if ( HasItemDefinition() )
	{
		const char* szModelName = m_Item.GetWorldDisplayModel();
		if ( !Q_stricmp( szModelName, "" ) )
		{
			szModelName = m_Item.GetWorldDisplayModel();
		}
	}

	if ( !szModelName || !Q_stricmp( szModelName, "" ) )
	{
		szModelName = BaseClass::GetWorldModel();
	}

	return szModelName;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetMaxClip1(void) const
{
	int iMaxClip = CBaseCombatWeapon::GetMaxClip1();

	float fMaxClipMult = 1.0f;
	CALL_ATTRIB_HOOK_FLOAT( fMaxClipMult, mult_clipsize );
	fMaxClipMult *= iMaxClip;
	if (fMaxClipMult != 0)
		return fMaxClipMult;

	return iMaxClip;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetDefaultClip1(void) const
{
	int iDefaultClip = CBaseCombatWeapon::GetDefaultClip1();

	float fDefaultClipMult = 1.0f;
	CALL_ATTRIB_HOOK_FLOAT( fDefaultClipMult, mult_clipsize );
	fDefaultClipMult *= iDefaultClip;
	if ( fDefaultClipMult != 0 )
		return fDefaultClipMult;

	return iDefaultClip;
}


int PrimaryArmActTable[13][2] = {
	{ ACT_VM_DRAW, ACT_PRIMARY_VM_DRAW },
	{ ACT_VM_HOLSTER, ACT_PRIMARY_VM_HOLSTER },
	{ ACT_VM_IDLE, ACT_PRIMARY_VM_IDLE },
	{ ACT_VM_PULLBACK, ACT_PRIMARY_VM_PULLBACK },
	{ ACT_VM_PRIMARYATTACK, ACT_PRIMARY_VM_PRIMARYATTACK },
	{ ACT_VM_SECONDARYATTACK, ACT_PRIMARY_VM_SECONDARYATTACK },
	{ ACT_VM_RELOAD, ACT_PRIMARY_VM_RELOAD },
	{ ACT_VM_RELOAD_START, ACT_PRIMARY_RELOAD_START },
	{ ACT_VM_RELOAD_FINISH, ACT_PRIMARY_RELOAD_FINISH },
	{ ACT_VM_DRYFIRE, ACT_PRIMARY_VM_DRYFIRE },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_PRIMARY_VM_IDLE_TO_LOWERED },
	{ ACT_VM_IDLE_LOWERED, ACT_PRIMARY_VM_IDLE_LOWERED },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_PRIMARY_VM_LOWERED_TO_IDLE },
};

int SecondaryArmActTable[13][2] = {
	{ ACT_VM_DRAW, ACT_SECONDARY_VM_DRAW },
	{ ACT_VM_HOLSTER, ACT_SECONDARY_VM_HOLSTER },
	{ ACT_VM_IDLE, ACT_SECONDARY_VM_IDLE },
	{ ACT_VM_PULLBACK, ACT_SECONDARY_VM_PULLBACK },
	{ ACT_VM_PRIMARYATTACK, ACT_SECONDARY_VM_PRIMARYATTACK },
	{ ACT_VM_SECONDARYATTACK, ACT_SECONDARY_VM_SECONDARYATTACK },
	{ ACT_VM_RELOAD, ACT_SECONDARY_VM_RELOAD },
	{ ACT_VM_RELOAD_START, ACT_SECONDARY_RELOAD_START },
	{ ACT_VM_RELOAD_FINISH, ACT_SECONDARY_RELOAD_FINISH },
	{ ACT_VM_DRYFIRE, ACT_SECONDARY_VM_DRYFIRE },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_SECONDARY_VM_IDLE_TO_LOWERED },
	{ ACT_VM_IDLE_LOWERED, ACT_SECONDARY_VM_IDLE_LOWERED },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_SECONDARY_VM_LOWERED_TO_IDLE },
};

int MeleeArmActTable[13][2] = {
	{ ACT_VM_DRAW, ACT_MELEE_VM_DRAW },
	{ ACT_VM_HOLSTER, ACT_MELEE_VM_HOLSTER },
	{ ACT_VM_IDLE, ACT_MELEE_VM_IDLE },
	{ ACT_VM_PULLBACK, ACT_MELEE_VM_PULLBACK },
	{ ACT_VM_PRIMARYATTACK, ACT_MELEE_VM_PRIMARYATTACK },
	{ ACT_VM_SECONDARYATTACK, ACT_MELEE_VM_SECONDARYATTACK },
	{ ACT_VM_RELOAD, ACT_MELEE_VM_RELOAD },
	{ ACT_VM_DRYFIRE, ACT_MELEE_VM_DRYFIRE },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_MELEE_VM_IDLE_TO_LOWERED },
	{ ACT_VM_IDLE_LOWERED, ACT_MELEE_VM_IDLE_LOWERED },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_MELEE_VM_LOWERED_TO_IDLE },
	{ ACT_VM_HITCENTER, ACT_MELEE_VM_HITCENTER },
	{ ACT_VM_SWINGHARD, ACT_MELEE_VM_SWINGHARD },
};

int BuildingArmActTable[2][2] = {
	{ ACT_VM_DRAW, ACT_ENGINEER_BLD_VM_DRAW },
	{ ACT_VM_IDLE, ACT_ENGINEER_BLD_VM_IDLE },
};

int PdaArmActTable[11][2] = {
	{ ACT_VM_DRAW, ACT_PDA_VM_DRAW },
	{ ACT_VM_HOLSTER, ACT_PDA_VM_HOLSTER },
	{ ACT_VM_IDLE, ACT_PDA_VM_IDLE },
	{ ACT_VM_PULLBACK, ACT_MELEE_VM_PULLBACK },
	{ ACT_VM_PRIMARYATTACK, ACT_PDA_VM_PRIMARYATTACK },
	{ ACT_VM_SECONDARYATTACK, ACT_PDA_VM_SECONDARYATTACK },
	{ ACT_VM_RELOAD, ACT_PDA_VM_RELOAD },
	{ ACT_VM_DRYFIRE, ACT_PDA_VM_DRYFIRE },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_PDA_VM_IDLE_TO_LOWERED },
	{ ACT_VM_IDLE_LOWERED, ACT_PDA_VM_IDLE_LOWERED },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_PDA_VM_LOWERED_TO_IDLE },
};

int Item1ArmActTable[15][2] = {
	{ ACT_VM_DRAW, ACT_ITEM1_VM_DRAW },
	{ ACT_VM_HOLSTER, ACT_ITEM1_VM_HOLSTER },
	{ ACT_VM_IDLE, ACT_ITEM1_VM_IDLE },
	{ ACT_VM_PULLBACK, ACT_ITEM1_VM_PULLBACK },
	{ ACT_VM_PRIMARYATTACK, ACT_ITEM1_VM_PRIMARYATTACK },
	{ ACT_VM_SECONDARYATTACK, ACT_ITEM1_VM_SECONDARYATTACK },
	{ ACT_VM_RELOAD, ACT_ITEM1_VM_RELOAD },
	{ ACT_VM_DRYFIRE, ACT_ITEM1_VM_DRYFIRE },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_ITEM1_VM_IDLE_TO_LOWERED },
	{ ACT_VM_IDLE_LOWERED, ACT_ITEM1_VM_IDLE_LOWERED },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_ITEM1_VM_LOWERED_TO_IDLE },
	{ ACT_VM_RELOAD_START, ACT_ITEM1_RELOAD_START },
	{ ACT_VM_RELOAD_FINISH, ACT_ITEM1_RELOAD_FINISH },
	{ ACT_VM_HITCENTER, ACT_ITEM1_VM_HITCENTER },
	{ ACT_VM_SWINGHARD, ACT_ITEM1_VM_SWINGHARD },
};

int Item2ArmActTable[13][2] = {
	{ ACT_VM_DRAW, ACT_ITEM2_VM_DRAW },
	{ ACT_VM_HOLSTER, ACT_ITEM2_VM_HOLSTER },
	{ ACT_VM_IDLE, ACT_ITEM2_VM_IDLE },
	{ ACT_VM_PULLBACK, ACT_ITEM2_VM_PULLBACK },
	{ ACT_VM_PRIMARYATTACK, ACT_ITEM2_VM_PRIMARYATTACK },
	{ ACT_VM_SECONDARYATTACK, ACT_ITEM2_VM_SECONDARYATTACK },
	{ ACT_VM_RELOAD, ACT_ITEM2_VM_RELOAD },
	{ ACT_VM_DRYFIRE, ACT_ITEM2_VM_DRYFIRE },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_ITEM2_VM_IDLE_TO_LOWERED },
	{ ACT_VM_IDLE_LOWERED, ACT_ITEM2_VM_IDLE_LOWERED },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_ITEM2_VM_LOWERED_TO_IDLE },
	{ ACT_VM_HITCENTER, ACT_ITEM2_VM_HITCENTER },
	{ ACT_VM_SWINGHARD, ACT_ITEM2_VM_SWINGHARD },
};

int Item3ArmActTable[13][2] = {
	{ ACT_VM_DRAW, ACT_ITEM3_VM_DRAW },
	{ ACT_VM_HOLSTER, ACT_ITEM3_VM_HOLSTER },
	{ ACT_VM_IDLE, ACT_ITEM3_VM_IDLE },
	{ ACT_VM_PULLBACK, ACT_ITEM3_VM_PULLBACK },
	{ ACT_VM_PRIMARYATTACK, ACT_ITEM3_VM_PRIMARYATTACK },
	{ ACT_VM_SECONDARYATTACK, ACT_ITEM3_VM_SECONDARYATTACK },
	{ ACT_VM_RELOAD, ACT_ITEM3_VM_RELOAD },
	{ ACT_VM_DRYFIRE, ACT_ITEM3_VM_DRYFIRE },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_ITEM3_VM_IDLE_TO_LOWERED },
	{ ACT_VM_IDLE_LOWERED, ACT_ITEM3_VM_IDLE_LOWERED },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_ITEM3_VM_LOWERED_TO_IDLE },
	{ ACT_VM_HITCENTER, ACT_ITEM3_VM_HITCENTER },
	{ ACT_VM_SWINGHARD, ACT_ITEM3_VM_SWINGHARD },
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::TranslateViewmodelHandActivity( int iActivity )
{
	int iWeaponRole = GetTFWpnData().m_iWeaponType;

	if ( HasItemDefinition() && m_Item.GetStaticData()->anim_slot > -1 )
		iWeaponRole = m_Item.GetStaticData()->anim_slot;

	if ( HasItemDefinition() )
	{
		Activity actActivityOverride = m_Item.GetActivityOverride( GetTeamNumber(), (Activity)iActivity );
		if ( actActivityOverride != ACT_INVALID )
		{
			iActivity = actActivityOverride;
			return iActivity;
		}
	}

	CTFPlayer *pTFPlayer = ToTFPlayer (GetOwner() );
	if ( pTFPlayer == NULL )
	{
		Assert(false); // This shouldn't be possible
		return iActivity;
	}

	CTFViewModel *vm = dynamic_cast<CTFViewModel*>( pTFPlayer->GetViewModel( m_nViewModelIndex, false ) );
	if ( vm == NULL )
	{
		return iActivity;
	}

	if ( vm->GetViewModelType() != vm->VMTYPE_TF2 )
		return iActivity;
	
	switch ( iWeaponRole )
	{
		case TF_WPN_TYPE_PRIMARY:
			for (int i = 0; i < 13; i++)
			{
				if (PrimaryArmActTable[i][0] == iActivity)
					return PrimaryArmActTable[i][1];
			}
			return iActivity;
		case TF_WPN_TYPE_SECONDARY:
			for (int i = 0; i < 13; i++)
			{
				if ( SecondaryArmActTable[i][0] == iActivity )
					return SecondaryArmActTable[i][1];
			}
			return iActivity;

		case TF_WPN_TYPE_MELEE:
			for (int i = 0; i < 13; i++)
			{
				if (MeleeArmActTable[i][0] == iActivity)
					return MeleeArmActTable[i][1];
			}
			return iActivity;

		case TF_WPN_TYPE_BUILDING:
			for (int i = 0; i < 2; i++)
			{
				if (BuildingArmActTable[i][0] == iActivity)
					return BuildingArmActTable[i][1];
			}
			return iActivity;

		case TF_WPN_TYPE_PDA:
			for (int i = 0; i < 13; i++)
			{
				if (PdaArmActTable[i][0] == iActivity)
					return PdaArmActTable[i][1];
			}
			return iActivity;

		case TF_WPN_TYPE_ITEM1:
			for (int i = 0; i < 13; i++)
			{
				if (Item1ArmActTable[i][0] == iActivity)
					return Item1ArmActTable[i][1];
			}
			return iActivity;

		case TF_WPN_TYPE_ITEM2:
			for (int i = 0; i < 13; i++)
			{
				if (Item2ArmActTable[i][0] == iActivity)
					return Item2ArmActTable[i][1];
			}
			return iActivity;

		default:
			return iActivity;
	};
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::SetViewModel()
{
	CTFPlayer *pTFPlayer = ToTFPlayer(GetOwner());
	if ( pTFPlayer == NULL )
		return;

	CTFViewModel *vm = dynamic_cast<CTFViewModel*>(pTFPlayer->GetViewModel(m_nViewModelIndex, false));
	if ( vm == NULL )
		return;

	Assert( vm->ViewModelIndex() == m_nViewModelIndex );

	vm->SetViewModelType( vm->VMTYPE_NONE );

	vm->SetWeaponModel( GetViewModel( m_nViewModelIndex ), this );

#ifdef CLIENT_DLL
	UpdateViewModel();
#endif
}

#ifdef CLIENT_DLL
void CTFWeaponBase::UpdateViewModel(void)
{
	CTFPlayer *pTFPlayer = ToTFPlayer( GetOwner() );
	if ( pTFPlayer == NULL )
		return;

	CTFViewModel *vm = dynamic_cast<CTFViewModel*>( pTFPlayer->GetViewModel(m_nViewModelIndex, false ) );
	if ( vm == NULL )
		return;
	
	GetViewModel( m_nViewModelIndex );


	int vmType = vm->GetViewModelType();
	if ( vmType == vm->VMTYPE_L4D )
		vm->UpdateViewmodelAddon( pTFPlayer->GetPlayerClass()->GetHandModelName() );
	else if (vmType == vm->VMTYPE_TF2)
	{
		if ( HasItemDefinition() )
		{
			const char* pModel = m_Item.GetPlayerDisplayModel();
			if ( pModel )
			{
				vm->UpdateViewmodelAddon( pModel );
			}
		}
		else
			vm->UpdateViewmodelAddon(GetTFWpnData().szViewModel);
	}
	else
		vm->RemoveViewmodelAddon();
}
#endif

const char *CTFWeaponBase::DetermineViewModelType( const char *vModel ) const
{
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if (!pPlayer)
		return vModel;

	CBaseAnimating *pTemp = new CBaseAnimating();
	if (!pTemp)
		return vModel;

	CTFViewModel *vm = dynamic_cast<CTFViewModel*>(pPlayer->GetViewModel(m_nViewModelIndex, false));

	pTemp->SetModel(vModel);

	if ( pTemp->LookupAttachment("l4d") > 0 )
	{
		pTemp->Remove();

		if ( vm )
			vm->SetViewModelType( vm->VMTYPE_L4D );

		return vModel;
	}
	else if ( pTemp->SelectWeightedSequence( ACT_VM_IDLE ) == -1 )
	{
		pTemp->Remove();

		if ( vm )
			vm->SetViewModelType( vm->VMTYPE_TF2 );

		return pPlayer->GetPlayerClass()->GetHandModelName();
	}

	pTemp->Remove();

	if (vm)
		vm->SetViewModelType(vm->VMTYPE_HL2);

	return vModel;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
const char *CTFWeaponBase::GetViewModel( int iViewModel ) const
{
	if ( HasItemDefinition() )
	{
		const char* szModelName = m_Item.GetPlayerDisplayModel();
		if ( szModelName[0] != '\0' )
		{ 
			return DetermineViewModelType( szModelName );
		}
	}
	
	if ( TFGameRules() && TFGameRules()->IsDeathmatch() )
	{
		if (GetTFWpnData().m_szViewModelDM[0] != '\0')
			return DetermineViewModelType( GetTFWpnData().m_szViewModelDM );
	}

	if ( GetPlayerOwner() )
	{
		return DetermineViewModelType( GetTFWpnData().szViewModel );
	}
	else
	{
		return BaseClass::GetViewModel();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::Drop( const Vector &vecVelocity )
{
#ifndef CLIENT_DLL
	if ( m_iAltFireHint )
	{
		CBasePlayer *pPlayer = GetPlayerOwner();
		if ( pPlayer )
		{
			pPlayer->StopHintTimer( m_iAltFireHint );
		}
	}
#endif

	BaseClass::Drop( vecVelocity );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBase::Holster( CBaseCombatWeapon *pSwitchingTo )
{
#ifndef CLIENT_DLL
	if ( m_iAltFireHint )
	{
		CBasePlayer *pPlayer = GetPlayerOwner();
		if ( pPlayer )
		{
			pPlayer->StopHintTimer( m_iAltFireHint );
		}
	}
#endif

	AbortReload();

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBase::Deploy( void )
{
#ifndef CLIENT_DLL
	if ( m_iAltFireHint )
	{
		CBasePlayer *pPlayer = GetPlayerOwner();
		if ( pPlayer )
		{
			pPlayer->StartHintTimer( m_iAltFireHint );
		}
	}
#endif

	float flOriginalPrimaryAttack = m_flNextPrimaryAttack;

	bool bDeploy = BaseClass::Deploy();

	if ( bDeploy )
	{
		// Make sure viewmodel index is correct since deathmatch uses alt viewmodels.
		// May also help custom weapons in the future.
		m_iViewModelIndex = modelinfo->GetModelIndex( GetViewModel() );

		// Overrides the anim length for calculating ready time.
		// Don't override primary attacks that are already further out than this. This prevents
		// people exploiting weapon switches to allow weapons to fire faster.
		float flDeployTime = 0.67;
		m_flNextPrimaryAttack = max( flOriginalPrimaryAttack, gpGlobals->curtime + flDeployTime );

		CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );
		if (!pPlayer)
			return false;

		GetViewModel( m_nViewModelIndex );

		pPlayer->SetNextAttack( m_flNextPrimaryAttack );
	}

	return bDeploy;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::OnActiveStateChanged( int iOldState )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();

	if ( pOwner )
	{
		int iProvideOnActive = 0;
		CALL_ATTRIB_HOOK_INT( iProvideOnActive, provide_on_active );

		// Just got equip add us to attribute providers list.
		if ( iOldState == WEAPON_NOT_CARRIED && !iProvideOnActive )
		{
			pOwner->GetAttributeManager()->AddProvider( this );
		}

		// If set to only provide attributes while active, handle it here.
		if ( iProvideOnActive )
		{
			if ( m_iState == WEAPON_IS_ACTIVE )
			{
				pOwner->GetAttributeManager()->AddProvider( this );
			}
			else
			{
				pOwner->GetAttributeManager()->RemoveProvider( this );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::UpdateOnRemove( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();

	// Remove ourselves from attribute providers list.
	if ( pOwner )
	{
		pOwner->GetAttributeManager()->RemoveProvider( this );
	}

	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : 
//-----------------------------------------------------------------------------
void CTFWeaponBase::PrimaryAttack( void )
{
	// Set the weapon mode.
	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	if ( !CanAttack() )
		return;

	BaseClass::PrimaryAttack();

	// Due to cl_autoreload we can now interrupt ANY reload.
	AbortReload();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::OnPickedUp(CBaseCombatCharacter *pNewOwner)
{
#ifdef GAME_DLL
	CTFPlayer *pPlayer = ToTFPlayer(pNewOwner);
	int iAmmoType = m_pWeaponInfo->iAmmoType;

	if ( iAmmoType != -1 )
	{
		pPlayer->SetAmmoCount(1,iAmmoType);
		pPlayer->GiveAmmo(pPlayer->GetMaxAmmo( iAmmoType ), iAmmoType);
	}
#endif

	BaseClass::OnPickedUp(pNewOwner);
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
void CTFWeaponBase::SecondaryAttack( void )
{
	// Set the weapon mode.
	m_iWeaponMode = TF_WEAPON_SECONDARY_MODE;

	// Don't hook secondary for now.
	return;
}

//-----------------------------------------------------------------------------
// Purpose: Most calls use the prediction seed
//-----------------------------------------------------------------------------
void CTFWeaponBase::CalcIsAttackCritical( void)
{
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return;

	if ( gpGlobals->framecount == m_iLastCritCheckFrame )
		return;

	m_iLastCritCheckFrame = gpGlobals->framecount;

	// if base entity seed has changed since last calculation, reseed with new seed
	int iSeed = CBaseEntity::GetPredictionRandomSeed();
	if ( iSeed != m_iCurrentSeed )
	{
		m_iCurrentSeed = iSeed;
		RandomSeed( m_iCurrentSeed );
	}
	if ( ( TFGameRules()->State_Get() == GR_STATE_TEAM_WIN ) && ( TFGameRules()->GetWinningTeam() == pPlayer->GetTeamNumber() ) )
	{
		m_bCurrentAttackIsCrit = true;
	}
	else if ( pPlayer->m_Shared.InCond( TF_COND_CRITBOOSTED ) )
	{
		m_bCurrentAttackIsCrit = true;
	}
	else if  (pPlayer->m_Shared.InCond( TF_COND_POWERUP_CRITDAMAGE ) )
	{
		m_bCurrentAttackIsCrit = true;
	}
	else
	{
		// call the weapon-specific helper method
		m_bCurrentAttackIsCrit = CalcIsAttackCriticalHelper();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Weapon-specific helper method to calculate if attack is crit
//-----------------------------------------------------------------------------
bool CTFWeaponBase::CalcIsAttackCriticalHelper()
{
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return false;

	// Don't bother checking if random crits are off.
	if ( !tf_weapon_criticals.GetBool() )
		return false;

	float flPlayerCritMult = pPlayer->GetCritMult();

	if ( !CanFireCriticalShot() )
		return false;

	if ( m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_bUseRapidFireCrits )
	{
		if ( m_flCritTime > gpGlobals->curtime )
			return true;
		// only perform one crit check per second for rapid fire weapons
		if ( gpGlobals->curtime < m_flLastCritCheckTime + 1.0f )
			return false;
		m_flLastCritCheckTime = gpGlobals->curtime;

		// get the total crit chance (ratio of total shots fired we want to be crits)
		float flTotalCritChance = clamp( TF_DAMAGE_CRIT_CHANCE_RAPID * flPlayerCritMult, 0.01f, 0.99f );
		// get the fixed amount of time that we start firing crit shots for	
		float flCritDuration = TF_DAMAGE_CRIT_DURATION_RAPID;
		// calculate the amount of time, on average, that we want to NOT fire crit shots for in order to achive the total crit chance we want
		float flNonCritDuration = ( flCritDuration / flTotalCritChance ) - flCritDuration;
		// calculate the chance per second of non-crit fire that we should transition into critting such that on average we achieve the total crit chance we want
		float flStartCritChance = 1 / flNonCritDuration;

		// see if we should start firing crit shots
		int iRandom = RandomInt( 0, WEAPON_RANDOM_RANGE-1 );
		if ( iRandom <= flStartCritChance * WEAPON_RANDOM_RANGE )
		{
			m_flCritTime = gpGlobals->curtime + TF_DAMAGE_CRIT_DURATION_RAPID;
			return true;
		}
		
		return false;
	}
	else
	{
		// single-shot weapon, just use random pct per shot
		return ( RandomInt( 0.0, WEAPON_RANDOM_RANGE-1 ) < ( TF_DAMAGE_CRIT_CHANCE * flPlayerCritMult ) * WEAPON_RANDOM_RANGE );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFWeaponBase::Reload( void )
{
	// Sorry, people, no speeding it up.
	if ( m_flNextPrimaryAttack > gpGlobals->curtime )
		return false;

	// If we're not already reloading, check to see if we have ammo to reload and check to see if we are max ammo.
	if ( m_iReloadMode == TF_RELOAD_START ) 
	{
		// If I don't have any spare ammo, I can't reload
		if ( GetOwner()->GetAmmoCount(m_iPrimaryAmmoType) <= 0 )
			return false;

		if ( Clip1() >= GetMaxClip1())
			return false;
	}

	// Reload one object at a time.
	if ( ReloadsSingly() )
		return ReloadSingly();

	// Normal reload.
	DefaultReload( GetMaxClip1(), GetMaxClip2(), ACT_VM_RELOAD );

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::AbortReload( void )
{
	BaseClass::AbortReload();

#ifndef CLIENT_DLL
	StopWeaponSound( RELOAD );
#endif
	m_iReloadMode.Set( TF_RELOAD_START );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFWeaponBase::ReloadSingly( void )
{
	// Don't reload.
	if ( m_flNextPrimaryAttack > gpGlobals->curtime )
		return false;

	// Get the current player.
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return false;

	// check to see if we're ready to reload
	switch ( m_iReloadMode )
	{
	case TF_RELOAD_START:
		{
			// Play weapon and player animations.
			if ( SendWeaponAnim( ACT_RELOAD_START ) )
			{
				SetReloadTimer( SequenceDuration() );
			}
			else
			{
				// Update the reload timers with script values.
				UpdateReloadTimers( true );
			}

			// Next reload the shells.
			m_iReloadMode.Set( TF_RELOADING );

			m_iReloadStartClipAmount = Clip1();

			return true;
		}
	case TF_RELOADING:
		{
			// Did we finish the reload start?  Now we can reload a rocket.
			if ( m_flTimeWeaponIdle > gpGlobals->curtime )
				return false;

			// Play weapon reload animations and sound.
			if ( Clip1() == m_iReloadStartClipAmount )
			{
				pPlayer->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
			}
			else
			{
				pPlayer->DoAnimationEvent( PLAYERANIMEVENT_RELOAD_LOOP );
			}

			m_bReloadedThroughAnimEvent = false;

			if (SendWeaponAnim(ACT_VM_RELOAD))
			{
				if ( GetWeaponID() == TF_WEAPON_GRENADELAUNCHER )
				{
					SetReloadTimer( GetTFWpnData().m_WeaponData[TF_WEAPON_PRIMARY_MODE].m_flTimeReload );
				}
				else
				{
					SetReloadTimer( SequenceDuration() );
				}
			}
			else
			{
				// Update the reload timers.
				UpdateReloadTimers( false );
			}

#ifndef CLIENT_DLL
			WeaponSound( RELOAD );
#endif

			// Next continue to reload shells?
			m_iReloadMode.Set( TF_RELOADING_CONTINUE );

			return true;
		}
	case TF_RELOADING_CONTINUE:
		{
			// Did we finish the reload start?  Now we can finish reloading the rocket.
			if ( m_flTimeWeaponIdle > gpGlobals->curtime )
				return false;

			// If we have ammo, remove ammo and add it to clip
			if ( pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) > 0 && !m_bReloadedThroughAnimEvent )
			{
				m_iClip1 = min( ( m_iClip1 + 1 ), GetMaxClip1() );
				pPlayer->RemoveAmmo( 1, m_iPrimaryAmmoType );
			}

			if ( Clip1() == GetMaxClip1() || pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
			{
				m_iReloadMode.Set( TF_RELOAD_FINISH );
			}
			else
			{
				m_iReloadMode.Set( TF_RELOADING );
			}

			return true;
		}

	case TF_RELOAD_FINISH:
	default:
		{
			if ( SendWeaponAnim( ACT_RELOAD_FINISH ) )
			{
				// We're done, allow primary attack as soon as we like
				//SetReloadTimer( SequenceDuration() );
			}

			pPlayer->DoAnimationEvent( PLAYERANIMEVENT_RELOAD_END );

			m_iReloadMode.Set( TF_RELOAD_START );
			return true;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pEvent - 
//			*pOperator - 
//-----------------------------------------------------------------------------
void CTFWeaponBase::Operator_HandleAnimEvent( animevent_t *pEvent, CBaseCombatCharacter *pOperator )
{
	if ( (pEvent->type & AE_TYPE_NEWEVENTSYSTEM) /*&& (pEvent->type & AE_TYPE_SERVER)*/ )
	{
		if ( pEvent->event == AE_WPN_INCREMENTAMMO )
		{
			if ( pOperator->GetAmmoCount( m_iPrimaryAmmoType ) > 0 && !m_bReloadedThroughAnimEvent )
			{
				m_iClip1 = min( ( m_iClip1 + 1 ), GetMaxClip1() );
				pOperator->RemoveAmmo( 1, m_iPrimaryAmmoType );
			}

			m_bReloadedThroughAnimEvent = true;
			return;
		}
	}
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
bool CTFWeaponBase::DefaultReload( int iClipSize1, int iClipSize2, int iActivity )
{
	// The the owning local player.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return false;

	// Setup and check for reload.
	bool bReloadPrimary = false;
	bool bReloadSecondary = false;

	// If you don't have clips, then don't try to reload them.
	if ( UsesClipsForAmmo1() )
	{
		// need to reload primary clip?
		int primary	= min( iClipSize1 - m_iClip1, pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) );
		if ( primary != 0 )
		{
			bReloadPrimary = true;
		}
	}

	if ( UsesClipsForAmmo2() )
	{
		// need to reload secondary clip?
		int secondary = min( iClipSize2 - m_iClip2, pPlayer->GetAmmoCount( m_iSecondaryAmmoType ) );
		if ( secondary != 0 )
		{
			bReloadSecondary = true;
		}
	}

	// We didn't reload.
	if ( !( bReloadPrimary || bReloadSecondary )  )
		return false;

#ifndef CLIENT_DLL
	// Play reload
	WeaponSound( RELOAD );
#endif

	// Play the player's reload animation
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );

	float flReloadTime;
	// First, see if we have a reload animation
	if ( SendWeaponAnim( iActivity ) )
	{
		flReloadTime = SequenceDuration();
	}
	else
	{
		// No reload animation. Use the script time.
		flReloadTime = GetTFWpnData().m_WeaponData[TF_WEAPON_PRIMARY_MODE].m_flTimeReload;  
		if ( bReloadSecondary )
		{
			flReloadTime = GetTFWpnData().m_WeaponData[TF_WEAPON_SECONDARY_MODE].m_flTimeReload;  
		}
	}

	SetReloadTimer( flReloadTime );

	m_bInReload = true;

	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::UpdateReloadTimers( bool bStart )
{
	// Starting a reload?
	if ( bStart )
	{
		// Get the reload start time.
		SetReloadTimer( m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeReloadStart );
	}
	// In reload.
	else
	{
		SetReloadTimer( m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeReload );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::SetReloadTimer( float flReloadTime )
{
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	float flTime = gpGlobals->curtime + flReloadTime;

	// Set next player attack time (weapon independent).
	pPlayer->m_flNextAttack = flTime;

	// Set next weapon attack times (based on reloading).
	m_flNextPrimaryAttack = flTime;

	// Don't push out secondary attack, because our secondary fire
	// systems are all separate from primary fire (sniper zooming, demoman pipebomb detonating, etc)
	//m_flNextSecondaryAttack = flTime;

	// Set next idle time (based on reloading).
	SetWeaponIdleTime( flTime );
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
bool CTFWeaponBase::PlayEmptySound()
{
	CPASAttenuationFilter filter( this );
	filter.UsePredictionRules();

	// TFTODO: Add default empty sound here!
//	EmitSound( filter, entindex(), "Default.ClipEmpty_Rifle" );

	return false;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBase::SendReloadEvents()
{
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	// Make the player play his reload animation.
	pPlayer->DoAnimationEvent( PLAYERANIMEVENT_RELOAD );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::ItemBusyFrame( void )
{
	// Call into the base ItemBusyFrame.
	BaseClass::ItemBusyFrame();

	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
	{
		return;
	}
	
	if ( (pOwner->m_nButtons & IN_ATTACK2) && m_bInReload == false && m_bInAttack2 == false )
	{
		if ( pOwner->DoClassSpecialSkill() )
		{
			m_flNextSecondaryAttack = gpGlobals->curtime + 0.5;
		}

		m_bInAttack2 = true;
	
	}
	else
	{
		m_bInAttack2 = false;
	}

	// Interrupt a reload.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer )
	{
		if ( pPlayer->m_nButtons & IN_ATTACK )
		{
			if ( ( !ReloadsSingly() || m_iReloadMode != TF_RELOAD_START ) && Clip1() > 0 )
			{
				m_iReloadMode.Set( TF_RELOAD_START );
				m_bInReload = false;

				pPlayer->m_flNextAttack = gpGlobals->curtime;
				m_flNextPrimaryAttack = gpGlobals->curtime;

				SetWeaponIdleTime( gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeIdle );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::ItemPostFrame( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	if ( !pOwner )
	{
		return;
	}

	// debounce InAttack flags
	if ( m_bInAttack && !( pOwner->m_nButtons & IN_ATTACK ) )
	{
		m_bInAttack = false;
	}

	if ( m_bInAttack2 && !( pOwner->m_nButtons & IN_ATTACK2 ) )
	{
		m_bInAttack2 = false;
	}

	// If we're lowered, we're not allowed to fire
	if ( m_bLowered )
		return;

	// Call the base item post frame.
	BaseClass::ItemPostFrame();

	// Check for reload singly interrupts.
	if ( ReloadsSingly() )
	{
		ReloadSinglyPostFrame();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::ReloadSinglyPostFrame( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->curtime )
		return;

	// if the clip is empty and we have ammo remaining, 
	if ( ( ( Clip1() == 0 ) && ( GetOwner()->GetAmmoCount(m_iPrimaryAmmoType) > 0 ) ) ||
		// or we are already in the process of reloading but not finished
		( m_iReloadMode != TF_RELOAD_START ) )
	{
		// reload/continue reloading
		Reload();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBase::WeaponShouldBeLowered( void )
{
	// Can't be in the middle of another animation
	if ( GetIdealActivity() != ACT_VM_IDLE_LOWERED && GetIdealActivity() != ACT_VM_IDLE &&
		GetIdealActivity() != ACT_VM_IDLE_TO_LOWERED && GetIdealActivity() != ACT_VM_LOWERED_TO_IDLE )
		return false;

	if ( m_bLowered )
		return true;

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBase::Ready( void )
{
	// If we don't have the anim, just hide for now
	if ( SelectWeightedSequence( ACT_VM_IDLE_LOWERED ) == ACTIVITY_NOT_AVAILABLE )
	{
		RemoveEffects( EF_NODRAW );
	}

	m_bLowered = false;	
	SendWeaponAnim( ACT_VM_IDLE );

	// Prevent firing until our weapon is back up
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	pPlayer->m_flNextAttack = gpGlobals->curtime + SequenceDuration();
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBase::Lower( void )
{
	AbortReload();

	// If we don't have the anim, just hide for now
	if ( SelectWeightedSequence( ACT_VM_IDLE_LOWERED ) == ACTIVITY_NOT_AVAILABLE )
	{
		AddEffects( EF_NODRAW );
	}

	m_bLowered = true;
	SendWeaponAnim( ACT_VM_IDLE_LOWERED );
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: Show/hide weapon and corresponding view model if any
// Input  : visible - 
//-----------------------------------------------------------------------------
void CTFWeaponBase::SetWeaponVisible( bool visible )
{
	if ( visible )
	{
		RemoveEffects( EF_NODRAW );
	}
	else
	{
		AddEffects( EF_NODRAW );
	}
	
#ifdef CLIENT_DLL
	UpdateVisibility();
#endif

}

//-----------------------------------------------------------------------------
// Purpose: If the current weapon has more ammo, reload it. Otherwise, switch 
//			to the next best weapon we've got. Returns true if it took any action.
//-----------------------------------------------------------------------------
bool CTFWeaponBase::ReloadOrSwitchWeapons( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwner() );
	Assert( pOwner );

	m_bFireOnEmpty = false;

	// If we don't have any ammo, switch to the next best weapon
	if ( !HasAnyAmmo() && m_flNextPrimaryAttack < gpGlobals->curtime && m_flNextSecondaryAttack < gpGlobals->curtime )
	{
		// weapon isn't useable, switch.
		if ( ( (GetWeaponFlags() & ITEM_FLAG_NOAUTOSWITCHEMPTY) == false ) && ( g_pGameRules->SwitchToNextBestWeapon( pOwner, this ) ) )
		{
			m_flNextPrimaryAttack = gpGlobals->curtime + 0.3;
			return true;
		}
	}
	else
	{
		// Weapon is useable. Reload if empty and weapon has waited as long as it has to after firing
		// Also auto-reload if owner has auto-reload enabled.
		if ( UsesClipsForAmmo1() && !AutoFiresFullClip() && 
			(m_iClip1 == 0 || (pOwner && pOwner->ShouldAutoReload() && m_iClip1 < GetMaxClip1() && CanAutoReload())) && 
			(GetWeaponFlags() & ITEM_FLAG_NOAUTORELOAD) == false && 
			m_flNextPrimaryAttack < gpGlobals->curtime && 
			m_flNextSecondaryAttack < gpGlobals->curtime )
		{
			// if we're successfully reloading, we're done
			if ( Reload() )
				return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Allows the weapon to choose proper weapon idle animation
//-----------------------------------------------------------------------------
void CTFWeaponBase::WeaponIdle( void )
{
	//See if we should idle high or low
	if ( WeaponShouldBeLowered() )
	{
		// Move to lowered position if we're not there yet
		if ( GetActivity() != ACT_VM_IDLE_LOWERED && GetActivity() != ACT_VM_IDLE_TO_LOWERED && GetActivity() != ACT_TRANSITION )
		{
			SendWeaponAnim( ACT_VM_IDLE_LOWERED );
		}
		else if ( HasWeaponIdleTimeElapsed() )
		{
			// Keep idling low
			SendWeaponAnim( ACT_VM_IDLE_LOWERED );
		}
	}
	else
	{
		// See if we need to raise immediately
		if ( GetActivity() == ACT_VM_IDLE_LOWERED ) 
		{
			SendWeaponAnim( ACT_VM_IDLE );
		}
		else if ( HasWeaponIdleTimeElapsed() ) 
		{
			if ( !( ReloadsSingly() && m_iReloadMode != TF_RELOAD_START ) )
			{
				SendWeaponAnim( ACT_VM_IDLE );
				m_flTimeWeaponIdle = gpGlobals->curtime + SequenceDuration();
			}
		}
	}
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
const char *CTFWeaponBase::GetMuzzleFlashModel( void )
{ 
	const char *pszModel = GetTFWpnData().m_szMuzzleFlashModel;

	if ( Q_strlen( pszModel ) > 0 )
	{
		return pszModel;
	}

	return NULL;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
const char *CTFWeaponBase::GetMuzzleFlashParticleEffect( void )
{ 
	const char *pszPEffect = GetTFWpnData().m_szMuzzleFlashParticleEffect;

	if ( Q_strlen( pszPEffect ) > 0 )
	{
		return pszPEffect;
	}

	return NULL;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
float CTFWeaponBase::GetMuzzleFlashModelLifetime( void )
{ 
	return GetTFWpnData().m_flMuzzleFlashModelDuration;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
float CTFWeaponBase::GetMuzzleFlashModelScale(void)
{
	return GetTFWpnData().m_flMuzzleFlashModelScale;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFWeaponBase::GetTracerType( void )
{ 
	if ( tf_useparticletracers.GetBool() && GetTFWpnData().m_szTracerEffect && GetTFWpnData().m_szTracerEffect[0] )
	{
		if (GetOwner() && !m_szTracerName[0])
		{
			if (TFGameRules()->IsDeathmatch())
			{
				Q_snprintf(m_szTracerName, MAX_TRACER_NAME, "%s_%s", GetTFWpnData().m_szTracerEffect, "dm");
			}
			else
			{
				switch (GetOwner()->GetTeamNumber())
				{
				case TF_TEAM_RED:
					Q_snprintf(m_szTracerName, MAX_TRACER_NAME, "%s_%s", GetTFWpnData().m_szTracerEffect, "red");
					break;
				case TF_TEAM_BLUE:
					Q_snprintf(m_szTracerName, MAX_TRACER_NAME, "%s_%s", GetTFWpnData().m_szTracerEffect, "blue");
					break;
				case TF_TEAM_GREEN:
					Q_snprintf(m_szTracerName, MAX_TRACER_NAME, "%s_%s", GetTFWpnData().m_szTracerEffect, "green");
					break;
				case TF_TEAM_YELLOW:
					Q_snprintf(m_szTracerName, MAX_TRACER_NAME, "%s_%s", GetTFWpnData().m_szTracerEffect, "yellow");
					break;
				default:
					Q_snprintf(m_szTracerName, MAX_TRACER_NAME, "%s_%s", GetTFWpnData().m_szTracerEffect, "red");
					break;
				}
			}
		}

		//if ( !m_szTracerName[0] )
		//{
		//	Q_snprintf(m_szTracerName, MAX_TRACER_NAME, "%s_%s", GetTFWpnData().m_szTracerEffect, tempString);
		//}

		return m_szTracerName;
	}

	if ( GetWeaponID() == TF_WEAPON_MINIGUN )
		return "BrightTracer";

	return BaseClass::GetTracerType();
}

//=============================================================================
//
// TFWeaponBase functions (Server specific).
//
#if !defined( CLIENT_DLL )

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBase::CheckRespawn()
{
	// Do not respawn.
	return;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
CBaseEntity *CTFWeaponBase::Respawn()
{
	// make a copy of this weapon that is invisible and inaccessible to players (no touch function). The weapon spawn/respawn code
	// will decide when to make the weapon visible and touchable.
	CBaseEntity *pNewWeapon = CBaseEntity::Create( GetClassname(), g_pGameRules->VecWeaponRespawnSpot( this ), GetAbsAngles(), GetOwner() );

	if ( pNewWeapon )
	{
		pNewWeapon->AddEffects( EF_NODRAW );// invisible for now
		pNewWeapon->SetTouch( NULL );// no touch
		pNewWeapon->SetThink( &CTFWeaponBase::AttemptToMaterialize );

		UTIL_DropToFloor( this, MASK_SOLID );

		// not a typo! We want to know when the weapon the player just picked up should respawn! This new entity we created is the replacement,
		// but when it should respawn is based on conditions belonging to the weapon that was taken.
		pNewWeapon->SetNextThink( gpGlobals->curtime + g_pGameRules->FlWeaponRespawnTime( this ) );
	}
	else
	{
		Msg( "Respawn failed to create %s!\n", GetClassname() );
	}

	return pNewWeapon;
}

// -----------------------------------------------------------------------------
// Purpose: Make a weapon visible and tangible.
// -----------------------------------------------------------------------------
void CTFWeaponBase::Materialize()
{
	if ( IsEffectActive( EF_NODRAW ) )
	{
		RemoveEffects( EF_NODRAW );
		DoMuzzleFlash();
	}

	AddSolidFlags( FSOLID_TRIGGER );

	SetThink ( &CTFWeaponBase::SUB_Remove );
	SetNextThink( gpGlobals->curtime + 1 );
}

// -----------------------------------------------------------------------------
// Purpose: The item is trying to materialize, should it do so now or wait longer?
// -----------------------------------------------------------------------------
void CTFWeaponBase::AttemptToMaterialize()
{
	float flTime = g_pGameRules->FlWeaponTryRespawn( this );

	if ( flTime == 0 )
	{
		Materialize();
		return;
	}

	SetNextThink( gpGlobals->curtime + flTime );
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBase::SetDieThink( bool bDie )
{
	if( bDie )
	{
		SetContextThink( &CTFWeaponBase::Die, gpGlobals->curtime + 30.0f, "DieContext" );
	}
	else
	{
		SetContextThink( NULL, gpGlobals->curtime, "DieContext" );
	}
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBase::Die( void )
{
	UTIL_Remove( this );
}

void CTFWeaponBase::WeaponReset( void )
{
	m_iReloadMode.Set( TF_RELOAD_START );

	m_bResetParity = !m_bResetParity;
}

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
const Vector &CTFWeaponBase::GetBulletSpread( void )
{
	static Vector cone = VECTOR_CONE_15DEGREES;
	return cone;
}

#else

void TE_DynamicLight( IRecipientFilter& filter, float delay,
					 const Vector* org, int r, int g, int b, int exponent, float radius, float time, float decay, int nLightIndex = LIGHT_INDEX_TE_DYNAMIC );

//=============================================================================
//
// TFWeaponBase functions (Client specific).
//
void CTFWeaponBase::CreateMuzzleFlashEffects( C_BaseEntity *pAttachEnt, int nIndex )
{
	Vector vecOrigin;
	QAngle angAngles;

	int iMuzzleFlashAttachment = pAttachEnt->LookupAttachment( "muzzle" );

	const char *pszMuzzleFlashEffect = NULL;
	const char *pszMuzzleFlashModel = GetMuzzleFlashModel();
	const char *pszMuzzleFlashParticleEffect = GetMuzzleFlashParticleEffect();

	// Pick the right muzzleflash (3rd / 1st person)
	CTFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( pLocalPlayer && (GetOwnerEntity() == pLocalPlayer || 
		(pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE && pLocalPlayer->GetObserverTarget() == GetOwnerEntity())) )
	{
		pszMuzzleFlashEffect = GetMuzzleFlashEffectName_1st();
	}
	else
	{
		pszMuzzleFlashEffect = GetMuzzleFlashEffectName_3rd();
	}

	// If we have an attachment, then stick a light on it.
	if ( iMuzzleFlashAttachment > 0 && (pszMuzzleFlashEffect || pszMuzzleFlashModel || pszMuzzleFlashParticleEffect ) )
	{
		pAttachEnt->GetAttachment( iMuzzleFlashAttachment, vecOrigin, angAngles );

		// Muzzleflash light
		if (tf2c_muzzlelight.GetBool())
		{
			CLocalPlayerFilter filter;
			TE_DynamicLight(filter, 0.0f, &vecOrigin, 255, 192, 64, 5, 70.0f, 0.05f, 70.0f / 0.05f, LIGHT_INDEX_MUZZLEFLASH);
		}
		
		if ( pszMuzzleFlashEffect )
		{
			// Using an muzzle flash dispatch effect
			CEffectData muzzleFlashData;
			muzzleFlashData.m_vOrigin = vecOrigin;
			muzzleFlashData.m_vAngles = angAngles;
			muzzleFlashData.m_hEntity = pAttachEnt->GetRefEHandle();
			muzzleFlashData.m_nAttachmentIndex = iMuzzleFlashAttachment;
			//muzzleFlashData.m_nHitBox = GetDODWpnData().m_iMuzzleFlashType;
			//muzzleFlashData.m_flMagnitude = GetDODWpnData().m_flMuzzleFlashScale;
			muzzleFlashData.m_flMagnitude = 0.2;
			DispatchEffect( pszMuzzleFlashEffect, muzzleFlashData );
		}

		if (pszMuzzleFlashModel && tf2c_model_muzzleflash.GetBool())
		{
			float flEffectLifetime = GetMuzzleFlashModelLifetime();

			// Using a model as a muzzle flash.
			if ( m_hMuzzleFlashModel[nIndex] )
			{
				// Increase the lifetime of the muzzleflash
				m_hMuzzleFlashModel[nIndex]->SetLifetime( flEffectLifetime );
			}
			else
			{
				m_hMuzzleFlashModel[nIndex] = C_MuzzleFlashModel::CreateMuzzleFlashModel( pszMuzzleFlashModel, pAttachEnt, iMuzzleFlashAttachment, flEffectLifetime );

				// FIXME: This is an incredibly brutal hack to get muzzle flashes positioned correctly for recording
				m_hMuzzleFlashModel[nIndex]->SetIs3rdPersonFlash( nIndex == 1 );
			}

			m_hMuzzleFlashModel[nIndex]->SetModelScale(GetMuzzleFlashModelScale());

			// If we use a muzzle model, we don't need to do the particle effect
			return;
		}

		if ( pszMuzzleFlashParticleEffect ) 
		{
			DispatchParticleEffect( pszMuzzleFlashParticleEffect, PATTACH_POINT_FOLLOW, pAttachEnt, "muzzle" );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFWeaponBase::InternalDrawModel( int flags )
{
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	bool bNotViewModel = ( (pOwner && !pOwner->IsLocalPlayer()) || C_BasePlayer::ShouldDrawLocalPlayer() );
	bool bUseInvulnMaterial = (bNotViewModel && pOwner && pOwner->m_Shared.InCond( TF_COND_INVULNERABLE ));
	if ( bUseInvulnMaterial )
	{
		modelrender->ForcedMaterialOverride( *pOwner->GetInvulnMaterialRef() );
	}

	int ret = BaseClass::InternalDrawModel( flags );

	if ( bUseInvulnMaterial )
	{
		modelrender->ForcedMaterialOverride( NULL );
	}

	return ret;
}

void CTFWeaponBase::ProcessMuzzleFlashEvent( void )
{
	C_BaseEntity *pAttachEnt;
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );

	if ( pOwner == NULL )
		return;

	bool bDrawMuzzleFlashOnViewModel = ( pOwner->IsLocalPlayer() && !C_BasePlayer::ShouldDrawLocalPlayer() ) ||
		( IsLocalPlayerSpectator() && GetSpectatorMode() == OBS_MODE_IN_EYE && GetSpectatorTarget() == pOwner->entindex() );

	if ( bDrawMuzzleFlashOnViewModel )
	{
		pAttachEnt = pOwner->GetViewModel();
	}
	else
	{
		pAttachEnt = this;
	}

	{
		CRecordEffectOwner recordOwner( pOwner, bDrawMuzzleFlashOnViewModel );
		CreateMuzzleFlashEffects( pAttachEnt, 0 );
	}

	// Quasi-evil
	int nModelIndex = GetModelIndex();
	int nWorldModelIndex = GetWorldModelIndex();
	bool bInToolRecordingMode = ToolsEnabled() && clienttools->IsInRecordingMode();
	if ( bInToolRecordingMode && nModelIndex != nWorldModelIndex && pOwner->IsLocalPlayer() )
	{
		CRecordEffectOwner recordOwner( pOwner, false );

		SetModelIndex( nWorldModelIndex );
		CreateMuzzleFlashEffects( this, 1 );
		SetModelIndex( nModelIndex );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
bool CTFWeaponBase::ShouldPredict()
{
	if ( GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer() )
	{
		return true;
	}

	return BaseClass::ShouldPredict();
}

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
void CTFWeaponBase::WeaponReset( void )
{
	UpdateVisibility();
}

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
void CTFWeaponBase::OnPreDataChanged( DataUpdateType_t type )
{
	BaseClass::OnPreDataChanged( type );

	m_bOldResetParity = m_bResetParity;

}

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
void CTFWeaponBase::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( GetPredictable() && !ShouldPredict() )
	{
		ShutdownPredictable();
	}

	//If its a world (held or dropped) model then set the correct skin color here.
	if ( m_nModelIndex == GetWorldModelIndex() )
	{
		m_nSkin = GetSkin();
	}

	if ( m_bResetParity != m_bOldResetParity )
	{
		WeaponReset();
	}

	//Here we go...
	//Since we can't get a repro for the invisible weapon thing, I'll fix it right up here:
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );

	//Our owner is alive
	if ( pOwner && pOwner->IsAlive() == true )
	{
		//And he is NOT taunting
		if ( pOwner->m_Shared.InCond ( TF_COND_TAUNTING ) == false )
		{
			//Then why the hell am I NODRAW?
			if ( pOwner->GetActiveWeapon() == this && IsEffectActive( EF_NODRAW ) )
			{
				RemoveEffects( EF_NODRAW );
				UpdateVisibility();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
int CTFWeaponBase::GetWorldModelIndex( void )
{
	CTFPlayer *pPlayer = GetTFPlayerOwner();

	if ( pPlayer )
	{
		// if we're a spy and we're disguised, we also
		// want to disguise our weapon's world model

		CTFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
		if ( !pLocalPlayer )
			return 0;

		int iLocalTeam = pLocalPlayer->GetTeamNumber();

		// We only show disguise weapon to the enemy team when owner is disguised
		bool bUseDisguiseWeapon = ( pPlayer->GetTeamNumber() != iLocalTeam && iLocalTeam > LAST_SHARED_TEAM );

		if ( bUseDisguiseWeapon && pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
		{
			int iModelIndex = pPlayer->m_Shared.GetDisguiseWeaponModelIndex();

			Assert( iModelIndex != -1 );

			return iModelIndex;
		}	
	}
	return BaseClass::GetWorldModelIndex();
}

bool CTFWeaponBase::ShouldDrawCrosshair( void )
{
	return GetTFWpnData().m_WeaponData[TF_WEAPON_PRIMARY_MODE].m_bDrawCrosshair;
}

void CTFWeaponBase::Redraw()
{
	if ( ShouldDrawCrosshair() && g_pClientMode->ShouldDrawCrosshair() )
	{
		DrawCrosshair();
	}
}

#endif

acttable_t CTFWeaponBase::m_acttablePrimary[] = 
{
	{ ACT_MP_STAND_IDLE,		ACT_MP_STAND_PRIMARY,				false },
	{ ACT_MP_CROUCH_IDLE,		ACT_MP_CROUCH_PRIMARY,				false },
	{ ACT_MP_DEPLOYED,			ACT_MP_DEPLOYED_PRIMARY,			false },
	{ ACT_MP_RUN,				ACT_MP_RUN_PRIMARY,					false },
	{ ACT_MP_WALK,				ACT_MP_WALK_PRIMARY,				false },
	{ ACT_MP_AIRWALK,			ACT_MP_AIRWALK_PRIMARY,				false },
	{ ACT_MP_CROUCHWALK,		ACT_MP_CROUCHWALK_PRIMARY,			false },
	{ ACT_MP_JUMP,				ACT_MP_JUMP_PRIMARY,				false },
	{ ACT_MP_JUMP_START,		ACT_MP_JUMP_START_PRIMARY,			false },
	{ ACT_MP_JUMP_FLOAT,		ACT_MP_JUMP_FLOAT_PRIMARY,			false },
	{ ACT_MP_JUMP_LAND,			ACT_MP_JUMP_LAND_PRIMARY,			false },
	{ ACT_MP_SWIM,				ACT_MP_SWIM_PRIMARY,				false },
	{ ACT_MP_DEPLOYED,			ACT_MP_DEPLOYED_PRIMARY,			false },
	{ ACT_MP_SWIM_DEPLOYED,		ACT_MP_SWIM_DEPLOYED_PRIMARY,		false },
	//{ ACT_MP_CROUCHWALK_DEPLOYED, ACT_MP_CROUCHWALK_DEPLOYED_PRIMARY, false },
	//{ ACT_MP_CROUCH_DEPLOYED_IDLE, ACT_MP_CROUCH_DEPLOYED_IDLE_PRIMARY, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,		ACT_MP_ATTACK_STAND_PRIMARY,	false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED,		ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,		ACT_MP_ATTACK_CROUCH_PRIMARY,	false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED,	ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED,	false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE,		ACT_MP_ATTACK_SWIM_PRIMARY,		false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,	ACT_MP_ATTACK_AIRWALK_PRIMARY,	false },

	{ ACT_MP_RELOAD_STAND,		ACT_MP_RELOAD_STAND_PRIMARY,		false },
	{ ACT_MP_RELOAD_STAND_LOOP,	ACT_MP_RELOAD_STAND_PRIMARY_LOOP,	false },
	{ ACT_MP_RELOAD_STAND_END,	ACT_MP_RELOAD_STAND_PRIMARY_END,	false },
	{ ACT_MP_RELOAD_CROUCH,		ACT_MP_RELOAD_CROUCH_PRIMARY,		false },
	{ ACT_MP_RELOAD_CROUCH_LOOP,ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP,	false },
	{ ACT_MP_RELOAD_CROUCH_END,	ACT_MP_RELOAD_CROUCH_PRIMARY_END,	false },
	{ ACT_MP_RELOAD_SWIM,		ACT_MP_RELOAD_SWIM_PRIMARY,			false },
	{ ACT_MP_RELOAD_SWIM_LOOP,	ACT_MP_RELOAD_SWIM_PRIMARY_LOOP,	false },
	{ ACT_MP_RELOAD_SWIM_END,	ACT_MP_RELOAD_SWIM_PRIMARY_END,		false },
	{ ACT_MP_RELOAD_AIRWALK,	ACT_MP_RELOAD_AIRWALK_PRIMARY,		false },
	{ ACT_MP_RELOAD_AIRWALK_LOOP,	ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP,	false },
	{ ACT_MP_RELOAD_AIRWALK_END,ACT_MP_RELOAD_AIRWALK_PRIMARY_END,	false },

	{ ACT_MP_GESTURE_FLINCH,	ACT_MP_GESTURE_FLINCH_PRIMARY, false },

	{ ACT_MP_GRENADE1_DRAW,		ACT_MP_PRIMARY_GRENADE1_DRAW,	false },
	{ ACT_MP_GRENADE1_IDLE,		ACT_MP_PRIMARY_GRENADE1_IDLE,	false },
	{ ACT_MP_GRENADE1_ATTACK,	ACT_MP_PRIMARY_GRENADE1_ATTACK,	false },
	{ ACT_MP_GRENADE2_DRAW,		ACT_MP_PRIMARY_GRENADE2_DRAW,	false },
	{ ACT_MP_GRENADE2_IDLE,		ACT_MP_PRIMARY_GRENADE2_IDLE,	false },
	{ ACT_MP_GRENADE2_ATTACK,	ACT_MP_PRIMARY_GRENADE2_ATTACK,	false },

	{ ACT_MP_ATTACK_STAND_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE,	false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE,	false },
	{ ACT_MP_ATTACK_SWIM_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE,	false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE,	ACT_MP_ATTACK_STAND_GRENADE,	false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH,	ACT_MP_GESTURE_VC_HANDMOUTH_PRIMARY,	false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT,	ACT_MP_GESTURE_VC_FINGERPOINT_PRIMARY,	false },
	{ ACT_MP_GESTURE_VC_FISTPUMP,	ACT_MP_GESTURE_VC_FISTPUMP_PRIMARY,	false },
	{ ACT_MP_GESTURE_VC_THUMBSUP,	ACT_MP_GESTURE_VC_THUMBSUP_PRIMARY,	false },
	{ ACT_MP_GESTURE_VC_NODYES,	ACT_MP_GESTURE_VC_NODYES_PRIMARY,	false },
	{ ACT_MP_GESTURE_VC_NODNO,	ACT_MP_GESTURE_VC_NODNO_PRIMARY,	false },
};

acttable_t CTFWeaponBase::m_acttableSecondary[] = 
{
	{ ACT_MP_STAND_IDLE,		ACT_MP_STAND_SECONDARY,				false },
	{ ACT_MP_CROUCH_IDLE,		ACT_MP_CROUCH_SECONDARY,			false },
	{ ACT_MP_RUN,				ACT_MP_RUN_SECONDARY,				false },
	{ ACT_MP_WALK,				ACT_MP_WALK_SECONDARY,				false },
	{ ACT_MP_AIRWALK,			ACT_MP_AIRWALK_SECONDARY,			false },
	{ ACT_MP_CROUCHWALK,		ACT_MP_CROUCHWALK_SECONDARY,		false },
	{ ACT_MP_JUMP,				ACT_MP_JUMP_SECONDARY,				false },
	{ ACT_MP_JUMP_START,		ACT_MP_JUMP_START_SECONDARY,		false },
	{ ACT_MP_JUMP_FLOAT,		ACT_MP_JUMP_FLOAT_SECONDARY,		false },
	{ ACT_MP_JUMP_LAND,			ACT_MP_JUMP_LAND_SECONDARY,			false },
	{ ACT_MP_SWIM,				ACT_MP_SWIM_SECONDARY,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,		ACT_MP_ATTACK_STAND_SECONDARY,		false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,		ACT_MP_ATTACK_CROUCH_SECONDARY,		false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE,		ACT_MP_ATTACK_SWIM_SECONDARY,		false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,	ACT_MP_ATTACK_AIRWALK_SECONDARY,	false },

	{ ACT_MP_RELOAD_STAND,		ACT_MP_RELOAD_STAND_SECONDARY,		false },
	{ ACT_MP_RELOAD_STAND_LOOP,	ACT_MP_RELOAD_STAND_SECONDARY_LOOP,	false },
	{ ACT_MP_RELOAD_STAND_END,	ACT_MP_RELOAD_STAND_SECONDARY_END,	false },
	{ ACT_MP_RELOAD_CROUCH,		ACT_MP_RELOAD_CROUCH_SECONDARY,		false },
	{ ACT_MP_RELOAD_CROUCH_LOOP,ACT_MP_RELOAD_CROUCH_SECONDARY_LOOP,false },
	{ ACT_MP_RELOAD_CROUCH_END,	ACT_MP_RELOAD_CROUCH_SECONDARY_END,	false },
	{ ACT_MP_RELOAD_SWIM,		ACT_MP_RELOAD_SWIM_SECONDARY,		false },
	{ ACT_MP_RELOAD_SWIM_LOOP,	ACT_MP_RELOAD_SWIM_SECONDARY_LOOP,	false },
	{ ACT_MP_RELOAD_SWIM_END,	ACT_MP_RELOAD_SWIM_SECONDARY_END,	false },
	{ ACT_MP_RELOAD_AIRWALK,	ACT_MP_RELOAD_AIRWALK_SECONDARY,	false },
	{ ACT_MP_RELOAD_AIRWALK_LOOP,	ACT_MP_RELOAD_AIRWALK_SECONDARY_LOOP,	false },
	{ ACT_MP_RELOAD_AIRWALK_END,ACT_MP_RELOAD_AIRWALK_SECONDARY_END,false },

	{ ACT_MP_GESTURE_FLINCH,	ACT_MP_GESTURE_FLINCH_SECONDARY, false },

	{ ACT_MP_GRENADE1_DRAW,		ACT_MP_SECONDARY_GRENADE1_DRAW,	false },
	{ ACT_MP_GRENADE1_IDLE,		ACT_MP_SECONDARY_GRENADE1_IDLE,	false },
	{ ACT_MP_GRENADE1_ATTACK,	ACT_MP_SECONDARY_GRENADE1_ATTACK,	false },
	{ ACT_MP_GRENADE2_DRAW,		ACT_MP_SECONDARY_GRENADE2_DRAW,	false },
	{ ACT_MP_GRENADE2_IDLE,		ACT_MP_SECONDARY_GRENADE2_IDLE,	false },
	{ ACT_MP_GRENADE2_ATTACK,	ACT_MP_SECONDARY_GRENADE2_ATTACK,	false },

	{ ACT_MP_ATTACK_STAND_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE,	false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE,	false },
	{ ACT_MP_ATTACK_SWIM_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE,	false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE,	ACT_MP_ATTACK_STAND_GRENADE,	false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH,	ACT_MP_GESTURE_VC_HANDMOUTH_SECONDARY,	false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT,	ACT_MP_GESTURE_VC_FINGERPOINT_SECONDARY,	false },
	{ ACT_MP_GESTURE_VC_FISTPUMP,	ACT_MP_GESTURE_VC_FISTPUMP_SECONDARY,	false },
	{ ACT_MP_GESTURE_VC_THUMBSUP,	ACT_MP_GESTURE_VC_THUMBSUP_SECONDARY,	false },
	{ ACT_MP_GESTURE_VC_NODYES,	ACT_MP_GESTURE_VC_NODYES_SECONDARY,	false },
	{ ACT_MP_GESTURE_VC_NODNO,	ACT_MP_GESTURE_VC_NODNO_SECONDARY,	false },
};

acttable_t CTFWeaponBase::m_acttableMelee[] = 
{
	{ ACT_MP_STAND_IDLE,		ACT_MP_STAND_MELEE,				false },
	{ ACT_MP_CROUCH_IDLE,		ACT_MP_CROUCH_MELEE,			false },
	{ ACT_MP_RUN,				ACT_MP_RUN_MELEE,				false },
	{ ACT_MP_WALK,				ACT_MP_WALK_MELEE,				false },
	{ ACT_MP_AIRWALK,			ACT_MP_AIRWALK_MELEE,			false },
	{ ACT_MP_CROUCHWALK,		ACT_MP_CROUCHWALK_MELEE,		false },
	{ ACT_MP_JUMP,				ACT_MP_JUMP_MELEE,				false },
	{ ACT_MP_JUMP_START,		ACT_MP_JUMP_START_MELEE,		false },
	{ ACT_MP_JUMP_FLOAT,		ACT_MP_JUMP_FLOAT_MELEE,		false },
	{ ACT_MP_JUMP_LAND,			ACT_MP_JUMP_LAND_MELEE,			false },
	{ ACT_MP_SWIM,				ACT_MP_SWIM_MELEE,				false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,		ACT_MP_ATTACK_STAND_MELEE,		false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,		ACT_MP_ATTACK_CROUCH_MELEE,		false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE,		ACT_MP_ATTACK_SWIM_MELEE,		false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,	ACT_MP_ATTACK_AIRWALK_MELEE,	false },

	{ ACT_MP_ATTACK_STAND_SECONDARYFIRE,	ACT_MP_ATTACK_STAND_MELEE_SECONDARY, false },
	{ ACT_MP_ATTACK_CROUCH_SECONDARYFIRE,	ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY,false },
	{ ACT_MP_ATTACK_SWIM_SECONDARYFIRE,		ACT_MP_ATTACK_SWIM_MELEE,		false },
	{ ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE,	ACT_MP_ATTACK_AIRWALK_MELEE,	false },

	{ ACT_MP_GESTURE_FLINCH,	ACT_MP_GESTURE_FLINCH_MELEE, false },

	{ ACT_MP_GRENADE1_DRAW,		ACT_MP_MELEE_GRENADE1_DRAW,	false },
	{ ACT_MP_GRENADE1_IDLE,		ACT_MP_MELEE_GRENADE1_IDLE,	false },
	{ ACT_MP_GRENADE1_ATTACK,	ACT_MP_MELEE_GRENADE1_ATTACK,	false },
	{ ACT_MP_GRENADE2_DRAW,		ACT_MP_MELEE_GRENADE2_DRAW,	false },
	{ ACT_MP_GRENADE2_IDLE,		ACT_MP_MELEE_GRENADE2_IDLE,	false },
	{ ACT_MP_GRENADE2_ATTACK,	ACT_MP_MELEE_GRENADE2_ATTACK,	false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH,	ACT_MP_GESTURE_VC_HANDMOUTH_MELEE,	false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT,	ACT_MP_GESTURE_VC_FINGERPOINT_MELEE,	false },
	{ ACT_MP_GESTURE_VC_FISTPUMP,	ACT_MP_GESTURE_VC_FISTPUMP_MELEE,	false },
	{ ACT_MP_GESTURE_VC_THUMBSUP,	ACT_MP_GESTURE_VC_THUMBSUP_MELEE,	false },
	{ ACT_MP_GESTURE_VC_NODYES,	ACT_MP_GESTURE_VC_NODYES_MELEE,	false },
	{ ACT_MP_GESTURE_VC_NODNO,	ACT_MP_GESTURE_VC_NODNO_MELEE,	false },
};

acttable_t CTFWeaponBase::m_acttableBuilding[] = 
{
	{ ACT_MP_STAND_IDLE,		ACT_MP_STAND_BUILDING,			false },
	{ ACT_MP_CROUCH_IDLE,		ACT_MP_CROUCH_BUILDING,			false },
	{ ACT_MP_RUN,				ACT_MP_RUN_BUILDING,			false },
	{ ACT_MP_WALK,				ACT_MP_WALK_BUILDING,			false },
	{ ACT_MP_AIRWALK,			ACT_MP_AIRWALK_BUILDING,		false },
	{ ACT_MP_CROUCHWALK,		ACT_MP_CROUCHWALK_BUILDING,		false },
	{ ACT_MP_JUMP,				ACT_MP_JUMP_BUILDING,			false },
	{ ACT_MP_JUMP_START,		ACT_MP_JUMP_START_BUILDING,		false },
	{ ACT_MP_JUMP_FLOAT,		ACT_MP_JUMP_FLOAT_BUILDING,		false },
	{ ACT_MP_JUMP_LAND,			ACT_MP_JUMP_LAND_BUILDING,		false },
	{ ACT_MP_SWIM,				ACT_MP_SWIM_BUILDING,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE,		ACT_MP_ATTACK_STAND_BUILDING,		false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE,		ACT_MP_ATTACK_CROUCH_BUILDING,		false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE,		ACT_MP_ATTACK_SWIM_BUILDING,		false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE,	ACT_MP_ATTACK_AIRWALK_BUILDING,	false },

	{ ACT_MP_ATTACK_STAND_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE_BUILDING,	false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE_BUILDING,	false },
	{ ACT_MP_ATTACK_SWIM_GRENADE,		ACT_MP_ATTACK_STAND_GRENADE_BUILDING,	false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE,	ACT_MP_ATTACK_STAND_GRENADE_BUILDING,	false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH,	ACT_MP_GESTURE_VC_HANDMOUTH_BUILDING,	false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT,	ACT_MP_GESTURE_VC_FINGERPOINT_BUILDING,	false },
	{ ACT_MP_GESTURE_VC_FISTPUMP,	ACT_MP_GESTURE_VC_FISTPUMP_BUILDING,	false },
	{ ACT_MP_GESTURE_VC_THUMBSUP,	ACT_MP_GESTURE_VC_THUMBSUP_BUILDING,	false },
	{ ACT_MP_GESTURE_VC_NODYES,	ACT_MP_GESTURE_VC_NODYES_BUILDING,	false },
	{ ACT_MP_GESTURE_VC_NODNO,	ACT_MP_GESTURE_VC_NODNO_BUILDING,	false },
};

acttable_t CTFWeaponBase::m_acttablePDA[] = 
{
	{ ACT_MP_STAND_IDLE,		ACT_MP_STAND_PDA,			false },
	{ ACT_MP_CROUCH_IDLE,		ACT_MP_CROUCH_PDA,			false },
	{ ACT_MP_RUN,				ACT_MP_RUN_PDA,				false },
	{ ACT_MP_WALK,				ACT_MP_WALK_PDA,			false },
	{ ACT_MP_AIRWALK,			ACT_MP_AIRWALK_PDA,			false },
	{ ACT_MP_CROUCHWALK,		ACT_MP_CROUCHWALK_PDA,		false },
	{ ACT_MP_JUMP,				ACT_MP_JUMP_PDA,			false },
	{ ACT_MP_JUMP_START,		ACT_MP_JUMP_START_PDA,		false },
	{ ACT_MP_JUMP_FLOAT,		ACT_MP_JUMP_FLOAT_PDA,		false },
	{ ACT_MP_JUMP_LAND,			ACT_MP_JUMP_LAND_PDA,		false },
	{ ACT_MP_SWIM,				ACT_MP_SWIM_PDA,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_PDA, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_PDA, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH,	ACT_MP_GESTURE_VC_HANDMOUTH_PDA,	false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT,	ACT_MP_GESTURE_VC_FINGERPOINT_PDA,	false },
	{ ACT_MP_GESTURE_VC_FISTPUMP,	ACT_MP_GESTURE_VC_FISTPUMP_PDA,	false },
	{ ACT_MP_GESTURE_VC_THUMBSUP,	ACT_MP_GESTURE_VC_THUMBSUP_PDA,	false },
	{ ACT_MP_GESTURE_VC_NODYES,	ACT_MP_GESTURE_VC_NODYES_PDA,	false },
	{ ACT_MP_GESTURE_VC_NODNO,	ACT_MP_GESTURE_VC_NODNO_PDA,	false },
};

acttable_t CTFWeaponBase::m_acttableItem1[] =
{
	{ ACT_MP_STAND_IDLE,		ACT_MP_STAND_ITEM1,			false },
	{ ACT_MP_CROUCH_IDLE,		ACT_MP_CROUCH_ITEM1,		false },
	{ ACT_MP_RUN,				ACT_MP_RUN_ITEM1,			false },
	{ ACT_MP_WALK,				ACT_MP_WALK_ITEM1,			false },
	{ ACT_MP_AIRWALK,			ACT_MP_AIRWALK_ITEM1,		false },
	{ ACT_MP_CROUCHWALK,		ACT_MP_CROUCHWALK_ITEM1,	false },
	{ ACT_MP_JUMP,				ACT_MP_JUMP_ITEM1,			false },
	{ ACT_MP_JUMP_START,		ACT_MP_JUMP_START_ITEM1,	false },
	{ ACT_MP_JUMP_FLOAT,		ACT_MP_JUMP_FLOAT_ITEM1,	false },
	{ ACT_MP_JUMP_LAND,			ACT_MP_JUMP_LAND_ITEM1,		false },
	{ ACT_MP_SWIM,				ACT_MP_SWIM_ITEM1,			false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_ITEM1, false },
	{ ACT_MP_ATTACK_STAND_SECONDARYFIRE, ACT_MP_ATTACK_STAND_ITEM1_SECONDARY, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_ITEM1, false },
	{ ACT_MP_ATTACK_CROUCH_SECONDARYFIRE, ACT_MP_ATTACK_CROUCH_ITEM1_SECONDARY, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_ITEM1, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_ITEM1, false },

	{ ACT_MP_DEPLOYED,			ACT_MP_DEPLOYED_ITEM1, false },
	{ ACT_MP_DEPLOYED_IDLE,		ACT_MP_DEPLOYED_IDLE_ITEM1, false },
	{ ACT_MP_CROUCHWALK_DEPLOYED, ACT_MP_CROUCHWALK_DEPLOYED_ITEM1, false },
	{ ACT_MP_CROUCH_DEPLOYED_IDLE, ACT_MP_CROUCH_DEPLOYED_IDLE_ITEM1, false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED, ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED_ITEM1, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED, ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED_ITEM1, false },

	{ ACT_MP_GESTURE_FLINCH, ACT_MP_GESTURE_FLINCH_ITEM1, false },

	{ ACT_MP_GRENADE1_DRAW, ACT_MP_ITEM1_GRENADE1_DRAW, false },
	{ ACT_MP_GRENADE1_IDLE, ACT_MP_ITEM1_GRENADE1_IDLE, false },
	{ ACT_MP_GRENADE1_ATTACK, ACT_MP_ITEM1_GRENADE1_ATTACK, false },
	{ ACT_MP_GRENADE2_DRAW, ACT_MP_ITEM1_GRENADE2_DRAW, false },
	{ ACT_MP_GRENADE2_IDLE, ACT_MP_ITEM1_GRENADE2_IDLE, false },
	{ ACT_MP_GRENADE2_ATTACK, ACT_MP_ITEM1_GRENADE2_ATTACK, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_ITEM1, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_ITEM1, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_ITEM1, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_ITEM1, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_ITEM1, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_ITEM1, false },
};

acttable_t CTFWeaponBase::m_acttableItem2[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_ITEM2, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_ITEM2, false },
	{ ACT_MP_RUN, ACT_MP_RUN_ITEM2, false },
	{ ACT_MP_WALK, ACT_MP_WALK_ITEM2, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_ITEM2, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_ITEM2, false },
	{ ACT_MP_JUMP,	ACT_MP_JUMP_ITEM2, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_ITEM2, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_ITEM2, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_ITEM2, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_ITEM2, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_ITEM2, false },
	{ ACT_MP_ATTACK_STAND_SECONDARYFIRE, ACT_MP_ATTACK_STAND_ITEM2_SECONDARY, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_ITEM2, false },
	{ ACT_MP_ATTACK_CROUCH_SECONDARYFIRE, ACT_MP_ATTACK_CROUCH_ITEM2_SECONDARY, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_ITEM2, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_ITEM2, false },

	{ ACT_MP_DEPLOYED, ACT_MP_DEPLOYED_ITEM2, false },
	{ ACT_MP_DEPLOYED_IDLE, ACT_MP_DEPLOYED_IDLE_ITEM2, false },
	{ ACT_MP_CROUCHWALK_DEPLOYED, ACT_MP_CROUCHWALK_DEPLOYED_ITEM2, false },
	{ ACT_MP_CROUCH_DEPLOYED_IDLE, ACT_MP_CROUCH_DEPLOYED_IDLE_ITEM2, false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED, ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED_ITEM2, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED, ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED_ITEM2, false },

	{ ACT_MP_RELOAD_STAND, ACT_MP_RELOAD_STAND_ITEM2, false },
	{ ACT_MP_RELOAD_STAND_LOOP, ACT_MP_RELOAD_STAND_ITEM2_LOOP, false },
	{ ACT_MP_RELOAD_STAND_END, ACT_MP_RELOAD_STAND_ITEM2_END, false },
	{ ACT_MP_RELOAD_CROUCH, ACT_MP_RELOAD_CROUCH_ITEM2, false },
	{ ACT_MP_RELOAD_CROUCH_LOOP, ACT_MP_RELOAD_CROUCH_ITEM2_LOOP, false },
	{ ACT_MP_RELOAD_CROUCH_END, ACT_MP_RELOAD_CROUCH_ITEM2_END, false },
	{ ACT_MP_RELOAD_SWIM, ACT_MP_RELOAD_SWIM_ITEM2, false },
	{ ACT_MP_RELOAD_SWIM_LOOP, ACT_MP_RELOAD_SWIM_ITEM2_LOOP, false },
	{ ACT_MP_RELOAD_SWIM_END, ACT_MP_RELOAD_SWIM_ITEM2_END, false },
	{ ACT_MP_RELOAD_AIRWALK, ACT_MP_RELOAD_AIRWALK_ITEM2, false },
	{ ACT_MP_RELOAD_AIRWALK_LOOP, ACT_MP_RELOAD_AIRWALK_ITEM2_LOOP, false },
	{ ACT_MP_RELOAD_AIRWALK_END, ACT_MP_RELOAD_AIRWALK_ITEM2_END, false },

	{ ACT_MP_GESTURE_FLINCH, ACT_MP_GESTURE_FLINCH_ITEM2, false },

	{ ACT_MP_GRENADE1_DRAW, ACT_MP_ITEM2_GRENADE1_DRAW, false },
	{ ACT_MP_GRENADE1_IDLE, ACT_MP_ITEM2_GRENADE1_IDLE, false },
	{ ACT_MP_GRENADE1_ATTACK, ACT_MP_ITEM2_GRENADE1_ATTACK, false },
	{ ACT_MP_GRENADE2_DRAW, ACT_MP_ITEM2_GRENADE2_DRAW, false },
	{ ACT_MP_GRENADE2_IDLE, ACT_MP_ITEM2_GRENADE2_IDLE, false },
	{ ACT_MP_GRENADE2_ATTACK, ACT_MP_ITEM2_GRENADE2_ATTACK, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE_ITEM2, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_CROUCH_GRENADE_ITEM2, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_SWIM_GRENADE_ITEM2, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_AIRWALK_GRENADE_ITEM2, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_ITEM2, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_ITEM2, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_ITEM2, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_ITEM2, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_ITEM2, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_ITEM2, false },
};

ConVar mp_forceactivityset( "mp_forceactivityset", "-1", FCVAR_CHEAT|FCVAR_REPLICATED|FCVAR_DEVELOPMENTONLY );

acttable_t *CTFWeaponBase::ActivityList( int &iActivityCount )
{
	int iWeaponRole = GetTFWpnData().m_iWeaponType;

	if ( mp_forceactivityset.GetInt() >= 0 )
	{
		iWeaponRole = mp_forceactivityset.GetInt();
	}

#ifdef CLIENT_DLL
	// If we're disguised, we show a different weapon from what we're actually carrying.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer && pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) && pPlayer->IsEnemyPlayer() )
	{
		CTFWeaponInfo *pWeaponInfo = pPlayer->m_Shared.GetDisguiseWeaponInfo();
		if ( pWeaponInfo )
		{
			iWeaponRole = pWeaponInfo->m_iWeaponType;
		}
	}
#endif

	acttable_t *pTable;

	switch( iWeaponRole )
	{
	case TF_WPN_TYPE_PRIMARY:
	default:
		pTable = m_acttablePrimary;
		iActivityCount = ARRAYSIZE( m_acttablePrimary );
		break;
	case TF_WPN_TYPE_SECONDARY:
		pTable = m_acttableSecondary;
		iActivityCount = ARRAYSIZE( m_acttableSecondary );
		break;
	case TF_WPN_TYPE_MELEE:
		pTable = m_acttableMelee;
		iActivityCount = ARRAYSIZE( m_acttableMelee );
		break;
	case TF_WPN_TYPE_BUILDING:
		pTable = m_acttableBuilding;
		iActivityCount = ARRAYSIZE( m_acttableBuilding );
		break;
	case TF_WPN_TYPE_PDA:
		pTable = m_acttablePDA;
		iActivityCount = ARRAYSIZE( m_acttablePDA );
		break;
	case TF_WPN_TYPE_ITEM1:
		pTable = m_acttableItem1;
		iActivityCount = ARRAYSIZE( m_acttableItem1 );
		break;
	case TF_WPN_TYPE_ITEM2:
		pTable = m_acttableItem2;
		iActivityCount = ARRAYSIZE( m_acttableItem2 );
		break;
	}

	return pTable;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
CBasePlayer *CTFWeaponBase::GetPlayerOwner() const
{
	return dynamic_cast<CBasePlayer*>( GetOwner() );
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
CTFPlayer *CTFWeaponBase::GetTFPlayerOwner() const
{
	return dynamic_cast<CTFPlayer*>( GetOwner() );
}

#ifdef CLIENT_DLL
// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
C_BaseEntity *CTFWeaponBase::GetWeaponForEffect()
{
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return NULL;

#if 0
	// This causes many problems!
	if ( pLocalPlayer->GetObserverMode() == OBS_MODE_IN_EYE )
	{
		C_BasePlayer *pObserverTarget = ToBasePlayer( pLocalPlayer->GetObserverTarget() );
		if ( pObserverTarget )
			return pObserverTarget->GetViewModel();
	}
#endif

	if ( pLocalPlayer == GetTFPlayerOwner() )
		return pLocalPlayer->GetViewModel();

	return this;
}
#endif

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
bool CTFWeaponBase::CanAttack( void )
{
	CTFPlayer *pPlayer = GetTFPlayerOwner();

	if ( pPlayer )
		return pPlayer->CanAttack();

	return false;
}


#if defined( CLIENT_DLL )

static ConVar	cl_bobcycle( "cl_bobcycle","0.8" );
static ConVar	cl_bobup( "cl_bobup","0.5" );

//-----------------------------------------------------------------------------
// Purpose: Helper function to calculate head bob
//-----------------------------------------------------------------------------
float CalcViewModelBobHelper( CBasePlayer *player, BobState_t *pBobState )
{
	Assert( pBobState );
	if ( !pBobState )
		return 0;

	float	cycle;

	//NOTENOTE: For now, let this cycle continue when in the air, because it snaps badly without it

	if ( ( !gpGlobals->frametime ) || ( player == NULL ) )
	{
		//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
		return 0.0f;// just use old value
	}

	//Find the speed of the player
	float speed = player->GetLocalVelocity().Length2D();
	float flmaxSpeedDelta = max( 0, (gpGlobals->curtime - pBobState->m_flLastBobTime ) * 320.0f );

	// don't allow too big speed changes
	speed = clamp( speed, pBobState->m_flLastSpeed-flmaxSpeedDelta, pBobState->m_flLastSpeed+flmaxSpeedDelta );
	speed = clamp( speed, -320, 320 );

	pBobState->m_flLastSpeed = speed;

	//FIXME: This maximum speed value must come from the server.
	//		 MaxSpeed() is not sufficient for dealing with sprinting - jdw

	float bob_offset = RemapVal( speed, 0, 320, 0.0f, 1.0f );

	pBobState->m_flBobTime += ( gpGlobals->curtime - pBobState->m_flLastBobTime ) * bob_offset;
	pBobState->m_flLastBobTime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = pBobState->m_flBobTime - (int)(pBobState->m_flBobTime/cl_bobcycle.GetFloat())*cl_bobcycle.GetFloat();
	cycle /= cl_bobcycle.GetFloat();

	if ( cycle < cl_bobup.GetFloat() )
	{
		cycle = M_PI * cycle / cl_bobup.GetFloat();
	}
	else
	{
		cycle = M_PI + M_PI*(cycle-cl_bobup.GetFloat())/(1.0 - cl_bobup.GetFloat());
	}

	pBobState->m_flVerticalBob = speed*0.005f;
	pBobState->m_flVerticalBob = pBobState->m_flVerticalBob*0.3 + pBobState->m_flVerticalBob*0.7*sin(cycle);

	pBobState->m_flVerticalBob = clamp( pBobState->m_flVerticalBob, -7.0f, 4.0f );

	//Calculate the lateral bob
	cycle = pBobState->m_flBobTime - (int)(pBobState->m_flBobTime/cl_bobcycle.GetFloat()*2)*cl_bobcycle.GetFloat()*2;
	cycle /= cl_bobcycle.GetFloat()*2;

	if ( cycle < cl_bobup.GetFloat() )
	{
		cycle = M_PI * cycle / cl_bobup.GetFloat();
	}
	else
	{
		cycle = M_PI + M_PI*(cycle-cl_bobup.GetFloat())/(1.0 - cl_bobup.GetFloat());
	}

	pBobState->m_flLateralBob = speed*0.005f;
	pBobState->m_flLateralBob = pBobState->m_flLateralBob*0.3 + pBobState->m_flLateralBob*0.7*sin(cycle);
	pBobState->m_flLateralBob = clamp( pBobState->m_flLateralBob, -7.0f, 4.0f );

	//NOTENOTE: We don't use this return value in our case (need to restructure the calculation function setup!)
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Helper function to add head bob
//-----------------------------------------------------------------------------
void AddViewModelBobHelper( Vector &origin, QAngle &angles, BobState_t *pBobState )
{
	Assert( pBobState );
	if ( !pBobState )
		return;

	Vector	forward, right;
	AngleVectors( angles, &forward, &right, NULL );

	// Apply bob, but scaled down to 40%
	VectorMA( origin, pBobState->m_flVerticalBob * 0.4f, forward, origin );

	// Z bob a bit more
	origin[2] += pBobState->m_flVerticalBob * 0.1f;

	// bob the angles
	angles[ ROLL ]	+= pBobState->m_flVerticalBob * 0.5f;
	angles[ PITCH ]	-= pBobState->m_flVerticalBob * 0.4f;
	angles[ YAW ]	-= pBobState->m_flLateralBob  * 0.3f;

	VectorMA( origin, pBobState->m_flLateralBob * 0.2f, right, origin );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float CTFWeaponBase::CalcViewmodelBob( void )
{
	CBasePlayer *player = ToBasePlayer( GetOwner() );
	//Assert( player );
	BobState_t *pBobState = GetBobState();
	if ( pBobState )
		return ::CalcViewModelBobHelper( player, pBobState );
	else
		return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &origin - 
//			&angles - 
//			viewmodelindex - 
//-----------------------------------------------------------------------------
void CTFWeaponBase::AddViewmodelBob( CBaseViewModel *viewmodel, Vector &origin, QAngle &angles )
{
	// call helper functions to do the calculation
	BobState_t *pBobState = GetBobState();
	if ( pBobState )
	{
		CalcViewmodelBob();
		::AddViewModelBobHelper( origin, angles, pBobState );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Returns the head bob state for this weapon, which is stored
//			in the view model.  Note that this this function can return
//			NULL if the player is dead or the view model is otherwise not present.
//-----------------------------------------------------------------------------
BobState_t *CTFWeaponBase::GetBobState()
{
	// get the view model for this weapon
	CBasePlayer *pOwner = ToBasePlayer( GetOwner() );
	if ( pOwner == NULL )
		return NULL;
	CBaseViewModel *baseViewModel = pOwner->GetViewModel( m_nViewModelIndex );
	if ( baseViewModel == NULL )
		return NULL;
	CTFViewModel *viewModel = dynamic_cast<CTFViewModel *>(baseViewModel);
	Assert( viewModel );

	// get the bob state out of the view model
	return &( viewModel->GetBobState() );
}

//-----------------------------------------------------------------------------
// Purpose: Used for spy invisiblity material
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetSkin()
{
	int nSkin = 0;

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer )
	{
		CTFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
		if ( !pLocalPlayer )
			return 0;

		int iLocalTeam = pLocalPlayer->GetTeamNumber();
		int iTeamNumber = pPlayer->GetTeamNumber();

		bool bHasTeamSkins = false;

		// We only show disguise weapon to the enemy team when owner is disguised
		bool bUseDisguiseWeapon = ( iTeamNumber != iLocalTeam && iLocalTeam > LAST_SHARED_TEAM );

		if ( bUseDisguiseWeapon && pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
		{
			CTFWeaponInfo *pInfo = pPlayer->m_Shared.GetDisguiseWeaponInfo();

			if ( pInfo )
			{
				bHasTeamSkins = pInfo->m_bHasTeamSkins_Worldmodel;
			}				

			if ( pLocalPlayer != pPlayer )
			{
				iTeamNumber = pPlayer->m_Shared.GetDisguiseTeam();
			}
		}
		else
		{
			 bHasTeamSkins = GetTFWpnData().m_bHasTeamSkins_Worldmodel;
		}

		if ( bHasTeamSkins )
		{
			switch( iTeamNumber )
			{
			case TF_TEAM_RED:
				nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				nSkin = 1;
				break;
			case TF_TEAM_GREEN:
				nSkin = 2;
				break;
			case TF_TEAM_YELLOW:
				nSkin = 3;
				break;
			}
		}
	}

	return nSkin;
}

bool CTFWeaponBase::OnFireEvent( C_BaseViewModel *pViewModel, const Vector& origin, const QAngle& angles, int event, const char *options )
{
	if( event == 6002 )
	{
		CEffectData data;
		pViewModel->GetAttachment( atoi(options), data.m_vOrigin, data.m_vAngles );
		data.m_nHitBox = GetWeaponID();
		DispatchEffect( "TF_EjectBrass", data );
		return true;
	}
	if ( event == AE_WPN_INCREMENTAMMO )
	{
		CTFPlayer *pPlayer = GetTFPlayerOwner();

		if ( pPlayer && pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) > 0 && !m_bReloadedThroughAnimEvent )
		{
			m_iClip1 = min( ( m_iClip1 + 1 ), GetMaxClip1() );
			pPlayer->RemoveAmmo( 1, m_iPrimaryAmmoType );
		}

		m_bReloadedThroughAnimEvent = true;

		return true;
	}

	return BaseClass::OnFireEvent( pViewModel, origin, angles, event, options );
}

//-----------------------------------------------------------------------------
// Purpose: Used for spy invisiblity material
//-----------------------------------------------------------------------------
class CWeaponInvisProxy : public CEntityMaterialProxy
{
public:

	CWeaponInvisProxy( void );
	virtual ~CWeaponInvisProxy( void );
	virtual bool Init( IMaterial *pMaterial, KeyValues* pKeyValues );
	virtual void OnBind( C_BaseEntity *pC_BaseEntity );
	virtual IMaterial * GetMaterial();

private:
	IMaterialVar *m_pPercentInvisible;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWeaponInvisProxy::CWeaponInvisProxy( void )
{
	m_pPercentInvisible = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CWeaponInvisProxy::~CWeaponInvisProxy( void )
{

}

//-----------------------------------------------------------------------------
// Purpose: Get pointer to the color value
// Input : *pMaterial - 
//-----------------------------------------------------------------------------
bool CWeaponInvisProxy::Init( IMaterial *pMaterial, KeyValues* pKeyValues )
{
	Assert( pMaterial );

	// Need to get the material var
	bool bFound;
	m_pPercentInvisible = pMaterial->FindVar( "$cloakfactor", &bFound );

	return bFound;
}

extern ConVar tf_teammate_max_invis;
//-----------------------------------------------------------------------------
// Purpose: 
// Input :
//-----------------------------------------------------------------------------
void CWeaponInvisProxy::OnBind( C_BaseEntity *pEnt )
{
	if( !m_pPercentInvisible )
		return;

	if ( !pEnt )
		return;

	C_BaseEntity *pMoveParent = pEnt->GetMoveParent();
	if ( !pMoveParent || !pMoveParent->IsPlayer() )
	{
		m_pPercentInvisible->SetFloatValue( 0.0f );
		return;
	}

	CTFPlayer *pPlayer = ToTFPlayer( pMoveParent );
	Assert( pPlayer );

	m_pPercentInvisible->SetFloatValue( pPlayer->GetEffectiveInvisibilityLevel() );
}

IMaterial *CWeaponInvisProxy::GetMaterial()
{
	if ( !m_pPercentInvisible )
		return NULL;

	return m_pPercentInvisible->GetOwningMaterial();
}

EXPOSE_INTERFACE( CWeaponInvisProxy, IMaterialProxy, "weapon_invis" IMATERIAL_PROXY_INTERFACE_VERSION );

#endif // CLIENT_DLL

CTFWeaponInfo *GetTFWeaponInfo(int iWeapon)
{
	// Get the weapon information.
	const char *pszWeaponAlias = WeaponIdToAlias(iWeapon);
	if (!pszWeaponAlias)
	{
		return NULL;
	}

	WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot(pszWeaponAlias);
	if (hWpnInfo == GetInvalidWeaponInfoHandle())
	{
		return NULL;
	}

	CTFWeaponInfo *pWeaponInfo = static_cast<CTFWeaponInfo*>(GetFileWeaponInfoFromHandle(hWpnInfo));
	return pWeaponInfo;
}