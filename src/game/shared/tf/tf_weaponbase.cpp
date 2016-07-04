//========= Copyright © 1996-2004, Valve LLC, All rights reserved. ============
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
#include "c_env_projectedtexture.h"

// for spy material proxy
#include "proxyentity.h"
#include "materialsystem/imaterial.h"
#include "materialsystem/imaterialvar.h"
#endif

extern ConVar r_drawviewmodel;
extern ConVar tf_useparticletracers;

#ifdef CLIENT_DLL
extern ConVar tf2c_model_muzzleflash;
extern ConVar tf2c_muzzlelight;
#endif

ConVar tf_weapon_criticals( "tf_weapon_criticals", "1", FCVAR_NOTIFY | FCVAR_REPLICATED, "Whether or not random crits are enabled." );
ConVar tf2c_weapon_noreload( "tf2c_weapon_noreload", "0", FCVAR_NOTIFY | FCVAR_REPLICATED, "Disables reloading for all weapons." );

#ifdef GAME_DLL
ConVar tf_debug_criticals( "tf_debug_criticals", "0", FCVAR_CHEAT );
#endif

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
	Vector minmaxs[2] = { mins, maxs };
	Vector vecHullEnd = tr.endpos;

	vecHullEnd = vecSrc + ( ( vecHullEnd - vecSrc ) * 2 );
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
					float thisDistance = ( tmpTrace.endpos - vecSrc ).Length();
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

#ifdef CLIENT_DLL
void RecvProxy_Sequence( const CRecvProxyData *pData, void *pStruct, void *pOut );

void RecvProxy_WeaponSequence( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_TFWeaponBase *pWeapon = (C_TFWeaponBase *)pStruct;

	// Weapons carried by other players have different models on server and client
	// so we should ignore sequence changes in such case.
	if ( !pWeapon->GetOwner() || pWeapon->UsingViewModel() )
	{
		RecvProxy_Sequence( pData, pStruct, pOut );
	}
}
#endif

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
	RecvPropTime( RECVINFO( m_flLastFireTime ) ),
	RecvPropTime( RECVINFO( m_flEffectBarRegenTime ) ),

	RecvPropInt( RECVINFO( m_nSequence ), 0, RecvProxy_WeaponSequence ),
	// Server specific.
	#else
	SendPropBool( SENDINFO( m_bLowered ) ),
	SendPropBool( SENDINFO( m_bResetParity ) ),
	SendPropInt( SENDINFO( m_iReloadMode ), 4, SPROP_UNSIGNED ),
	SendPropBool( SENDINFO( m_bReloadedThroughAnimEvent ) ),
	SendPropTime( SENDINFO( m_flLastFireTime ) ),
	SendPropTime( SENDINFO( m_flEffectBarRegenTime ) ),

	SendPropExclude( "DT_BaseAnimating", "m_nSequence" ),
	SendPropInt( SENDINFO( m_nSequence ), ANIMATION_SEQUENCE_BITS, SPROP_UNSIGNED ),
	#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFWeaponBase )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_bLowered, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_iReloadMode, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_bReloadedThroughAnimEvent, FIELD_BOOLEAN, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD_TOL( m_flLastFireTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE, TD_MSECTOLERANCE ),
	DEFINE_PRED_FIELD( m_flEffectBarRegenTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_base, CTFWeaponBase );

// Server specific.
#if !defined( CLIENT_DLL )

BEGIN_DATADESC( CTFWeaponBase )
	DEFINE_THINKFUNC( FallThink )
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
	m_flLastFireTime = 0.0f;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
void CTFWeaponBase::Spawn()
{
	GetAttributeContainer()->InitializeAttributes( this );

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

	// Explosion sound.
	if ( pTFInfo->m_szExplosionSound[0] )
	{
		PrecacheScriptSound( pTFInfo->m_szExplosionSound );
	}

	// Eject brass shells model.
	if ( pTFInfo->m_szBrassModel[0] )
	{
		PrecacheModel( pTFInfo->m_szBrassModel );
	}

	// Muzzle particle.
	if ( pTFInfo->m_szMuzzleFlashParticleEffect[0] )
	{
		PrecacheParticleSystem( pTFInfo->m_szMuzzleFlashParticleEffect );
	}

	// Explosion particles.
	if ( pTFInfo->m_szExplosionEffect[0] )
	{
		PrecacheParticleSystem( pTFInfo->m_szExplosionEffect );
	}

	if ( pTFInfo->m_szExplosionPlayerEffect[0] )
	{
		PrecacheParticleSystem( pTFInfo->m_szExplosionPlayerEffect );
	}

	if ( pTFInfo->m_szExplosionWaterEffect[0] )
	{
		PrecacheParticleSystem( pTFInfo->m_szExplosionWaterEffect );
	}

	if ( pTFInfo->m_szExplosionEffect_Crit[0] )
	{
		PrecacheTeamParticles( pTFInfo->m_szExplosionEffect_Crit, true );
	}

	if ( pTFInfo->m_szExplosionPlayerEffect_Crit[0] )
	{
		PrecacheTeamParticles( pTFInfo->m_szExplosionPlayerEffect_Crit, true );
	}

	if ( pTFInfo->m_szExplosionWaterEffect_Crit[0] )
	{
		PrecacheTeamParticles( pTFInfo->m_szExplosionWaterEffect_Crit, true );
	}

	// Tracers
	if ( pTFInfo->m_szTracerEffect[0] )
	{
		char szTracerEffect[128];
		char szTracerEffectCrit[128];

		V_snprintf( szTracerEffect, sizeof( szTracerEffect ), "%s_%%s", pTFInfo->m_szTracerEffect );
		V_snprintf( szTracerEffectCrit, sizeof( szTracerEffectCrit ), "%s_%%s_crit", pTFInfo->m_szTracerEffect );
		PrecacheTeamParticles( szTracerEffect, true );
		PrecacheTeamParticles( szTracerEffectCrit, true );
	}
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
const CTFWeaponInfo &CTFWeaponBase::GetTFWpnData() const
{
	const FileWeaponInfo_t *pWeaponInfo = &GetWpnData();
	const CTFWeaponInfo *pTFInfo = dynamic_cast<const CTFWeaponInfo*>( pWeaponInfo );
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
int CTFWeaponBase::TranslateViewmodelHandActivity( int iActivity )
{
	CTFPlayer *pTFPlayer = ToTFPlayer( GetOwner() );
	if ( pTFPlayer == NULL )
	{
		Assert( false ); // This shouldn't be possible
		return iActivity;
	}

	CTFViewModel *vm = dynamic_cast<CTFViewModel*>( pTFPlayer->GetViewModel( m_nViewModelIndex, false ) );
	if ( vm == NULL )
	{
		return iActivity;
	}

	// This is only used by TF2 VM type.
	if ( vm->GetViewModelType() != VMTYPE_TF2 )
		return iActivity;

	int iWeaponRole = GetTFWpnData().m_iWeaponType;

	if ( HasItemDefinition() )
	{
		int iSchemaRole = m_Item.GetAnimationSlot();
		if ( iSchemaRole >= 0 )
		{
			iWeaponRole = iSchemaRole;
		}

		Activity actActivityOverride = m_Item.GetActivityOverride( GetTeamNumber(), (Activity)iActivity );
		if ( actActivityOverride != iActivity )
		{
			return actActivityOverride;
		}
	}

	for ( int i = 0; i <= 160; i++ )
	{
		const viewmodel_acttable_t& act = s_viewmodelacttable[i];
		if ( iActivity == act.actBaseAct && iWeaponRole == act.iWeaponRole )
		{
			return act.actTargetAct;
		}
	}

	return iActivity;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::SetViewModel()
{
	CTFPlayer *pTFPlayer = ToTFPlayer( GetOwner() );
	if ( pTFPlayer == NULL )
		return;

	CTFViewModel *vm = dynamic_cast<CTFViewModel*>( pTFPlayer->GetViewModel( m_nViewModelIndex, false ) );
	if ( vm == NULL )
		return;

	Assert( vm->ViewModelIndex() == m_nViewModelIndex );

	vm->SetViewModelType( VMTYPE_NONE );

	const char *pszModelName = GetViewModel( m_nViewModelIndex );

	m_iViewModelIndex = modelinfo->GetModelIndex( pszModelName );

	vm->SetWeaponModel( pszModelName, this );

#ifdef CLIENT_DLL
	UpdateViewModel();
#endif
}

#ifdef CLIENT_DLL
void CTFWeaponBase::UpdateViewModel( void )
{
	CTFPlayer *pTFPlayer = ToTFPlayer( GetOwner() );
	if ( pTFPlayer == NULL )
		return;

	CTFViewModel *vm = dynamic_cast<CTFViewModel*>( pTFPlayer->GetViewModel( m_nViewModelIndex, false ) );
	if ( vm == NULL )
		return;

	int vmType = vm->GetViewModelType();
	const char *pszModel = NULL;

	if ( vmType == VMTYPE_L4D )
	{
		pszModel = pTFPlayer->GetPlayerClass()->GetHandModelName();
	}
	else if ( vmType == VMTYPE_TF2 )
	{
		if ( HasItemDefinition() )
		{
			pszModel = m_Item.GetPlayerDisplayModel( pTFPlayer->GetPlayerClass()->GetClassIndex() );
		}
		else
		{
			pszModel = GetTFWpnData().szViewModel;
		}
	}

	if ( pszModel && pszModel[0] != '\0' )
	{
		vm->UpdateViewmodelAddon( pszModel );
	}
	else
	{
		vm->RemoveViewmodelAddon();
	}
}
#endif

const char *CTFWeaponBase::DetermineViewModelType( const char *vModel ) const
{
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( !pPlayer )
		return vModel;

	CEconItemDefinition *pStatic = m_Item.GetStaticData();

	if ( pStatic )
	{
		int iType = pStatic->attach_to_hands;

		CTFViewModel *vm = dynamic_cast<CTFViewModel *>( pPlayer->GetViewModel( m_nViewModelIndex ) );
		if ( vm )
			vm->SetViewModelType( iType );

		if ( iType == VMTYPE_TF2 )
			return pPlayer->GetPlayerClass()->GetHandModelName();
	}

	return vModel;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
const char *CTFWeaponBase::GetViewModel( int iViewModel ) const
{
	const char *pszModelName = NULL;
	CTFPlayer *pOwner = GetTFPlayerOwner();

	if ( pOwner && HasItemDefinition() )
	{
		pszModelName = m_Item.GetPlayerDisplayModel( pOwner->GetPlayerClass()->GetClassIndex() );
	}
	else
	{
		pszModelName = BaseClass::GetViewModel( iViewModel );
	}

	return DetermineViewModelType( pszModelName );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFWeaponBase::GetWorldModel( void ) const
{
	// Use model from item schema if we have an item ID.
	if ( HasItemDefinition() )
	{
		return m_Item.GetWorldDisplayModel();
	}

	return BaseClass::GetWorldModel();
}

#ifdef DM_WEAPON_BUCKET
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetSlot( void ) const
{
	if ( TFGameRules()->IsDeathmatch() )
		return GetTFWpnData().m_iSlotDM;

	return GetWpnData().iSlot;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetPosition( void ) const
{
	if ( TFGameRules()->IsDeathmatch() )
		return GetTFWpnData().m_iPositionDM;

	return GetWpnData().iPosition;
}
#endif

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
bool CTFWeaponBase::CanHolster( void ) const
{
	CTFPlayer *pOwner = GetTFPlayerOwner();

	// Not while taunting.
	if ( pOwner && pOwner->m_Shared.InCond( TF_COND_TAUNTING ) )
		return false;

	return BaseClass::CanHolster();
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
	float flOriginalSecondaryAttack = m_flNextSecondaryAttack;

	bool bDeploy = BaseClass::Deploy();

	if ( bDeploy )
	{
		CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );
		if ( !pPlayer )
			return false;

		// Overrides the anim length for calculating ready time.
		// Don't override primary attacks that are already further out than this. This prevents
		// people exploiting weapon switches to allow weapons to fire faster.
		float flDeployTime = 0.5f;
		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pPlayer, flDeployTime, mult_deploy_time );
		CALL_ATTRIB_HOOK_FLOAT( flDeployTime, mult_single_wep_deploy_time );

		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pPlayer->GetLastWeapon(), flDeployTime, mult_switch_from_wep_deploy_time );

		if ( pPlayer->m_Shared.InCond( TF_COND_BLASTJUMPING ) )
			CALL_ATTRIB_HOOK_FLOAT( flDeployTime, mult_rocketjump_deploy_time );

		if ( pPlayer->m_Shared.GetNumHealers() == 0 )
			CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pPlayer, flDeployTime, mod_medic_healed_deploy_time );

		m_flNextPrimaryAttack = max( flOriginalPrimaryAttack, gpGlobals->curtime + flDeployTime );
		m_flNextSecondaryAttack = max( flOriginalSecondaryAttack, gpGlobals->curtime + flDeployTime );


		pPlayer->SetNextAttack( m_flNextPrimaryAttack );
	}

	return bDeploy;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::Equip( CBaseCombatCharacter *pOwner )
{
	BaseClass::Equip( pOwner );

	// Add it to attribute providers list.
	ReapplyProvision();

	CTFPlayer *pTFOwner = GetTFPlayerOwner();
	if ( pTFOwner )
	{
		pTFOwner->TeamFortress_SetSpeed();
	}
}

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::UnEquip( CBaseCombatCharacter *pOwner )
{
	if ( pOwner )
	{
		if ( pOwner->GetActiveWeapon() == this )
			Holster();

		pOwner->Weapon_Detach( this );
	}

	UTIL_Remove( this );
}

#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFWeaponBase::IsViewModelFlipped( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();

	if ( pOwner )
	{
		return ( m_bFlipViewModel != pOwner->ShouldFlipViewModel() );
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::ReapplyProvision( void )
{
	int iProvideOnActive = 0;
	CALL_ATTRIB_HOOK_INT( iProvideOnActive, provide_on_active );
	if ( !iProvideOnActive || m_iState == WEAPON_IS_ACTIVE )
	{
		BaseClass::ReapplyProvision();
	}
	else
	{
		// Weapon not active, remove it from providers list.
		GetAttributeContainer()->StopProvidingTo( GetOwner() );
		m_hOldOwner = NULL;
	}
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

		// If set to only provide attributes while active, update the status now.
		if ( iProvideOnActive )
		{
			ReapplyProvision();
		}

		// Weapon might be giving us speed boost when active.
		pOwner->TeamFortress_SetSpeed();
	}

	BaseClass::OnActiveStateChanged( iOldState );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::UpdateOnRemove( void )
{
	//RemoveExtraWearables();

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
void CTFWeaponBase::OnPickedUp( CBaseCombatCharacter *pNewOwner )
{
	BaseClass::OnPickedUp( pNewOwner );
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
void CTFWeaponBase::CalcIsAttackCritical( void )
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

	if ( pPlayer->m_Shared.IsCritBoosted() )
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

	// Random crits are disabled in DM due to balancing reasons
	if ( TFGameRules()->IsDeathmatch() )
		return false;

	// Don't bother checking if random crits are off.
	if ( !tf_weapon_criticals.GetBool() )
		return false;

	if ( !CanFireCriticalShot() )
		return false;

	float flPlayerCritMult = pPlayer->GetCritMult();

	if ( m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_bUseRapidFireCrits )
	{
		if ( m_flCritTime > gpGlobals->curtime )
			return true;
		// only perform one crit check per second for rapid fire weapons
		if ( gpGlobals->curtime < m_flLastCritCheckTime + 1.0f )
			return false;
		m_flLastCritCheckTime = gpGlobals->curtime;

		// get the total crit chance (ratio of total shots fired we want to be crits)
		float flTotalCritChance = TF_DAMAGE_CRIT_CHANCE_RAPID * flPlayerCritMult;
		CALL_ATTRIB_HOOK_FLOAT( flTotalCritChance, mult_crit_chance );

		// If the chance is 0, just bail.
		if ( flTotalCritChance == 0.0f )
			return false;

		flTotalCritChance = clamp( flTotalCritChance, 0.01f, 0.99f );
		// get the fixed amount of time that we start firing crit shots for	
		float flCritDuration = TF_DAMAGE_CRIT_DURATION_RAPID;
		// calculate the amount of time, on average, that we want to NOT fire crit shots for in order to achive the total crit chance we want
		float flNonCritDuration = ( flCritDuration / flTotalCritChance ) - flCritDuration;
		// calculate the chance per second of non-crit fire that we should transition into critting such that on average we achieve the total crit chance we want
		float flStartCritChance = 1 / flNonCritDuration;

#ifdef GAME_DLL
		if ( tf_debug_criticals.GetBool() )
		{
			Msg( "Rolling crit: %.02f%% chance... ", flTotalCritChance * 100.0f );
		}
#endif

		// see if we should start firing crit shots
		bool bSuccess = RandomInt( 0, WEAPON_RANDOM_RANGE - 1 ) <= ( flStartCritChance * WEAPON_RANDOM_RANGE );

		if ( bSuccess )
		{
			m_flCritTime = gpGlobals->curtime + TF_DAMAGE_CRIT_DURATION_RAPID;
		}

#ifdef GAME_DLL
		if ( tf_debug_criticals.GetBool() )
		{
			Msg( "%s\n", bSuccess ? "SUCCESS" : "FAILURE" );
		}
#endif

		return false;
	}
	else
	{
		// single-shot weapon, just use random pct per shot
		float flCritChance = TF_DAMAGE_CRIT_CHANCE * flPlayerCritMult;
		CALL_ATTRIB_HOOK_FLOAT( flCritChance, mult_crit_chance );

		// If the chance is 0, just bail.
		if ( flCritChance == 0.0f )
			return false;

#ifdef GAME_DLL
		if ( tf_debug_criticals.GetBool() )
		{
			Msg( "Rolling crit: %.02f%% chance... ", flCritChance * 100.0f );
		}
#endif

		bool bSuccess = ( RandomInt( 0.0, WEAPON_RANDOM_RANGE - 1 ) < flCritChance * WEAPON_RANDOM_RANGE );

#ifdef GAME_DLL
		if ( tf_debug_criticals.GetBool() )
		{
			Msg( "%s\n", bSuccess ? "SUCCESS" : "FAILURE" );
		}
#endif

		return bSuccess;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetMaxClip1( void ) const
{
	float flMaxClip = (float)CBaseCombatWeapon::GetMaxClip1();
	if ( flMaxClip == WEAPON_NOCLIP )
		return (int)flMaxClip;

	CALL_ATTRIB_HOOK_FLOAT( flMaxClip, mult_clipsize );

	int iMaxClip = (int)( flMaxClip + 0.5f );

	if ( tf2c_weapon_noreload.GetBool() && iMaxClip != 1 )
	{
		return WEAPON_NOCLIP;
	}

	// Round to the nearest integer.
	return iMaxClip;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetDefaultClip1( void ) const
{
	float flDefaultClip = (float)CBaseCombatWeapon::GetDefaultClip1();
	CALL_ATTRIB_HOOK_FLOAT( flDefaultClip, mult_clipsize );
	if ( flDefaultClip == WEAPON_NOCLIP )
		return (int)flDefaultClip;

	int iDefaultClip = (int)( flDefaultClip + 0.5f );

	if ( tf2c_weapon_noreload.GetBool() && iDefaultClip != 1 )
	{
		return WEAPON_NOCLIP;
	}

	// Round to the nearest integer.
	return iDefaultClip;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetMaxAmmo( void )
{
	return GetTFWpnData().m_iMaxAmmo;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFWeaponBase::GetInitialAmmo( void )
{
	int iSpawnAmmo = 0;
	CALL_ATTRIB_HOOK_INT( iSpawnAmmo, mod_spawn_ammo_override );
	if ( iSpawnAmmo )
		return iSpawnAmmo;

	return GetTFWpnData().m_iSpawnAmmo;
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

	CTFPlayer *pOwner = GetTFPlayerOwner();

	// Can't reload while cloaked.
	if ( pOwner->m_Shared.InCond( TF_COND_STEALTHED ) )
		return false;

	// If we're not already reloading, check to see if we have ammo to reload and check to see if we are max ammo.
	if ( m_iReloadMode == TF_RELOAD_START )
	{
		// If I don't have any spare ammo, I can't reload
		if ( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 )
			return false;

		if ( Clip1() >= GetMaxClip1() )
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

#ifdef CLIENT_DLL
	if ( !UsingViewModel() )
#endif
		StopWeaponSound( RELOAD );

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

		if ( SendWeaponAnim( ACT_VM_RELOAD ) )
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

#ifdef CLIENT_DLL
		if ( !UsingViewModel() )
#endif
			WeaponSound( RELOAD );

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
		if ( pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) > 0 && m_iClip1 < GetMaxClip1() && !m_bReloadedThroughAnimEvent )
		{
			m_iClip1++;
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
	if ( ( pEvent->type & AE_TYPE_NEWEVENTSYSTEM ) /*&& (pEvent->type & AE_TYPE_SERVER)*/ )
	{
		if ( pEvent->event == AE_WPN_INCREMENTAMMO )
		{
			if ( pOperator->GetAmmoCount( m_iPrimaryAmmoType ) > 0 && m_iClip1 < GetMaxClip1() && !m_bReloadedThroughAnimEvent )
			{
				m_iClip1++;
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
		int primary = min( iClipSize1 - m_iClip1, pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) );
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
	if ( !( bReloadPrimary || bReloadSecondary ) )
		return false;

	// Play reload
#ifdef CLIENT_DLL
	if ( !UsingViewModel() )
#endif
		WeaponSound( RELOAD );

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

	float flModifiedTime = flReloadTime;
	CALL_ATTRIB_HOOK_FLOAT( flModifiedTime, mult_reload_time );
	CALL_ATTRIB_HOOK_FLOAT( flModifiedTime, mult_reload_time_hidden );
	CALL_ATTRIB_HOOK_FLOAT( flModifiedTime, fast_reload );

	CBaseViewModel *vm = pPlayer->GetViewModel( m_nViewModelIndex );
	if ( vm )
	{
		vm->SetPlaybackRate( flReloadTime / flModifiedTime );
	}

	float flTime = gpGlobals->curtime + flModifiedTime;

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
// Purpose: Returns override from item schema if there is one.
// -----------------------------------------------------------------------------
const char *CTFWeaponBase::GetShootSound( int iIndex ) const
{
	const char *pszSoundName = NULL;

	if ( HasItemDefinition() )
	{
		pszSoundName = m_Item.GetSoundOverride( iIndex, GetTeamNumber() );
	}

	if ( !pszSoundName || pszSoundName[0] == '\0' )
	{
		pszSoundName = BaseClass::GetShootSound( iIndex );
	}

	return pszSoundName;
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

	if ( ( pOwner->m_nButtons & IN_ATTACK2 ) && m_bInReload == false && m_bInAttack2 == false )
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

	CheckEffectBarRegen();

	// Interrupt a reload.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer )
	{
		if ( pPlayer->m_nButtons & IN_ATTACK )
		{
			if ( ( ( ReloadsSingly() && m_iReloadMode != TF_RELOAD_START ) || m_bInReload ) && Clip1() > 0 )
			{
				AbortReload();

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

	CheckEffectBarRegen();

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
void CTFWeaponBase::ItemHolsterFrame( void )
{
	CheckEffectBarRegen();
	BaseClass::ItemHolsterFrame();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::ReloadSinglyPostFrame( void )
{
	if ( m_flTimeWeaponIdle > gpGlobals->curtime )
		return;

	// if the clip is empty and we have ammo remaining, 
	if ( ( ( Clip1() == 0 ) && ( GetOwner()->GetAmmoCount( m_iPrimaryAmmoType ) > 0 ) ) ||
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
		if ( ( ( GetWeaponFlags() & ITEM_FLAG_NOAUTOSWITCHEMPTY ) == false ) && ( g_pGameRules->SwitchToNextBestWeapon( pOwner, this ) ) )
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
			( m_iClip1 == 0 || ( pOwner && pOwner->ShouldAutoReload() && m_iClip1 < GetMaxClip1() && CanAutoReload() ) ) &&
			( GetWeaponFlags() & ITEM_FLAG_NOAUTORELOAD ) == false &&
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

ConVar mp_forceactivityset( "mp_forceactivityset", "-1", FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
int CTFWeaponBase::GetActivityWeaponRole( void )
{
	int iWeaponRole = GetTFWpnData().m_iWeaponType;

	if ( HasItemDefinition() )
	{
		int iSchemaRole = m_Item.GetAnimationSlot();
		if ( iSchemaRole >= 0 )
			iWeaponRole = iSchemaRole;
	}

	if ( mp_forceactivityset.GetInt() >= 0 )
	{
		iWeaponRole = mp_forceactivityset.GetInt();
	}

#ifdef CLIENT_DLL
	// If we're disguised, we show a different weapon from what we're actually carrying.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer && pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) && pPlayer->IsEnemyPlayer() )
	{
		CEconItemView *pItem = pPlayer->m_Shared.GetDisguiseItem();
		if ( pItem->GetItemDefIndex() >= 0 )
		{
			int iSchemaRole = pItem->GetAnimationSlot();
			if ( iSchemaRole >= 0 )
			{
				iWeaponRole = iSchemaRole;
			}
			else
			{
				CTFWeaponInfo *pWeaponInfo = pPlayer->m_Shared.GetDisguiseWeaponInfo();
				if ( pWeaponInfo )
				{
					iWeaponRole = pWeaponInfo->m_iWeaponType;
				}
			}
		}
	}
#endif

	return iWeaponRole;
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
float CTFWeaponBase::GetMuzzleFlashModelScale( void )
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
		if ( GetOwner() && !m_szTracerName[0] )
		{
			const char *pszTeamName = GetTeamParticleName( GetOwner()->GetTeamNumber(), true );
			V_snprintf( m_szTracerName, MAX_TRACER_NAME, "%s_%s", GetTFWpnData().m_szTracerEffect, pszTeamName );
		}

		return m_szTracerName;
	}

	if ( GetWeaponID() == TF_WEAPON_MINIGUN )
		return "BrightTracer";

	return BaseClass::GetTracerType();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::StartEffectBarRegen( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	// Don't recharge unless we actually need recharging.
	if ( gpGlobals->curtime > m_flEffectBarRegenTime ||
		pOwner->GetAmmoCount( m_iPrimaryAmmoType ) + 1 <= pOwner->GetMaxAmmo( m_iPrimaryAmmoType ) )
	{
		m_flEffectBarRegenTime = gpGlobals->curtime + InternalGetEffectBarRechargeTime();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::EffectBarRegenFinished( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

#ifdef GAME_DLL
	pOwner->GiveAmmo( 1, m_iPrimaryAmmoType, true, TF_AMMO_SOURCE_RESUPPLY );
#endif

	// Keep recharging until we're full on ammo.
#ifdef GAME_DLL
	if ( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) < pOwner->GetMaxAmmo( m_iPrimaryAmmoType ) )
#else
	if ( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) + 1 < pOwner->GetMaxAmmo( m_iPrimaryAmmoType ) )
#endif
	{
		StartEffectBarRegen();
	}
	else
	{
		m_flEffectBarRegenTime = 0.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWeaponBase::CheckEffectBarRegen( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	if ( m_flEffectBarRegenTime != 0.0f )
	{
		// Stop recharging if we're restocked on ammo.
		if ( pOwner->GetAmmoCount( m_iPrimaryAmmoType ) == pOwner->GetMaxAmmo( m_iPrimaryAmmoType ) )
		{
			m_flEffectBarRegenTime = 0.0f;
		}
		else if ( gpGlobals->curtime >= m_flEffectBarRegenTime )
		{
			m_flEffectBarRegenTime = 0.0f;
			EffectBarRegenFinished();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFWeaponBase::GetEffectBarProgress( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner && pOwner->GetAmmoCount( m_iPrimaryAmmoType ) < pOwner->GetMaxAmmo( m_iPrimaryAmmoType ) )
	{
		float flTimeLeft = m_flEffectBarRegenTime - gpGlobals->curtime;
		float flRechargeTime = InternalGetEffectBarRechargeTime();
		return clamp( ( ( flRechargeTime - flTimeLeft ) / flRechargeTime ), 0.0f, 1.0f );
	}

	return 1.0f;
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

	m_flEffectBarRegenTime = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
const Vector &CTFWeaponBase::GetBulletSpread( void )
{
	static Vector cone = VECTOR_CONE_15DEGREES;
	return cone;
}

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
void CTFWeaponBase::ApplyOnHitAttributes( CTFPlayer *pVictim, const CTakeDamageInfo &info )
{
	float flStunTime = 0.0f;
	CALL_ATTRIB_HOOK_FLOAT( flStunTime, mult_onhit_enemyspeed_major );
	if ( flStunTime )
	{
		pVictim->m_Shared.AddCond( TF_COND_SLOWED, flStunTime );
	}

	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner || !pOwner->IsAlive() )
		return;

	// Afterburn shouldn't trigger on-hit effects.
	// Disguised spies shouldn't trigger on-hit effects.
	if ( ( info.GetDamageType() & DMG_BURN ) ||
		( pVictim->m_Shared.InCond( TF_COND_DISGUISED ) && pVictim->m_Shared.GetDisguiseTeam() == pOwner->GetTeamNumber() ) )
		return;

	float flAddCharge = 0.0f;
	CALL_ATTRIB_HOOK_FLOAT( flAddCharge, add_onhit_ubercharge );
	if ( flAddCharge )
	{
		CWeaponMedigun *pMedigun = pOwner->GetMedigun();

		if ( pMedigun )
		{
			pMedigun->AddCharge( flAddCharge );
		}
	}

	float flAddHealth = 0.0f;
	CALL_ATTRIB_HOOK_FLOAT( flAddHealth, add_onhit_addhealth );
	if ( flAddHealth )
	{
		int iHealthRestored = pOwner->TakeHealth( flAddHealth, DMG_GENERIC );

		if ( iHealthRestored )
		{
			IGameEvent *event = gameeventmanager->CreateEvent( "player_healonhit" );

			if ( event )
			{
				event->SetInt( "amount", iHealthRestored );
				event->SetInt( "entindex", pOwner->entindex() );

				gameeventmanager->FireEvent( event );
			}
		}
	}
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
	if ( UsingViewModel() )
	{
		pszMuzzleFlashEffect = GetMuzzleFlashEffectName_1st();
	}
	else
	{
		pszMuzzleFlashEffect = GetMuzzleFlashEffectName_3rd();
	}

	// If we have an attachment, then stick a light on it.
	if ( iMuzzleFlashAttachment > 0 && ( pszMuzzleFlashEffect || pszMuzzleFlashModel || pszMuzzleFlashParticleEffect ) )
	{
		pAttachEnt->GetAttachment( iMuzzleFlashAttachment, vecOrigin, angAngles );

		// Muzzleflash light
		if ( tf2c_muzzlelight.GetBool() )
		{
			CLocalPlayerFilter filter;
			TE_DynamicLight( filter, 0.0f, &vecOrigin, 255, 192, 64, 5, 70.0f, 0.05f, 70.0f / 0.05f, LIGHT_INDEX_MUZZLEFLASH );
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

		if ( pszMuzzleFlashModel && tf2c_model_muzzleflash.GetBool() )
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

			m_hMuzzleFlashModel[nIndex]->SetModelScale( GetMuzzleFlashModelScale() );

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
	bool bNotViewModel = ( ( pOwner && !pOwner->IsLocalPlayer() ) || C_BasePlayer::ShouldDrawLocalPlayer() );
	bool bUseInvulnMaterial = ( bNotViewModel && pOwner && pOwner->m_Shared.IsInvulnerable() && !pOwner->m_Shared.InCond( TF_COND_INVULNERABLE_HIDE_UNLESS_DAMAGE ) );
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

//-----------------------------------------------------------------------------
// Purpose:
// ----------------------------------------------------------------------------
bool CTFWeaponBase::ShouldDraw( void )
{
	C_TFPlayer *pOwner = GetTFPlayerOwner();
	if ( pOwner )
	{
		if ( pOwner->m_Shared.IsLoser() )
			return false;
	}

	return BaseClass::ShouldDraw();
}

void CTFWeaponBase::ProcessMuzzleFlashEvent( void )
{
	C_BaseEntity *pAttachEnt;
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );

	if ( pOwner == NULL )
		return;

	bool bDrawMuzzleFlashOnViewModel = !pOwner->ShouldDrawThisPlayer();

	// Don't draw muzzleflashes if the viewmodel is not drawn.
	if ( bDrawMuzzleFlashOnViewModel && ( !g_pClientMode->ShouldDrawViewModel() || !r_drawviewmodel.GetBool() ) )
		return;

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
	C_TFPlayer *pOwner = GetTFPlayerOwner();

	//Our owner is alive
	if ( pOwner && pOwner->IsAlive() == true )
	{
		//And he is NOT taunting
		if ( pOwner->m_Shared.InCond( TF_COND_TAUNTING ) == false &&
			pOwner->m_Shared.IsLoser() == false )
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

acttable_t CTFWeaponBase::s_acttablePrimary[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_PRIMARY, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_PRIMARY, false },
	{ ACT_MP_DEPLOYED, ACT_MP_DEPLOYED_PRIMARY, false },
	{ ACT_MP_CROUCH_DEPLOYED, ACT_MP_CROUCHWALK_DEPLOYED, false },
	{ ACT_MP_RUN, ACT_MP_RUN_PRIMARY, false },
	{ ACT_MP_WALK, ACT_MP_WALK_PRIMARY, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_PRIMARY, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_PRIMARY, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_PRIMARY, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_PRIMARY, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_PRIMARY, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_PRIMARY, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_PRIMARY, false },
	{ ACT_MP_SWIM_DEPLOYED, ACT_MP_SWIM_DEPLOYED_PRIMARY, false },
	//{ ACT_MP_DEPLOYED,		ACT_MP_DEPLOYED_PRIMARY,			false },
	{ ACT_MP_DOUBLEJUMP_CROUCH, ACT_MP_DOUBLEJUMP_CROUCH_PRIMARY, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_PRIMARY, false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED, ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_PRIMARY, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED, ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_PRIMARY, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_PRIMARY, false },

	{ ACT_MP_RELOAD_STAND, ACT_MP_RELOAD_STAND_PRIMARY, false },
	{ ACT_MP_RELOAD_STAND_LOOP, ACT_MP_RELOAD_STAND_PRIMARY_LOOP, false },
	{ ACT_MP_RELOAD_STAND_END, ACT_MP_RELOAD_STAND_PRIMARY_END, false },
	{ ACT_MP_RELOAD_CROUCH, ACT_MP_RELOAD_CROUCH_PRIMARY, false },
	{ ACT_MP_RELOAD_CROUCH_LOOP, ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP, false },
	{ ACT_MP_RELOAD_CROUCH_END, ACT_MP_RELOAD_CROUCH_PRIMARY_END, false },
	{ ACT_MP_RELOAD_SWIM, ACT_MP_RELOAD_SWIM_PRIMARY, false },
	{ ACT_MP_RELOAD_SWIM_LOOP, ACT_MP_RELOAD_SWIM_PRIMARY_LOOP, false },
	{ ACT_MP_RELOAD_SWIM_END, ACT_MP_RELOAD_SWIM_PRIMARY_END, false },
	{ ACT_MP_RELOAD_AIRWALK, ACT_MP_RELOAD_AIRWALK_PRIMARY, false },
	{ ACT_MP_RELOAD_AIRWALK_LOOP, ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP, false },
	{ ACT_MP_RELOAD_AIRWALK_END, ACT_MP_RELOAD_AIRWALK_PRIMARY_END, false },

	{ ACT_MP_GESTURE_FLINCH, ACT_MP_GESTURE_FLINCH_PRIMARY, false },

	{ ACT_MP_GRENADE1_DRAW, ACT_MP_PRIMARY_GRENADE1_DRAW, false },
	{ ACT_MP_GRENADE1_IDLE, ACT_MP_PRIMARY_GRENADE1_IDLE, false },
	{ ACT_MP_GRENADE1_ATTACK, ACT_MP_PRIMARY_GRENADE1_ATTACK, false },
	{ ACT_MP_GRENADE2_DRAW, ACT_MP_PRIMARY_GRENADE2_DRAW, false },
	{ ACT_MP_GRENADE2_IDLE, ACT_MP_PRIMARY_GRENADE2_IDLE, false },
	{ ACT_MP_GRENADE2_ATTACK, ACT_MP_PRIMARY_GRENADE2_ATTACK, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_PRIMARY, false },
};

acttable_t CTFWeaponBase::s_acttableSecondary[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_SECONDARY, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_SECONDARY, false },
	{ ACT_MP_RUN, ACT_MP_RUN_SECONDARY, false },
	{ ACT_MP_WALK, ACT_MP_WALK_SECONDARY, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_SECONDARY, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_SECONDARY, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_SECONDARY, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_SECONDARY, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_SECONDARY, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_SECONDARY, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_SECONDARY, false },
	{ ACT_MP_DOUBLEJUMP_CROUCH, ACT_MP_DOUBLEJUMP_CROUCH_SECONDARY, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_SECONDARY, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_SECONDARY, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_SECONDARY, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_SECONDARY, false },

	{ ACT_MP_RELOAD_STAND, ACT_MP_RELOAD_STAND_SECONDARY, false },
	{ ACT_MP_RELOAD_STAND_LOOP, ACT_MP_RELOAD_STAND_SECONDARY_LOOP, false },
	{ ACT_MP_RELOAD_STAND_END, ACT_MP_RELOAD_STAND_SECONDARY_END, false },
	{ ACT_MP_RELOAD_CROUCH, ACT_MP_RELOAD_CROUCH_SECONDARY, false },
	{ ACT_MP_RELOAD_CROUCH_LOOP, ACT_MP_RELOAD_CROUCH_SECONDARY_LOOP, false },
	{ ACT_MP_RELOAD_CROUCH_END, ACT_MP_RELOAD_CROUCH_SECONDARY_END, false },
	{ ACT_MP_RELOAD_SWIM, ACT_MP_RELOAD_SWIM_SECONDARY, false },
	{ ACT_MP_RELOAD_SWIM_LOOP, ACT_MP_RELOAD_SWIM_SECONDARY_LOOP, false },
	{ ACT_MP_RELOAD_SWIM_END, ACT_MP_RELOAD_SWIM_SECONDARY_END, false },
	{ ACT_MP_RELOAD_AIRWALK, ACT_MP_RELOAD_AIRWALK_SECONDARY, false },
	{ ACT_MP_RELOAD_AIRWALK_LOOP, ACT_MP_RELOAD_AIRWALK_SECONDARY_LOOP, false },
	{ ACT_MP_RELOAD_AIRWALK_END, ACT_MP_RELOAD_AIRWALK_SECONDARY_END, false },

	{ ACT_MP_GESTURE_FLINCH, ACT_MP_GESTURE_FLINCH_SECONDARY, false },

	{ ACT_MP_GRENADE1_DRAW, ACT_MP_SECONDARY_GRENADE1_DRAW, false },
	{ ACT_MP_GRENADE1_IDLE, ACT_MP_SECONDARY_GRENADE1_IDLE, false },
	{ ACT_MP_GRENADE1_ATTACK, ACT_MP_SECONDARY_GRENADE1_ATTACK, false },
	{ ACT_MP_GRENADE2_DRAW, ACT_MP_SECONDARY_GRENADE2_DRAW, false },
	{ ACT_MP_GRENADE2_IDLE, ACT_MP_SECONDARY_GRENADE2_IDLE, false },
	{ ACT_MP_GRENADE2_ATTACK, ACT_MP_SECONDARY_GRENADE2_ATTACK, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_SECONDARY, false },
};

acttable_t CTFWeaponBase::s_acttableMelee[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_MELEE, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_MELEE, false },
	{ ACT_MP_RUN, ACT_MP_RUN_MELEE, false },
	{ ACT_MP_WALK, ACT_MP_WALK_MELEE, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_MELEE, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_MELEE, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_MELEE, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_MELEE, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_MELEE, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_MELEE, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_MELEE, false },
	{ ACT_MP_DOUBLEJUMP_CROUCH, ACT_MP_DOUBLEJUMP_CROUCH_MELEE, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_MELEE, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_MELEE, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_MELEE, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_MELEE, false },

	{ ACT_MP_ATTACK_STAND_SECONDARYFIRE, ACT_MP_ATTACK_STAND_MELEE_SECONDARY, false },
	{ ACT_MP_ATTACK_CROUCH_SECONDARYFIRE, ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY, false },
	{ ACT_MP_ATTACK_SWIM_SECONDARYFIRE, ACT_MP_ATTACK_SWIM_MELEE, false },
	{ ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE, ACT_MP_ATTACK_AIRWALK_MELEE, false },

	{ ACT_MP_GESTURE_FLINCH, ACT_MP_GESTURE_FLINCH_MELEE, false },

	{ ACT_MP_GRENADE1_DRAW, ACT_MP_MELEE_GRENADE1_DRAW, false },
	{ ACT_MP_GRENADE1_IDLE, ACT_MP_MELEE_GRENADE1_IDLE, false },
	{ ACT_MP_GRENADE1_ATTACK, ACT_MP_MELEE_GRENADE1_ATTACK, false },
	{ ACT_MP_GRENADE2_DRAW, ACT_MP_MELEE_GRENADE2_DRAW, false },
	{ ACT_MP_GRENADE2_IDLE, ACT_MP_MELEE_GRENADE2_IDLE, false },
	{ ACT_MP_GRENADE2_ATTACK, ACT_MP_MELEE_GRENADE2_ATTACK, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_MELEE, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_MELEE, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_MELEE, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_MELEE, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_MELEE, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_MELEE, false },
};

acttable_t CTFWeaponBase::s_acttableBuilding[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_BUILDING, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_BUILDING, false },
	{ ACT_MP_RUN, ACT_MP_RUN_BUILDING, false },
	{ ACT_MP_WALK, ACT_MP_WALK_BUILDING, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_BUILDING, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_BUILDING, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_BUILDING, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_BUILDING, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_BUILDING, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_BUILDING, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_BUILDING, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_BUILDING, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_BUILDING, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_BUILDING, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_BUILDING, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE_BUILDING, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_STAND_GRENADE_BUILDING, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_STAND_GRENADE_BUILDING, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_STAND_GRENADE_BUILDING, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_BUILDING, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_BUILDING, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_BUILDING, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_BUILDING, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_BUILDING, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_BUILDING, false },
};


acttable_t CTFWeaponBase::s_acttablePDA[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_PDA, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_PDA, false },
	{ ACT_MP_RUN, ACT_MP_RUN_PDA, false },
	{ ACT_MP_WALK, ACT_MP_WALK_PDA, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_PDA, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_PDA, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_PDA, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_PDA, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_PDA, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_PDA, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_PDA, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_PDA, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_PDA, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_PDA, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_PDA, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_PDA, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_PDA, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_PDA, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_PDA, false },
};

acttable_t CTFWeaponBase::s_acttableItem1[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_ITEM1, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_ITEM1, false },
	{ ACT_MP_RUN, ACT_MP_RUN_ITEM1, false },
	{ ACT_MP_WALK, ACT_MP_WALK_ITEM1, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_ITEM1, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_ITEM1, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_ITEM1, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_ITEM1, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_ITEM1, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_ITEM1, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_ITEM1, false },
	{ ACT_MP_DOUBLEJUMP_CROUCH, ACT_MP_DOUBLEJUMP_CROUCH_ITEM1, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_ITEM1, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_ITEM1, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_ITEM1, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_ITEM1, false },
	{ ACT_MP_ATTACK_STAND_SECONDARYFIRE, ACT_MP_ATTACK_STAND_ITEM1_SECONDARY, false },
	{ ACT_MP_ATTACK_CROUCH_SECONDARYFIRE, ACT_MP_ATTACK_CROUCH_ITEM1_SECONDARY, false },
	{ ACT_MP_ATTACK_SWIM_SECONDARYFIRE, ACT_MP_ATTACK_SWIM_ITEM1, false },
	{ ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE, ACT_MP_ATTACK_AIRWALK_ITEM1, false },

	{ ACT_MP_DEPLOYED, ACT_MP_DEPLOYED_ITEM1, false },
	{ ACT_MP_DEPLOYED_IDLE, ACT_MP_DEPLOYED_IDLE_ITEM1, false },
	{ ACT_MP_CROUCH_DEPLOYED, ACT_MP_CROUCHWALK_DEPLOYED_ITEM1, false },
	{ ACT_MP_CROUCH_DEPLOYED_IDLE, ACT_MP_CROUCH_DEPLOYED_IDLE_ITEM1, false },

	{ ACT_MP_GESTURE_FLINCH, ACT_MP_GESTURE_FLINCH_ITEM1, false },

	{ ACT_MP_GRENADE1_DRAW, ACT_MP_ITEM1_GRENADE1_DRAW, false },
	{ ACT_MP_GRENADE1_IDLE, ACT_MP_ITEM1_GRENADE1_IDLE, false },
	{ ACT_MP_GRENADE1_ATTACK, ACT_MP_ITEM1_GRENADE1_ATTACK, false },
	{ ACT_MP_GRENADE2_DRAW, ACT_MP_ITEM1_GRENADE2_DRAW, false },
	{ ACT_MP_GRENADE2_IDLE, ACT_MP_ITEM1_GRENADE2_IDLE, false },
	{ ACT_MP_GRENADE2_ATTACK, ACT_MP_ITEM1_GRENADE2_ATTACK, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_ITEM1, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_ITEM1, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_ITEM1, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_ITEM1, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_ITEM1, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_ITEM1, false },
};

acttable_t CTFWeaponBase::s_acttableItem2[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_ITEM2, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_ITEM2, false },
	{ ACT_MP_RUN, ACT_MP_RUN_ITEM2, false },
	{ ACT_MP_WALK, ACT_MP_WALK_ITEM2, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_ITEM2, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_ITEM2, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_ITEM2, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_ITEM2, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_ITEM2, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_ITEM2, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_ITEM2, false },
	{ ACT_MP_DOUBLEJUMP_CROUCH, ACT_MP_DOUBLEJUMP_CROUCH_ITEM2, false },

	{ ACT_MP_RELOAD_STAND, ACT_MP_RELOAD_STAND_ITEM2, false },
	{ ACT_MP_RELOAD_CROUCH, ACT_MP_RELOAD_CROUCH_ITEM2, false },
	{ ACT_MP_RELOAD_SWIM, ACT_MP_RELOAD_SWIM_ITEM2, false },
	{ ACT_MP_RELOAD_AIRWALK, ACT_MP_RELOAD_AIRWALK_ITEM2, false },
	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_ITEM2, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_ITEM2, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_ITEM2, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_ITEM2, false },

	{ ACT_MP_DEPLOYED, ACT_MP_DEPLOYED_ITEM2, false },
	{ ACT_MP_DEPLOYED_IDLE, ACT_MP_DEPLOYED_IDLE_ITEM2, false },
	{ ACT_MP_CROUCH_DEPLOYED, ACT_MP_CROUCHWALK_DEPLOYED_ITEM2, false },
	{ ACT_MP_CROUCH_DEPLOYED_IDLE, ACT_MP_CROUCH_DEPLOYED_IDLE_ITEM2, false },

	{ ACT_MP_ATTACK_STAND_SECONDARYFIRE, ACT_MP_ATTACK_STAND_ITEM2_SECONDARY, false },
	{ ACT_MP_ATTACK_CROUCH_SECONDARYFIRE, ACT_MP_ATTACK_CROUCH_ITEM2_SECONDARY, false },
	{ ACT_MP_ATTACK_SWIM_SECONDARYFIRE, ACT_MP_ATTACK_SWIM_ITEM2, false },
	{ ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE, ACT_MP_ATTACK_AIRWALK_ITEM2, false },

	{ ACT_MP_GESTURE_FLINCH, ACT_MP_GESTURE_FLINCH_ITEM2, false },

	{ ACT_MP_GRENADE1_DRAW, ACT_MP_ITEM2_GRENADE1_DRAW, false },
	{ ACT_MP_GRENADE1_IDLE, ACT_MP_ITEM2_GRENADE1_IDLE, false },
	{ ACT_MP_GRENADE1_ATTACK, ACT_MP_ITEM2_GRENADE1_ATTACK, false },
	{ ACT_MP_GRENADE2_DRAW, ACT_MP_ITEM2_GRENADE2_DRAW, false },
	{ ACT_MP_GRENADE2_IDLE, ACT_MP_ITEM2_GRENADE2_IDLE, false },
	{ ACT_MP_GRENADE2_ATTACK, ACT_MP_ITEM2_GRENADE2_ATTACK, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_ITEM2, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_ITEM2, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_ITEM2, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_ITEM2, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_ITEM2, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_ITEM2, false },


	{ ACT_MP_RELOAD_STAND_LOOP, ACT_MP_RELOAD_STAND_ITEM2_LOOP, false },
	{ ACT_MP_RELOAD_STAND_END, ACT_MP_RELOAD_STAND_ITEM2_END, false },
	{ ACT_MP_RELOAD_CROUCH_LOOP, ACT_MP_RELOAD_CROUCH_ITEM2_LOOP, false },
	{ ACT_MP_RELOAD_CROUCH_END, ACT_MP_RELOAD_CROUCH_ITEM2_END, false },
	{ ACT_MP_RELOAD_SWIM_LOOP, ACT_MP_RELOAD_SWIM_ITEM2_LOOP, false },
	{ ACT_MP_RELOAD_SWIM_END, ACT_MP_RELOAD_SWIM_ITEM2_END, false },
	{ ACT_MP_RELOAD_AIRWALK_LOOP, ACT_MP_RELOAD_AIRWALK_ITEM2_LOOP, false },
	{ ACT_MP_RELOAD_AIRWALK_END, ACT_MP_RELOAD_AIRWALK_ITEM2_END, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE_DEPLOYED, ACT_MP_ATTACK_STAND_PRIMARY_DEPLOYED_ITEM2, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE_DEPLOYED, ACT_MP_ATTACK_CROUCH_PRIMARY_DEPLOYED_ITEM2, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE_ITEM2, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_CROUCH_GRENADE_ITEM2, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_SWIM_GRENADE_ITEM2, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_AIRWALK_GRENADE_ITEM2, false },

};

acttable_t CTFWeaponBase::s_acttableMeleeAllClass[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_MELEE_ALLCLASS, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_MELEE_ALLCLASS, false },
	{ ACT_MP_RUN, ACT_MP_RUN_MELEE_ALLCLASS, false },
	{ ACT_MP_WALK, ACT_MP_WALK_MELEE_ALLCLASS, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_MELEE_ALLCLASS, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_MELEE_ALLCLASS, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_MELEE_ALLCLASS, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_MELEE_ALLCLASS, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_MELEE_ALLCLASS, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_MELEE_ALLCLASS, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_MELEE_ALLCLASS, false },
	{ ACT_MP_DOUBLEJUMP_CROUCH, ACT_MP_DOUBLEJUMP_CROUCH_MELEE, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_MELEE_ALLCLASS, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_MELEE_ALLCLASS, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_MELEE_ALLCLASS, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_MELEE_ALLCLASS, false },

	{ ACT_MP_ATTACK_STAND_SECONDARYFIRE, ACT_MP_ATTACK_STAND_MELEE_SECONDARY_ALLCLASS, false },
	{ ACT_MP_ATTACK_CROUCH_SECONDARYFIRE, ACT_MP_ATTACK_CROUCH_MELEE_SECONDARY_ALLCLASS, false },
	{ ACT_MP_ATTACK_SWIM_SECONDARYFIRE, ACT_MP_ATTACK_SWIM_MELEE_ALLCLASS, false },
	{ ACT_MP_ATTACK_AIRWALK_SECONDARYFIRE, ACT_MP_ATTACK_AIRWALK_MELEE_ALLCLASS, false },

	{ ACT_MP_GESTURE_FLINCH, ACT_MP_GESTURE_FLINCH_MELEE, false },

	{ ACT_MP_GRENADE1_DRAW, ACT_MP_MELEE_GRENADE1_DRAW, false },
	{ ACT_MP_GRENADE1_IDLE, ACT_MP_MELEE_GRENADE1_IDLE, false },
	{ ACT_MP_GRENADE1_ATTACK, ACT_MP_MELEE_GRENADE1_ATTACK, false },
	{ ACT_MP_GRENADE2_DRAW, ACT_MP_MELEE_GRENADE2_DRAW, false },
	{ ACT_MP_GRENADE2_IDLE, ACT_MP_MELEE_GRENADE2_IDLE, false },
	{ ACT_MP_GRENADE2_ATTACK, ACT_MP_MELEE_GRENADE2_ATTACK, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_MELEE, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_MELEE, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_MELEE, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_MELEE, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_MELEE, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_MELEE, false },
};

acttable_t CTFWeaponBase::s_acttableSecondary2[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_SECONDARY2, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_SECONDARY2, false },
	{ ACT_MP_RUN, ACT_MP_RUN_SECONDARY2, false },
	{ ACT_MP_WALK, ACT_MP_WALK_SECONDARY2, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_SECONDARY2, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_SECONDARY2, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_SECONDARY2, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_SECONDARY2, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_SECONDARY2, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_SECONDARY2, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_SECONDARY2, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_SECONDARY2, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_SECONDARY2, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_SECONDARY2, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_SECONDARY2, false },

	{ ACT_MP_RELOAD_STAND, ACT_MP_RELOAD_STAND_SECONDARY2, false },
	{ ACT_MP_RELOAD_STAND_LOOP, ACT_MP_RELOAD_STAND_SECONDARY2_LOOP, false },
	{ ACT_MP_RELOAD_STAND_END, ACT_MP_RELOAD_STAND_SECONDARY2_END, false },
	{ ACT_MP_RELOAD_CROUCH, ACT_MP_RELOAD_CROUCH_SECONDARY2, false },
	{ ACT_MP_RELOAD_CROUCH_LOOP, ACT_MP_RELOAD_CROUCH_SECONDARY2_LOOP, false },
	{ ACT_MP_RELOAD_CROUCH_END, ACT_MP_RELOAD_CROUCH_SECONDARY2_END, false },
	{ ACT_MP_RELOAD_SWIM, ACT_MP_RELOAD_SWIM_SECONDARY2, false },
	{ ACT_MP_RELOAD_SWIM_LOOP, ACT_MP_RELOAD_SWIM_SECONDARY2_LOOP, false },
	{ ACT_MP_RELOAD_SWIM_END, ACT_MP_RELOAD_SWIM_SECONDARY2_END, false },
	{ ACT_MP_RELOAD_AIRWALK, ACT_MP_RELOAD_AIRWALK_SECONDARY2, false },
	{ ACT_MP_RELOAD_AIRWALK_LOOP, ACT_MP_RELOAD_AIRWALK_SECONDARY2_LOOP, false },
	{ ACT_MP_RELOAD_AIRWALK_END, ACT_MP_RELOAD_AIRWALK_SECONDARY2_END, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_SECONDARY, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_SECONDARY, false },

};

acttable_t CTFWeaponBase::s_acttablePrimary2[] =
{
	{ ACT_MP_STAND_IDLE, ACT_MP_STAND_PRIMARY, false },
	{ ACT_MP_CROUCH_IDLE, ACT_MP_CROUCH_PRIMARY, false },
	{ ACT_MP_DEPLOYED, ACT_MP_DEPLOYED_PRIMARY, false },
	{ ACT_MP_CROUCH_DEPLOYED, ACT_MP_CROUCHWALK_DEPLOYED, false },
	{ ACT_MP_CROUCH_DEPLOYED_IDLE, ACT_MP_CROUCH_DEPLOYED_IDLE, false },
	{ ACT_MP_RUN, ACT_MP_RUN_PRIMARY, false },
	{ ACT_MP_WALK, ACT_MP_WALK_PRIMARY, false },
	{ ACT_MP_AIRWALK, ACT_MP_AIRWALK_PRIMARY, false },
	{ ACT_MP_CROUCHWALK, ACT_MP_CROUCHWALK_PRIMARY, false },
	{ ACT_MP_JUMP, ACT_MP_JUMP_PRIMARY, false },
	{ ACT_MP_JUMP_START, ACT_MP_JUMP_START_PRIMARY, false },
	{ ACT_MP_JUMP_FLOAT, ACT_MP_JUMP_FLOAT_PRIMARY, false },
	{ ACT_MP_JUMP_LAND, ACT_MP_JUMP_LAND_PRIMARY, false },
	{ ACT_MP_SWIM, ACT_MP_SWIM_PRIMARY, false },
	{ ACT_MP_SWIM_DEPLOYED, ACT_MP_SWIM_DEPLOYED_PRIMARY, false },
	{ ACT_MP_DOUBLEJUMP_CROUCH, ACT_MP_DOUBLEJUMP_CROUCH_PRIMARY, false },
	{ ACT_MP_ATTACK_STAND_PRIMARY_SUPER, ACT_MP_ATTACK_STAND_PRIMARY_SUPER, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARY_SUPER, ACT_MP_ATTACK_CROUCH_PRIMARY_SUPER, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARY_SUPER, ACT_MP_ATTACK_SWIM_PRIMARY_SUPER, false },

	{ ACT_MP_ATTACK_STAND_PRIMARYFIRE, ACT_MP_ATTACK_STAND_PRIMARY_ALT, false },
	{ ACT_MP_ATTACK_CROUCH_PRIMARYFIRE, ACT_MP_ATTACK_CROUCH_PRIMARY_ALT, false },
	{ ACT_MP_ATTACK_SWIM_PRIMARYFIRE, ACT_MP_ATTACK_SWIM_PRIMARY_ALT, false },
	{ ACT_MP_ATTACK_AIRWALK_PRIMARYFIRE, ACT_MP_ATTACK_AIRWALK_PRIMARY, false },

	{ ACT_MP_RELOAD_STAND, ACT_MP_RELOAD_STAND_PRIMARY_ALT, false },
	{ ACT_MP_RELOAD_STAND_LOOP, ACT_MP_RELOAD_STAND_PRIMARY_LOOP_ALT, false },
	{ ACT_MP_RELOAD_STAND_END, ACT_MP_RELOAD_STAND_PRIMARY_END_ALT, false },
	{ ACT_MP_RELOAD_CROUCH, ACT_MP_RELOAD_CROUCH_PRIMARY_ALT, false },
	{ ACT_MP_RELOAD_CROUCH_LOOP, ACT_MP_RELOAD_CROUCH_PRIMARY_LOOP_ALT, false },
	{ ACT_MP_RELOAD_CROUCH_END, ACT_MP_RELOAD_CROUCH_PRIMARY_END_ALT, false },
	{ ACT_MP_RELOAD_SWIM, ACT_MP_RELOAD_SWIM_PRIMARY_ALT, false },
	{ ACT_MP_RELOAD_SWIM_LOOP, ACT_MP_RELOAD_SWIM_PRIMARY_LOOP, false },
	{ ACT_MP_RELOAD_SWIM_END, ACT_MP_RELOAD_SWIM_PRIMARY_END, false },
	{ ACT_MP_RELOAD_AIRWALK, ACT_MP_RELOAD_AIRWALK_PRIMARY_ALT, false },
	{ ACT_MP_RELOAD_AIRWALK_LOOP, ACT_MP_RELOAD_AIRWALK_PRIMARY_LOOP_ALT, false },
	{ ACT_MP_RELOAD_AIRWALK_END, ACT_MP_RELOAD_AIRWALK_PRIMARY_END_ALT, false },

	{ ACT_MP_ATTACK_STAND_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_CROUCH_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_SWIM_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },
	{ ACT_MP_ATTACK_AIRWALK_GRENADE, ACT_MP_ATTACK_STAND_GRENADE, false },

	{ ACT_MP_GESTURE_VC_HANDMOUTH, ACT_MP_GESTURE_VC_HANDMOUTH_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_FINGERPOINT, ACT_MP_GESTURE_VC_FINGERPOINT_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_FISTPUMP, ACT_MP_GESTURE_VC_FISTPUMP_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_THUMBSUP, ACT_MP_GESTURE_VC_THUMBSUP_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_NODYES, ACT_MP_GESTURE_VC_NODYES_PRIMARY, false },
	{ ACT_MP_GESTURE_VC_NODNO, ACT_MP_GESTURE_VC_NODNO_PRIMARY, false },
};

viewmodel_acttable_t CTFWeaponBase::s_viewmodelacttable[] =
{
	{ ACT_VM_DRAW, ACT_PRIMARY_VM_DRAW, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_HOLSTER, ACT_PRIMARY_VM_HOLSTER, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_IDLE, ACT_PRIMARY_VM_IDLE, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_PULLBACK, ACT_PRIMARY_VM_PULLBACK, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_PRIMARYATTACK, ACT_PRIMARY_VM_PRIMARYATTACK, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_SECONDARYATTACK, ACT_PRIMARY_VM_SECONDARYATTACK, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_RELOAD, ACT_PRIMARY_VM_RELOAD, TF_WPN_TYPE_PRIMARY },
	{ ACT_RELOAD_START, ACT_PRIMARY_RELOAD_START, TF_WPN_TYPE_PRIMARY },
	{ ACT_RELOAD_FINISH, ACT_PRIMARY_RELOAD_FINISH, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_DRYFIRE, ACT_PRIMARY_VM_DRYFIRE, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_PRIMARY_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_IDLE_LOWERED, ACT_PRIMARY_VM_IDLE_LOWERED, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_PRIMARY_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_PRIMARY },
	{ ACT_MP_ATTACK_STAND_PREFIRE, ACT_PRIMARY_ATTACK_STAND_PREFIRE, TF_WPN_TYPE_PRIMARY },
	{ ACT_MP_ATTACK_STAND_POSTFIRE, ACT_PRIMARY_ATTACK_STAND_POSTFIRE, TF_WPN_TYPE_PRIMARY },
	{ ACT_MP_ATTACK_STAND_STARTFIRE, ACT_PRIMARY_ATTACK_STAND_STARTFIRE, TF_WPN_TYPE_PRIMARY },
	{ ACT_MP_ATTACK_CROUCH_PREFIRE, ACT_PRIMARY_ATTACK_CROUCH_PREFIRE, TF_WPN_TYPE_PRIMARY },
	{ ACT_MP_ATTACK_CROUCH_POSTFIRE, ACT_PRIMARY_ATTACK_CROUCH_POSTFIRE, TF_WPN_TYPE_PRIMARY },
	{ ACT_MP_ATTACK_SWIM_PREFIRE, ACT_PRIMARY_ATTACK_SWIM_PREFIRE, TF_WPN_TYPE_PRIMARY },
	{ ACT_MP_ATTACK_SWIM_POSTFIRE, ACT_PRIMARY_ATTACK_SWIM_POSTFIRE, TF_WPN_TYPE_PRIMARY },
	{ ACT_VM_DRAW, ACT_SECONDARY_VM_DRAW, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_HOLSTER, ACT_SECONDARY_VM_HOLSTER, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_IDLE, ACT_SECONDARY_VM_IDLE, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_PULLBACK, ACT_SECONDARY_VM_PULLBACK, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_PRIMARYATTACK, ACT_SECONDARY_VM_PRIMARYATTACK, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_SECONDARYATTACK, ACT_SECONDARY_VM_SECONDARYATTACK, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_RELOAD, ACT_SECONDARY_VM_RELOAD, TF_WPN_TYPE_SECONDARY },
	{ ACT_RELOAD_START, ACT_SECONDARY_RELOAD_START, TF_WPN_TYPE_SECONDARY },
	{ ACT_RELOAD_FINISH, ACT_SECONDARY_RELOAD_FINISH, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_DRYFIRE, ACT_SECONDARY_VM_DRYFIRE, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_SECONDARY_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_IDLE_LOWERED, ACT_SECONDARY_VM_IDLE_LOWERED, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_SECONDARY_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_SECONDARY },
	{ ACT_MP_ATTACK_STAND_PREFIRE, ACT_SECONDARY_ATTACK_STAND_PREFIRE, TF_WPN_TYPE_SECONDARY },
	{ ACT_MP_ATTACK_STAND_POSTFIRE, ACT_SECONDARY_ATTACK_STAND_POSTFIRE, TF_WPN_TYPE_SECONDARY },
	{ ACT_MP_ATTACK_STAND_STARTFIRE, ACT_SECONDARY_ATTACK_STAND_STARTFIRE, TF_WPN_TYPE_SECONDARY },
	{ ACT_MP_ATTACK_CROUCH_PREFIRE, ACT_SECONDARY_ATTACK_CROUCH_PREFIRE, TF_WPN_TYPE_SECONDARY },
	{ ACT_MP_ATTACK_CROUCH_POSTFIRE, ACT_SECONDARY_ATTACK_CROUCH_POSTFIRE, TF_WPN_TYPE_SECONDARY },
	{ ACT_MP_ATTACK_SWIM_PREFIRE, ACT_SECONDARY_ATTACK_SWIM_PREFIRE, TF_WPN_TYPE_SECONDARY },
	{ ACT_MP_ATTACK_SWIM_POSTFIRE, ACT_SECONDARY_ATTACK_SWIM_POSTFIRE, TF_WPN_TYPE_SECONDARY },
	{ ACT_VM_DRAW, ACT_MELEE_VM_DRAW, TF_WPN_TYPE_MELEE },
	{ ACT_VM_HOLSTER, ACT_MELEE_VM_HOLSTER, TF_WPN_TYPE_MELEE },
	{ ACT_VM_IDLE, ACT_MELEE_VM_IDLE, TF_WPN_TYPE_MELEE },
	{ ACT_VM_PULLBACK, ACT_MELEE_VM_PULLBACK, TF_WPN_TYPE_MELEE },
	{ ACT_VM_PRIMARYATTACK, ACT_MELEE_VM_PRIMARYATTACK, TF_WPN_TYPE_MELEE },
	{ ACT_VM_SECONDARYATTACK, ACT_MELEE_VM_SECONDARYATTACK, TF_WPN_TYPE_MELEE },
	{ ACT_VM_RELOAD, ACT_MELEE_VM_RELOAD, TF_WPN_TYPE_MELEE },
	{ ACT_VM_DRYFIRE, ACT_MELEE_VM_DRYFIRE, TF_WPN_TYPE_MELEE },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_MELEE_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_MELEE },
	{ ACT_VM_IDLE_LOWERED, ACT_MELEE_VM_IDLE_LOWERED, TF_WPN_TYPE_MELEE },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_MELEE_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_MELEE },
	{ ACT_VM_HITCENTER, ACT_MELEE_VM_HITCENTER, TF_WPN_TYPE_MELEE },
	{ ACT_VM_SWINGHARD, ACT_MELEE_VM_SWINGHARD, TF_WPN_TYPE_MELEE },
	{ ACT_MP_ATTACK_STAND_PREFIRE, ACT_MELEE_ATTACK_STAND_PREFIRE, TF_WPN_TYPE_MELEE },
	{ ACT_MP_ATTACK_STAND_POSTFIRE, ACT_MELEE_ATTACK_STAND_POSTFIRE, TF_WPN_TYPE_MELEE },
	{ ACT_MP_ATTACK_STAND_STARTFIRE, ACT_MELEE_ATTACK_STAND_STARTFIRE, TF_WPN_TYPE_MELEE },
	{ ACT_MP_ATTACK_CROUCH_PREFIRE, ACT_MELEE_ATTACK_CROUCH_PREFIRE, TF_WPN_TYPE_MELEE },
	{ ACT_MP_ATTACK_CROUCH_POSTFIRE, ACT_MELEE_ATTACK_CROUCH_POSTFIRE, TF_WPN_TYPE_MELEE },
	{ ACT_MP_ATTACK_SWIM_PREFIRE, ACT_MELEE_ATTACK_SWIM_PREFIRE, TF_WPN_TYPE_MELEE },
	{ ACT_MP_ATTACK_SWIM_POSTFIRE, ACT_MELEE_ATTACK_SWIM_POSTFIRE, TF_WPN_TYPE_MELEE },
	{ ACT_VM_DRAW_SPECIAL, ACT_VM_DRAW_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_HOLSTER_SPECIAL, ACT_VM_HOLSTER_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_IDLE_SPECIAL, ACT_VM_IDLE_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_PULLBACK_SPECIAL, ACT_VM_PULLBACK_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_PRIMARYATTACK_SPECIAL, ACT_VM_PRIMARYATTACK_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_SECONDARYATTACK_SPECIAL, ACT_VM_SECONDARYATTACK_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_HITCENTER_SPECIAL, ACT_VM_HITCENTER_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_SWINGHARD_SPECIAL, ACT_VM_SWINGHARD_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_IDLE_TO_LOWERED_SPECIAL, ACT_VM_IDLE_TO_LOWERED_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_IDLE_LOWERED_SPECIAL, ACT_VM_IDLE_LOWERED_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_VM_LOWERED_TO_IDLE_SPECIAL, ACT_VM_LOWERED_TO_IDLE_SPECIAL, TF_WPN_TYPE_MELEE },
	{ ACT_BACKSTAB_VM_DOWN, ACT_BACKSTAB_VM_DOWN, TF_WPN_TYPE_MELEE },
	{ ACT_BACKSTAB_VM_UP, ACT_BACKSTAB_VM_UP, TF_WPN_TYPE_MELEE },
	{ ACT_BACKSTAB_VM_IDLE, ACT_BACKSTAB_VM_IDLE, TF_WPN_TYPE_MELEE },
	{ ACT_VM_DRAW, ACT_PDA_VM_DRAW, TF_WPN_TYPE_PDA },
	{ ACT_VM_HOLSTER, ACT_PDA_VM_HOLSTER, TF_WPN_TYPE_PDA },
	{ ACT_VM_IDLE, ACT_PDA_VM_IDLE, TF_WPN_TYPE_PDA },
	{ ACT_VM_PULLBACK, ACT_PDA_VM_PULLBACK, TF_WPN_TYPE_PDA },
	{ ACT_VM_PRIMARYATTACK, ACT_PDA_VM_PRIMARYATTACK, TF_WPN_TYPE_PDA },
	{ ACT_VM_SECONDARYATTACK, ACT_PDA_VM_SECONDARYATTACK, TF_WPN_TYPE_PDA },
	{ ACT_VM_RELOAD, ACT_PDA_VM_RELOAD, TF_WPN_TYPE_PDA },
	{ ACT_VM_DRYFIRE, ACT_PDA_VM_DRYFIRE, TF_WPN_TYPE_PDA },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_PDA_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_PDA },
	{ ACT_VM_IDLE_LOWERED, ACT_PDA_VM_IDLE_LOWERED, TF_WPN_TYPE_PDA },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_PDA_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_PDA },
	{ ACT_VM_DRAW, ACT_ITEM1_VM_DRAW, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_HOLSTER, ACT_ITEM1_VM_HOLSTER, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_IDLE, ACT_ITEM1_VM_IDLE, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_PULLBACK, ACT_ITEM1_VM_PULLBACK, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_PRIMARYATTACK, ACT_ITEM1_VM_PRIMARYATTACK, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_SECONDARYATTACK, ACT_ITEM1_VM_SECONDARYATTACK, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_RELOAD, ACT_ITEM1_VM_RELOAD, TF_WPN_TYPE_ITEM1 },
	{ ACT_RELOAD_START, ACT_ITEM1_RELOAD_START, TF_WPN_TYPE_ITEM1 },
	{ ACT_RELOAD_FINISH, ACT_ITEM1_RELOAD_FINISH, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_DRYFIRE, ACT_ITEM1_VM_DRYFIRE, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_ITEM1_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_IDLE_LOWERED, ACT_ITEM1_VM_IDLE_LOWERED, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_ITEM1_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_ITEM1 },
	{ ACT_MP_ATTACK_STAND_PREFIRE, ACT_ITEM1_ATTACK_STAND_PREFIRE, TF_WPN_TYPE_ITEM1 },
	{ ACT_MP_ATTACK_STAND_POSTFIRE, ACT_ITEM1_ATTACK_STAND_POSTFIRE, TF_WPN_TYPE_ITEM1 },
	{ ACT_MP_ATTACK_STAND_STARTFIRE, ACT_ITEM1_ATTACK_STAND_STARTFIRE, TF_WPN_TYPE_ITEM1 },
	{ ACT_MP_ATTACK_CROUCH_PREFIRE, ACT_ITEM1_ATTACK_CROUCH_PREFIRE, TF_WPN_TYPE_ITEM1 },
	{ ACT_MP_ATTACK_CROUCH_POSTFIRE, ACT_ITEM1_ATTACK_CROUCH_POSTFIRE, TF_WPN_TYPE_ITEM1 },
	{ ACT_MP_ATTACK_SWIM_PREFIRE, ACT_ITEM1_ATTACK_SWIM_PREFIRE, TF_WPN_TYPE_ITEM1 },
	{ ACT_MP_ATTACK_SWIM_POSTFIRE, ACT_ITEM1_ATTACK_SWIM_POSTFIRE, TF_WPN_TYPE_ITEM1 },
	{ ACT_VM_DRAW, ACT_ITEM2_VM_DRAW, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_HOLSTER, ACT_ITEM2_VM_HOLSTER, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_IDLE, ACT_ITEM2_VM_IDLE, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_PULLBACK, ACT_ITEM2_VM_PULLBACK, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_PRIMARYATTACK, ACT_ITEM2_VM_PRIMARYATTACK, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_SECONDARYATTACK, ACT_ITEM2_VM_SECONDARYATTACK, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_RELOAD, ACT_ITEM2_VM_RELOAD, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_DRYFIRE, ACT_ITEM2_VM_DRYFIRE, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_ITEM2_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_IDLE_LOWERED, ACT_ITEM2_VM_IDLE_LOWERED, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_ITEM2_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_ITEM2 },
	{ ACT_MP_ATTACK_STAND_PREFIRE, ACT_ITEM2_ATTACK_STAND_PREFIRE, TF_WPN_TYPE_ITEM2 },
	{ ACT_MP_ATTACK_STAND_POSTFIRE, ACT_ITEM2_ATTACK_STAND_POSTFIRE, TF_WPN_TYPE_ITEM2 },
	{ ACT_MP_ATTACK_STAND_STARTFIRE, ACT_ITEM2_ATTACK_STAND_STARTFIRE, TF_WPN_TYPE_ITEM2 },
	{ ACT_MP_ATTACK_CROUCH_PREFIRE, ACT_ITEM2_ATTACK_CROUCH_PREFIRE, TF_WPN_TYPE_ITEM2 },
	{ ACT_MP_ATTACK_CROUCH_POSTFIRE, ACT_ITEM2_ATTACK_CROUCH_POSTFIRE, TF_WPN_TYPE_ITEM2 },
	{ ACT_MP_ATTACK_SWIM_PREFIRE, ACT_ITEM2_ATTACK_SWIM_PREFIRE, TF_WPN_TYPE_ITEM2 },
	{ ACT_MP_ATTACK_SWIM_POSTFIRE, ACT_ITEM2_ATTACK_SWIM_POSTFIRE, TF_WPN_TYPE_ITEM2 },
	{ ACT_VM_DRAW, ACT_MELEE_ALLCLASS_VM_DRAW, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_HOLSTER, ACT_MELEE_ALLCLASS_VM_HOLSTER, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_IDLE, ACT_MELEE_ALLCLASS_VM_IDLE, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_PULLBACK, ACT_MELEE_ALLCLASS_VM_PULLBACK, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_PRIMARYATTACK, ACT_MELEE_ALLCLASS_VM_PRIMARYATTACK, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_SECONDARYATTACK, ACT_MELEE_ALLCLASS_VM_SECONDARYATTACK, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_RELOAD, ACT_MELEE_ALLCLASS_VM_RELOAD, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_DRYFIRE, ACT_MELEE_ALLCLASS_VM_DRYFIRE, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_MELEE_ALLCLASS_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_IDLE_LOWERED, ACT_MELEE_ALLCLASS_VM_IDLE_LOWERED, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_MELEE_ALLCLASS_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_HITCENTER, ACT_MELEE_ALLCLASS_VM_HITCENTER, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_SWINGHARD, ACT_MELEE_ALLCLASS_VM_SWINGHARD, TF_WPN_TYPE_MELEE_ALLCLASS },
	{ ACT_VM_DRAW, ACT_SECONDARY2_VM_DRAW, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_HOLSTER, ACT_SECONDARY2_VM_HOLSTER, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_IDLE, ACT_SECONDARY2_VM_IDLE, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_PULLBACK, ACT_SECONDARY2_VM_PULLBACK, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_PRIMARYATTACK, ACT_SECONDARY2_VM_PRIMARYATTACK, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_RELOAD, ACT_SECONDARY2_VM_RELOAD, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_RELOAD_START, ACT_SECONDARY2_RELOAD_START, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_RELOAD_FINISH, ACT_SECONDARY2_RELOAD_FINISH, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_DRYFIRE, ACT_SECONDARY2_VM_DRYFIRE, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_SECONDARY2_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_IDLE_LOWERED, ACT_SECONDARY2_VM_IDLE_LOWERED, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_SECONDARY2_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_SECONDARY2 },
	{ ACT_VM_DRAW, ACT_PRIMARY_VM_DRAW, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_HOLSTER, ACT_PRIMARY_VM_HOLSTER, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_IDLE, ACT_PRIMARY_VM_IDLE, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_PULLBACK, ACT_PRIMARY_VM_PULLBACK, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_PRIMARYATTACK, ACT_PRIMARY_VM_PRIMARYATTACK, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_RELOAD, ACT_PRIMARY_VM_RELOAD_3, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_RELOAD_START, ACT_PRIMARY_RELOAD_START_3, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_RELOAD_FINISH, ACT_PRIMARY_RELOAD_FINISH_3, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_DRYFIRE, ACT_PRIMARY_VM_DRYFIRE, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_IDLE_TO_LOWERED, ACT_PRIMARY_VM_IDLE_TO_LOWERED, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_IDLE_LOWERED, ACT_PRIMARY_VM_IDLE_LOWERED, TF_WPN_TYPE_PRIMARY2 },
	{ ACT_VM_LOWERED_TO_IDLE, ACT_PRIMARY_VM_LOWERED_TO_IDLE, TF_WPN_TYPE_PRIMARY2 },
};


acttable_t *CTFWeaponBase::ActivityList( int &iActivityCount )
{
	int iWeaponRole = GetActivityWeaponRole();

	acttable_t *pTable;

	switch ( iWeaponRole )
	{
	case TF_WPN_TYPE_PRIMARY:
	default:
		pTable = s_acttablePrimary;
		iActivityCount = ARRAYSIZE( s_acttablePrimary );
		break;
	case TF_WPN_TYPE_SECONDARY:
		pTable = s_acttableSecondary;
		iActivityCount = ARRAYSIZE( s_acttableSecondary );
		break;
	case TF_WPN_TYPE_MELEE:
		pTable = s_acttableMelee;
		iActivityCount = ARRAYSIZE( s_acttableMelee );
		break;
	case TF_WPN_TYPE_BUILDING:
		pTable = s_acttableBuilding;
		iActivityCount = ARRAYSIZE( s_acttableBuilding );
		break;
	case TF_WPN_TYPE_PDA:
		pTable = s_acttablePDA;
		iActivityCount = ARRAYSIZE( s_acttablePDA );
		break;
	case TF_WPN_TYPE_ITEM1:
		pTable = s_acttableItem1;
		iActivityCount = ARRAYSIZE( s_acttableItem1 );
		break;
	case TF_WPN_TYPE_ITEM2:
		pTable = s_acttableItem2;
		iActivityCount = ARRAYSIZE( s_acttableItem2 );
		break;
	case TF_WPN_TYPE_MELEE_ALLCLASS:
		pTable = s_acttableMeleeAllClass;
		iActivityCount = ARRAYSIZE( s_acttableMeleeAllClass );
		break;
	case TF_WPN_TYPE_SECONDARY2:
		pTable = s_acttableSecondary2;
		iActivityCount = ARRAYSIZE( s_acttableSecondary2 );
		break;
	case TF_WPN_TYPE_PRIMARY2:
		pTable = s_acttablePrimary2;
		iActivityCount = ARRAYSIZE( s_acttablePrimary2 );
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
bool CTFWeaponBase::UsingViewModel( void )
{
	C_TFPlayer *pOwner = GetTFPlayerOwner();

	if ( pOwner && !pOwner->ShouldDrawThisPlayer() )
		return true;

	return false;
}

// -----------------------------------------------------------------------------
// Purpose:
// -----------------------------------------------------------------------------
C_BaseEntity *CTFWeaponBase::GetWeaponForEffect()
{
	C_TFPlayer *pOwner = GetTFPlayerOwner();

	if ( pOwner && !pOwner->ShouldDrawThisPlayer() )
	{
		C_BaseViewModel *pViewModel = pOwner->GetViewModel();

		if ( pViewModel )
			return pViewModel;
	}

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

static ConVar	cl_bobcycle( "cl_bobcycle", "0.8" );
static ConVar	cl_bobup( "cl_bobup", "0.5" );

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
	float flmaxSpeedDelta = max( 0, ( gpGlobals->curtime - pBobState->m_flLastBobTime ) * 320.0f );

	// don't allow too big speed changes
	speed = clamp( speed, pBobState->m_flLastSpeed - flmaxSpeedDelta, pBobState->m_flLastSpeed + flmaxSpeedDelta );
	speed = clamp( speed, -320, 320 );

	pBobState->m_flLastSpeed = speed;

	//FIXME: This maximum speed value must come from the server.
	//		 MaxSpeed() is not sufficient for dealing with sprinting - jdw

	float bob_offset = RemapVal( speed, 0, 320, 0.0f, 1.0f );

	pBobState->m_flBobTime += ( gpGlobals->curtime - pBobState->m_flLastBobTime ) * bob_offset;
	pBobState->m_flLastBobTime = gpGlobals->curtime;

	//Calculate the vertical bob
	cycle = pBobState->m_flBobTime - (int)( pBobState->m_flBobTime / cl_bobcycle.GetFloat() )*cl_bobcycle.GetFloat();
	cycle /= cl_bobcycle.GetFloat();

	if ( cycle < cl_bobup.GetFloat() )
	{
		cycle = M_PI * cycle / cl_bobup.GetFloat();
	}
	else
	{
		cycle = M_PI + M_PI*( cycle - cl_bobup.GetFloat() ) / ( 1.0 - cl_bobup.GetFloat() );
	}

	pBobState->m_flVerticalBob = speed*0.005f;
	pBobState->m_flVerticalBob = pBobState->m_flVerticalBob*0.3 + pBobState->m_flVerticalBob*0.7*sin( cycle );

	pBobState->m_flVerticalBob = clamp( pBobState->m_flVerticalBob, -7.0f, 4.0f );

	//Calculate the lateral bob
	cycle = pBobState->m_flBobTime - (int)( pBobState->m_flBobTime / cl_bobcycle.GetFloat() * 2 )*cl_bobcycle.GetFloat() * 2;
	cycle /= cl_bobcycle.GetFloat() * 2;

	if ( cycle < cl_bobup.GetFloat() )
	{
		cycle = M_PI * cycle / cl_bobup.GetFloat();
	}
	else
	{
		cycle = M_PI + M_PI*( cycle - cl_bobup.GetFloat() ) / ( 1.0 - cl_bobup.GetFloat() );
	}

	pBobState->m_flLateralBob = speed*0.005f;
	pBobState->m_flLateralBob = pBobState->m_flLateralBob*0.3 + pBobState->m_flLateralBob*0.7*sin( cycle );
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
	angles[ROLL] += pBobState->m_flVerticalBob * 0.5f;
	angles[PITCH] -= pBobState->m_flVerticalBob * 0.4f;
	angles[YAW] -= pBobState->m_flLateralBob  * 0.3f;

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
	CTFViewModel *viewModel = dynamic_cast<CTFViewModel *>( baseViewModel );
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
			switch ( iTeamNumber )
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

//-----------------------------------------------------------------------------
// Should this object cast shadows?
//-----------------------------------------------------------------------------
ShadowType_t CTFWeaponBase::ShadowCastType( void )
{
	if ( IsEffectActive( EF_NODRAW | EF_NOSHADOW ) || m_iState != WEAPON_IS_ACTIVE )
		return SHADOWS_NONE;

	return BaseClass::ShadowCastType();
}

bool CTFWeaponBase::OnFireEvent( C_BaseViewModel *pViewModel, const Vector& origin, const QAngle& angles, int event, const char *options )
{
	if ( event == 6002 )
	{
		CEffectData data;
		pViewModel->GetAttachment( atoi( options ), data.m_vOrigin, data.m_vAngles );
		data.m_nHitBox = GetWeaponID();
		DispatchEffect( "TF_EjectBrass", data );
		return true;
	}
	if ( event == AE_WPN_INCREMENTAMMO )
	{
		CTFPlayer *pPlayer = GetTFPlayerOwner();

		if ( pPlayer && pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) > 0 && m_iClip1 < GetMaxClip1() && !m_bReloadedThroughAnimEvent )
		{
			m_iClip1++;
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
	if ( !m_pPercentInvisible )
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

CTFWeaponInfo *GetTFWeaponInfo( int iWeapon )
{
	// Get the weapon information.
	const char *pszWeaponAlias = WeaponIdToAlias( iWeapon );
	if ( !pszWeaponAlias )
	{
		return NULL;
	}

	WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot( pszWeaponAlias );
	if ( hWpnInfo == GetInvalidWeaponInfoHandle() )
	{
		return NULL;
	}

	CTFWeaponInfo *pWeaponInfo = static_cast<CTFWeaponInfo*>( GetFileWeaponInfoFromHandle( hWpnInfo ) );
	return pWeaponInfo;
}

CTFWeaponInfo *GetTFWeaponInfoForItem( CEconItemView *pItem, int iClass )
{
	// Get the weapon information.
	CEconItemDefinition *pItemDef = pItem->GetStaticData();

	if ( !pItemDef )
		return NULL;

	const char *pszClassname = TranslateWeaponEntForClass( pItemDef->item_class, iClass );

	WEAPON_FILE_INFO_HANDLE	hWpnInfo = LookupWeaponInfoSlot( pszClassname );
	if ( hWpnInfo == GetInvalidWeaponInfoHandle() )
	{
		return NULL;
	}

	CTFWeaponInfo *pWeaponInfo = static_cast<CTFWeaponInfo*>( GetFileWeaponInfoFromHandle( hWpnInfo ) );
	return pWeaponInfo;
}
