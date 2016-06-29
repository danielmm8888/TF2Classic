//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Nail
//
//=============================================================================
#include "cbase.h"
#include "tf_projectile_nail.h"
#include "tf_gamerules.h"

#ifdef CLIENT_DLL
#include "c_basetempentity.h"
#include "c_te_legacytempents.h"
#include "c_te_effect_dispatch.h"
#include "input.h"
#include "c_tf_player.h"
#include "cliententitylist.h"
#endif

//=============================================================================
//
// TF Syringe Projectile functions (Server specific).
//
#define SYRINGE_MODEL				"models/weapons/w_models/w_syringe_proj.mdl"
#define SYRINGE_DISPATCH_EFFECT		"ClientProjectile_Syringe"
#define SYRINGE_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS( tf_projectile_syringe, CTFProjectile_Syringe );
PRECACHE_REGISTER( tf_projectile_syringe );

short g_sModelIndexSyringe;
void PrecacheSyringe( void *pUser )
{
	g_sModelIndexSyringe = modelinfo->GetModelIndex( SYRINGE_MODEL );
}

PRECACHE_REGISTER_FN( PrecacheSyringe );

CTFProjectile_Syringe::CTFProjectile_Syringe()
{
}

CTFProjectile_Syringe::~CTFProjectile_Syringe()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Syringe *CTFProjectile_Syringe::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, bool bCritical )
{
	return static_cast<CTFProjectile_Syringe*>( CTFBaseProjectile::Create( "tf_projectile_syringe", vecOrigin, vecAngles, pOwner, CTFProjectile_Syringe::GetInitialVelocity(), g_sModelIndexSyringe, SYRINGE_DISPATCH_EFFECT, pScorer, bCritical ) );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Syringe::GetProjectileModelName( void )
{
	return SYRINGE_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Syringe::GetGravity( void )
{
	return SYRINGE_GRAVITY;
}

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetSyringeTrailParticleName( int iTeamNumber, bool bCritical )
{
	const char *pszFormat = bCritical ? "nailtrails_medic_%s_crit" : "nailtrails_medic_%s";

	return ConstructTeamParticle( pszFormat, iTeamNumber, true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileSyringeCallback( const CEffectData &data )
{
	// Get the syringe and add it to the client entity list, so we can attach a particle system to it.
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>( ClientEntityList().GetBaseEntityFromHandle( data.m_hEntity ) );
	if ( pPlayer )
	{
		C_LocalTempEntity *pSyringe = ClientsideProjectileCallback( data, SYRINGE_GRAVITY );
		if ( pSyringe )
		{
			switch ( pPlayer->GetTeamNumber() )
			{
			case TF_TEAM_RED:
				pSyringe->m_nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				pSyringe->m_nSkin = 1;
				break;
			case TF_TEAM_GREEN:
				pSyringe->m_nSkin = 2;
				break;
			case TF_TEAM_YELLOW:
				pSyringe->m_nSkin = 3;
				break;
			}
			bool bCritical = ( ( data.m_nDamageType & DMG_CRITICAL ) != 0 );
			pPlayer->m_Shared.SetParticleToMercColor(
				pSyringe->AddParticleEffect( GetSyringeTrailParticleName( pPlayer->GetTeamNumber(), bCritical ) )
				);
			pSyringe->AddEffects( EF_NOSHADOW );
			pSyringe->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT( SYRINGE_DISPATCH_EFFECT, ClientsideProjectileSyringeCallback );

#endif


// Nail projectile used by the Nailgun

#define NAILGUN_NAIL_MODEL				"models/weapons/w_models/w_nail.mdl"
#define NAILGUN_NAIL_DISPATCH_EFFECT	"ClientProjectile_Nail"
#define NAILGUN_NAIL_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS( tf_projectile_nail, CTFProjectile_Nail );
PRECACHE_REGISTER( tf_projectile_nail );

short g_sModelIndexNail;
void PrecacheNail( void *pUser )
{
	g_sModelIndexNail = modelinfo->GetModelIndex( NAILGUN_NAIL_MODEL );
}

PRECACHE_REGISTER_FN( PrecacheNail );

CTFProjectile_Nail::CTFProjectile_Nail()
{
}

CTFProjectile_Nail::~CTFProjectile_Nail()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Nail *CTFProjectile_Nail::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, bool bCritical )
{
	return static_cast<CTFProjectile_Nail*>( CTFBaseProjectile::Create( "tf_projectile_nail", vecOrigin, vecAngles, pOwner, CTFProjectile_Nail::GetInitialVelocity(), g_sModelIndexNail, NAILGUN_NAIL_DISPATCH_EFFECT, pScorer, bCritical ) );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Nail::GetProjectileModelName( void )
{
	return NAILGUN_NAIL_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Nail::GetGravity( void )
{
	// TODO: Maybe it would be a better idea to have some kind of a keyvalue in the weapon file
	// to easily change the nail gravity?
	return NAILGUN_NAIL_GRAVITY;
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetNailTrailParticleName( int iTeamNumber, bool bCritical )
{
	const char *pszFormat = bCritical ? "nailtrails_scout_%s_crit" : "nailtrails_scout_%s";

	return ConstructTeamParticle( pszFormat, iTeamNumber, true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileNailCallback( const CEffectData &data )
{
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>( ClientEntityList().GetBaseEntityFromHandle( data.m_hEntity ) );
	if ( pPlayer )
	{
		C_LocalTempEntity *pNail = ClientsideProjectileCallback( data, NAILGUN_NAIL_GRAVITY );
		if ( pNail )
		{
			switch ( pPlayer->GetTeamNumber() )
			{
			case TF_TEAM_RED:
				pNail->m_nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				pNail->m_nSkin = 1;
				break;
			case TF_TEAM_GREEN:
				pNail->m_nSkin = 2;
				break;
			case TF_TEAM_YELLOW:
				pNail->m_nSkin = 3;
				break;
			}
			bool bCritical = ( ( data.m_nDamageType & DMG_CRITICAL ) != 0 );
			pPlayer->m_Shared.SetParticleToMercColor(
				pNail->AddParticleEffect( GetNailTrailParticleName( pPlayer->GetTeamNumber(), bCritical ) )
				);
			pNail->AddEffects( EF_NOSHADOW );
			pNail->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT( NAILGUN_NAIL_DISPATCH_EFFECT, ClientsideProjectileNailCallback );

#endif

// Tranqualizer syringe (same as medic's syringe, except with a different gravity and particle effect

#define TRANQDART_MODEL				"models/weapons/w_models/w_dart.mdl"
#define TRANQDART_DISPATCH_EFFECT	"ClientProjectile_TranqDart"
#define TRANQDART_GRAVITY	0.3f

LINK_ENTITY_TO_CLASS( tf_projectile_dart, CTFProjectile_Dart );
PRECACHE_REGISTER( tf_projectile_dart );

short g_sModelIndexTranqDart;
void PrecacheTranqDart( void *pUser )
{
	g_sModelIndexTranqDart = modelinfo->GetModelIndex( TRANQDART_MODEL );
}

PRECACHE_REGISTER_FN( PrecacheTranqDart );


CTFProjectile_Dart::CTFProjectile_Dart()
{
}

CTFProjectile_Dart::~CTFProjectile_Dart()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Dart *CTFProjectile_Dart::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer, bool bCritical )
{
	return static_cast<CTFProjectile_Dart*>( CTFBaseProjectile::Create( "tf_projectile_dart", vecOrigin, vecAngles, pOwner, CTFProjectile_Dart::GetInitialVelocity(), g_sModelIndexTranqDart, TRANQDART_DISPATCH_EFFECT, pScorer, bCritical ) );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *CTFProjectile_Dart::GetProjectileModelName( void )
{
	return TRANQDART_MODEL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFProjectile_Dart::GetGravity( void )
{
	return TRANQDART_GRAVITY;
}

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
//-----------------------------------------------------------------------------
const char *GetTranqDartTrailParticleName( int iTeamNumber, bool bCritical )
{
	const char *pszFormat = bCritical ? "tranq_tracer_teamcolor_%s_crit" : "tranq_tracer_teamcolor_%s";

	return ConstructTeamParticle( pszFormat, iTeamNumber, true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void ClientsideProjectileTranqDartCallback( const CEffectData &data )
{
	// Get the syringe and add it to the client entity list, so we can attach a particle system to it.
	C_TFPlayer *pPlayer = dynamic_cast<C_TFPlayer*>( ClientEntityList().GetBaseEntityFromHandle( data.m_hEntity ) );
	if ( pPlayer )
	{
		C_LocalTempEntity *pSyringe = ClientsideProjectileCallback( data, TRANQDART_GRAVITY );
		if ( pSyringe )
		{
			switch ( pPlayer->GetTeamNumber() )
			{
			case TF_TEAM_RED:
				pSyringe->m_nSkin = 0;
				break;
			case TF_TEAM_BLUE:
				pSyringe->m_nSkin = 1;
				break;
			case TF_TEAM_GREEN:
				pSyringe->m_nSkin = 2;
				break;
			case TF_TEAM_YELLOW:
				pSyringe->m_nSkin = 3;
				break;
			}
			bool bCritical = ( ( data.m_nDamageType & DMG_CRITICAL ) != 0 );
			pPlayer->m_Shared.SetParticleToMercColor(
				pSyringe->AddParticleEffect( GetTranqDartTrailParticleName( pPlayer->GetTeamNumber(), bCritical ) )
				);
			pSyringe->AddEffects( EF_NOSHADOW );
			pSyringe->flags |= FTENT_USEFASTCOLLISIONS;
		}
	}
}

DECLARE_CLIENT_EFFECT( TRANQDART_DISPATCH_EFFECT, ClientsideProjectileTranqDartCallback );

#endif
