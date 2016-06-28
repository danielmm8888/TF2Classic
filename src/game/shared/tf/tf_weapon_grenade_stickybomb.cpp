//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Pipebomb Grenade.
//
//=============================================================================//
#include "cbase.h"
#include "tf_weaponbase.h"
#include "tf_gamerules.h"
#include "engine/IEngineSound.h"
#include "tf_weapon_grenade_stickybomb.h"
#include "tf_weapon_pipebomblauncher.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "IEffects.h"
#include "c_team.h"
// Server specific.
#else
#include "tf_player.h"
#include "IEffects.h"
#include "props.h"
#endif

#define TF_WEAPON_STICKYBOMB_MODEL		"models/weapons/w_models/w_stickybomb.mdl"

ConVar tf_grenadelauncher_livetime( "tf_grenadelauncher_livetime", "0.8", FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );

IMPLEMENT_NETWORKCLASS_ALIASED( TFGrenadeStickybombProjectile, DT_TFProjecile_Stickybomb )

BEGIN_NETWORK_TABLE( CTFGrenadeStickybombProjectile, DT_TFProjecile_Stickybomb )
END_NETWORK_TABLE()

#ifdef GAME_DLL
static string_t s_iszSawBlade01;
static string_t s_iszSawBlade02;
#endif

CTFGrenadeStickybombProjectile::CTFGrenadeStickybombProjectile()
{
#ifdef GAME_DLL
	s_iszSawBlade01 = AllocPooledString( "sawmovelinear01" );
	s_iszSawBlade02 = AllocPooledString( "sawmovelinear02" );
	m_bTouched = false;
#else
	m_bPulsed = false;
#endif
}

CTFGrenadeStickybombProjectile::~CTFGrenadeStickybombProjectile()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFGrenadeStickybombProjectile::GetDamageType( void )
{
	int iDmgType = BaseClass::GetDamageType();

	// Do distance based damage falloff for just the first few seconds of our life.
	if ( gpGlobals->curtime - m_flCreationTime < 5.0 )
	{
		iDmgType |= DMG_USEDISTANCEMOD;
	}

	return iDmgType;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::UpdateOnRemove( void )
{
	// Tell our launcher that we were removed
	CTFPipebombLauncher *pLauncher = dynamic_cast<CTFPipebombLauncher*>( m_hLauncher.Get() );

	if ( pLauncher )
	{
		pLauncher->DeathNotice( this );
	}

	BaseClass::UpdateOnRemove();
}

#ifdef CLIENT_DLL

//=============================================================================
//
// TF Stickybomb Projectile functions (Client specific).
//

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFGrenadeStickybombProjectile::GetTrailParticleName( void )
{
	return ConstructTeamParticle( "stickybombtrail_%s", GetTeamNumber(), true );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::CreateTrails( void )
{
	CNewParticleEffect *pParticle = ParticleProp()->Create( GetTrailParticleName(), PATTACH_ABSORIGIN_FOLLOW );

	C_TFPlayer *pPlayer = ToTFPlayer( GetThrower() );

	if ( pPlayer )
	{
		pPlayer->m_Shared.SetParticleToMercColor( pParticle );
	}

	if ( m_bCritical )
	{
		const char *pszEffectName = ConstructTeamParticle( "critical_grenade_%s", GetTeamNumber(), true );

		pParticle = ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );

		if ( pPlayer )
		{
			pPlayer->m_Shared.SetParticleToMercColor( pParticle );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_flCreationTime = gpGlobals->curtime;

		CTFPipebombLauncher *pLauncher = dynamic_cast<CTFPipebombLauncher *>( m_hLauncher.Get() );

		if ( pLauncher )
		{
			pLauncher->AddPipeBomb( this );
		}

		CreateTrails();
	}

#if 0
	if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::Simulate( void )
{
	BaseClass::Simulate();

	if ( m_bPulsed == false )
	{
		if ( ( gpGlobals->curtime - m_flCreationTime ) >= tf_grenadelauncher_livetime.GetFloat() )
		{
			const char *pszEffectName = ConstructTeamParticle( "stickybomb_pulse_%s", GetTeamNumber() );
			ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN );

			m_bPulsed = true;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Don't draw if we haven't yet gone past our original spawn point
// Input  : flags - 
//-----------------------------------------------------------------------------
int CTFGrenadeStickybombProjectile::DrawModel( int flags )
{
	if ( gpGlobals->curtime < ( m_flCreationTime + 0.1 ) )
		return 0;

	return BaseClass::DrawModel( flags );
}

#else

BEGIN_DATADESC( CTFGrenadeStickybombProjectile )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tf_projectile_pipe_remote, CTFGrenadeStickybombProjectile );
PRECACHE_REGISTER( tf_projectile_pipe_remote );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFGrenadeStickybombProjectile *CTFGrenadeStickybombProjectile::Create( const Vector &position, const QAngle &angles,
	const Vector &velocity, const AngularImpulse &angVelocity,
	CBaseCombatCharacter *pOwner, CBaseEntity *pWeapon )
{
	CTFGrenadeStickybombProjectile *pGrenade = static_cast<CTFGrenadeStickybombProjectile *>( CBaseEntity::Create( "tf_projectile_pipe_remote", position, angles, pOwner ) );
	if ( pGrenade )
	{
		pGrenade->InitGrenade( velocity, angVelocity, pOwner, pWeapon );
		pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );
	}

	return pGrenade;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::Spawn( void )
{
	// Set this to max, so effectively they do not self-implode.
	SetModel( TF_WEAPON_STICKYBOMB_MODEL );
	SetDetonateTimerLength( FLT_MAX );

	BaseClass::Spawn();

	m_flCreationTime = gpGlobals->curtime;

	// We want to get touch functions called so we can damage enemy players
	AddSolidFlags( FSOLID_TRIGGER );

	m_flMinSleepTime = 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::Precache( void )
{
	PrecacheModel( TF_WEAPON_STICKYBOMB_MODEL );
	PrecacheTeamParticles( "stickybombtrail_%s", true );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::Detonate()
{
	// If we're detonating stickies then we're currently inside prediction
	// so we gotta make sure all effects show up.
	CDisablePredictionFiltering disabler;

	if ( ShouldNotDetonate() )
	{
		RemoveGrenade();
		return;
	}

	if ( m_bFizzle )
	{
		g_pEffects->Sparks( GetAbsOrigin() );
		RemoveGrenade();
		return;
	}

	BaseClass::Detonate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::Fizzle( void )
{
	m_bFizzle = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::VPhysicsCollision( int index, gamevcollisionevent_t *pEvent )
{
	BaseClass::VPhysicsCollision( index, pEvent );

	int otherIndex = !index;
	CBaseEntity *pHitEntity = pEvent->pEntities[otherIndex];

	if ( !pHitEntity )
		return;

	// Handle hitting skybox (disappear).
	surfacedata_t *pprops = physprops->GetSurfaceData( pEvent->surfaceProps[otherIndex] );
	if ( pprops->game.material == 'X' )
	{
		// uncomment to destroy grenade upon hitting sky brush
		//SetThink( &CTFGrenadePipebombProjectile::SUB_Remove );
		//SetNextThink( gpGlobals->curtime );
		return;
	}

	bool bIsDynamicProp = ( NULL != dynamic_cast<CDynamicProp *>( pHitEntity ) );

	// HACK: Prevents stickies from sticking to blades in Sawmill. Need to find a way that is not as silly.
	CBaseEntity *pParent = pHitEntity->GetMoveParent();
	if ( pParent )
	{
		if ( pParent->NameMatches( s_iszSawBlade01 ) || pParent->NameMatches( s_iszSawBlade02 ) )
		{
			bIsDynamicProp = false;
		}
	}

	// Sitckybombs stick to the world when they touch it.
	if ( ( pHitEntity->IsWorld() || bIsDynamicProp ) && gpGlobals->curtime > m_flMinSleepTime )
	{
		m_bTouched = true;
		VPhysicsGetObject()->EnableMotion( false );

		// Save impact data for explosions.
		m_bUseImpactNormal = true;
		pEvent->pInternalData->GetSurfaceNormal( m_vecImpactNormal );
		m_vecImpactNormal.Negate();
	}
}

ConVar tf_grenade_force_sleeptime( "tf_grenade_force_sleeptime", "1.0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );	// How long after being shot will we re-stick to the world.

extern ConVar tf_grenade_forcefrom_blast;
extern ConVar tf_pipebomb_force_to_move;

//-----------------------------------------------------------------------------
// Purpose: If we are shot after being stuck to the world, move a bit, unless we're a sticky, in which case, fizzle out and die.
//-----------------------------------------------------------------------------
int CTFGrenadeStickybombProjectile::OnTakeDamage( const CTakeDamageInfo &info )
{
	if ( !info.GetAttacker() )
	{
		Assert( !info.GetAttacker() );
		return 0;
	}

	bool bSameTeam = ( info.GetAttacker()->GetTeamNumber() == GetTeamNumber() );

	if ( m_bTouched && ( info.GetDamageType() & ( DMG_BULLET | DMG_BUCKSHOT | DMG_BLAST | DMG_CLUB | DMG_SLASH ) ) && bSameTeam == false )
	{
		Vector vecForce = info.GetDamageForce();

		// Sticky bombs get destroyed by bullets and melee, not pushed
		if ( info.GetDamageType() & ( DMG_BULLET | DMG_BUCKSHOT | DMG_CLUB | DMG_SLASH ) )
		{
			m_bFizzle = true;
			Detonate();
		}
		else if ( info.GetDamageType() & DMG_BLAST )
		{
			vecForce *= tf_grenade_forcefrom_blast.GetFloat();
		}

		// If the force is sufficient, detach & move the pipebomb
		float flForce = tf_pipebomb_force_to_move.GetFloat();
		if ( vecForce.LengthSqr() > ( flForce*flForce ) )
		{
			if ( VPhysicsGetObject() )
			{
				VPhysicsGetObject()->EnableMotion( true );
			}

			CTakeDamageInfo newInfo = info;
			newInfo.SetDamageForce( vecForce );

			VPhysicsTakeDamage( newInfo );

			// The pipebomb will re-stick to the ground after this time expires
			m_flMinSleepTime = gpGlobals->curtime + tf_grenade_force_sleeptime.GetFloat();
			m_bTouched = false;

			// It has moved the data is no longer valid.
			m_bUseImpactNormal = false;
			m_vecImpactNormal.Init();

			return 1;
		}
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadeStickybombProjectile::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	// This is kind of lame.
	Vector vecPushSrc = pDeflectedBy->WorldSpaceCenter();
	Vector vecPushDir = GetAbsOrigin() - vecPushSrc;

	CTakeDamageInfo info( pDeflectedBy, pDeflectedBy, 100, DMG_BLAST );
	CalculateExplosiveDamageForce( &info, vecPushDir, vecPushSrc );
	TakeDamage( info );

	// TODO: Live TF2 adds white trail to reflected pipes and stickies. We need one as well.
}

#endif
