//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Pipebomb Grenade.
//
//=============================================================================//
// Warning to all who enter trying to figure out grenade code:
// This file contains both Sticky Bombs and Grenade Launcher grenades. Valve seemed to not be able to decide what the hell
// they should call them, and so half of the file refers to stickies as pipebombs, and grenades as grenades,
// and the other half refers to stickies as grenades and grenades as pipebombs.
// I've tried to mark which ones are which with comments at the start of functions so that future coders know what's up.
// - Iamgoofball
//=============================================================================//
#include "cbase.h"
#include "tf_weaponbase.h"
#include "tf_gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "tf_weapon_grenade_pipebomb.h"
#include "tf_weapon_pipebomblauncher.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "IEffects.h"
#include "c_team.h"
// Server specific.
#else
#include "tf_player.h"
#include "items.h"
#include "tf_weaponbase_grenadeproj.h"
#include "soundent.h"
#include "KeyValues.h"
#include "IEffects.h"
#include "props.h"
#include "func_respawnroom.h"
#endif

#define TF_WEAPON_PIPEBOMB_TIMER		3.0f //Seconds

#define TF_WEAPON_PIPEBOMB_GRAVITY		0.5f
#define TF_WEAPON_PIPEBOMB_FRICTION		0.8f
#define TF_WEAPON_PIPEBOMB_ELASTICITY	0.45f

#define TF_WEAPON_PIPEBOMB_TIMER_DMG_REDUCTION		0.6

extern ConVar tf_grenadelauncher_max_chargetime;
ConVar tf_grenadelauncher_chargescale( "tf_grenadelauncher_chargescale", "1.0", FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar tf_grenadelauncher_livetime( "tf_grenadelauncher_livetime", "0.8", FCVAR_CHEAT | FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );

#ifndef CLIENT_DLL
ConVar tf_grenadelauncher_min_contact_speed( "tf_grenadelauncher_min_contact_speed", "100", FCVAR_DEVELOPMENTONLY );
#endif

IMPLEMENT_NETWORKCLASS_ALIASED( TFGrenadePipebombProjectile, DT_TFProjectile_Pipebomb )

BEGIN_NETWORK_TABLE( CTFGrenadePipebombProjectile, DT_TFProjectile_Pipebomb )
#ifdef CLIENT_DLL
RecvPropInt( RECVINFO( m_bTouched ) ),
RecvPropInt( RECVINFO( m_iType ) ),
RecvPropEHandle( RECVINFO( m_hLauncher ) ),
#else
SendPropBool( SENDINFO( m_bTouched ) ),
SendPropInt( SENDINFO( m_iType ), 2 ),
SendPropEHandle( SENDINFO( m_hLauncher ) ),
#endif
END_NETWORK_TABLE()

#ifdef GAME_DLL
static string_t s_iszTrainName;
static string_t s_iszSawBlade01;
static string_t s_iszSawBlade02;
#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFGrenadePipebombProjectile::CTFGrenadePipebombProjectile()
{
	m_bTouched = false;
	m_flChargeTime = 0.0f;
#ifdef GAME_DLL
	s_iszTrainName  = AllocPooledString( "models/props_vehicles/train_enginecar.mdl" );
	s_iszSawBlade01 = AllocPooledString( "sawmovelinear01" );
	s_iszSawBlade02 = AllocPooledString( "sawmovelinear02" );
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFGrenadePipebombProjectile::~CTFGrenadePipebombProjectile()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
// PIPEBOMB = STICKY
//-----------------------------------------------------------------------------
int	CTFGrenadePipebombProjectile::GetDamageType( void )
{
	int iDmgType = BaseClass::GetDamageType();

	// If we're a pipebomb, we do distance based damage falloff for just the first few seconds of our life
	if ( m_iType == TF_GL_MODE_REMOTE_DETONATE )
	{
		if ( gpGlobals->curtime - m_flCreationTime < 5.0 )
		{
			iDmgType |= DMG_USEDISTANCEMOD;
		}
	}

	return iDmgType;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::UpdateOnRemove( void )
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
// TF Pipebomb Grenade Projectile functions (Client specific).
//

//-----------------------------------------------------------------------------
// Purpose: 
// Output : const char
// STICKYBOMB = STICKY
// PIPEBOMB = GRENADE
//-----------------------------------------------------------------------------
const char *CTFGrenadePipebombProjectile::GetTrailParticleName( void )
{
	if ( m_iType == TF_GL_MODE_REMOTE_DETONATE )
	{
		return ConstructTeamParticle( "stickybombtrail_%s", GetTeamNumber(), true );
	}
	else
	{
		return ConstructTeamParticle( "pipebombtrail_%s", GetTeamNumber(), true );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : updateType - 
// GRENADE = STICKY
// PIPE = GRENADE
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::CreateTrails( void )
{
	CNewParticleEffect *pParticle = ParticleProp()->Create( GetTrailParticleName(), PATTACH_ABSORIGIN_FOLLOW );

	C_TFPlayer *pPlayer = ToTFPlayer( GetThrower() );

	if ( pPlayer )
	{
		pPlayer->m_Shared.SetParticleToMercColor( pParticle );
	}

	if ( m_bCritical )
	{
		const char *pszFormat = ( m_iType == TF_GL_MODE_REMOTE_DETONATE ) ? "critical_grenade_%s" : "critical_pipe_%s";
		const char *pszEffectName = ConstructTeamParticle( pszFormat, GetTeamNumber(), true );

		pParticle = ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );

		if ( pPlayer )
		{
			pPlayer->m_Shared.SetParticleToMercColor( pParticle );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : updateType - 
// PIPEBOMB = STICKY
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_flCreationTime = gpGlobals->curtime;

		m_bPulsed = false;

		CTFPipebombLauncher *pLauncher = dynamic_cast<CTFPipebombLauncher*>( m_hLauncher.Get() );

		if ( pLauncher )
		{
			pLauncher->AddPipeBomb( this );
		}

		CreateTrails();
	}
	else if ( m_bTouched )
	{
		//ParticleProp()->StopEmission();
	}

	if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
	}
}

extern ConVar tf_grenadelauncher_livetime;

void CTFGrenadePipebombProjectile::Simulate( void )
{
	BaseClass::Simulate();

	if ( m_iType != TF_GL_MODE_REMOTE_DETONATE )
		return;

	if ( m_bPulsed == false )
	{
		if ( (gpGlobals->curtime - m_flCreationTime) >= tf_grenadelauncher_livetime.GetFloat() )
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
int CTFGrenadePipebombProjectile::DrawModel( int flags )
{
	if ( gpGlobals->curtime < ( m_flCreationTime + 0.1 ) )
		return 0;

	return BaseClass::DrawModel( flags );
}

#else

//=============================================================================
//
// TF Pipebomb Grenade Projectile functions (Server specific).
//
#define TF_WEAPON_PIPEGRENADE_MODEL		"models/weapons/w_models/w_grenade_grenadelauncher.mdl"
#define TF_WEAPON_PIPEBOMB_MODEL		"models/weapons/w_models/w_stickybomb.mdl"
#define TF_WEAPON_PIPEBOMB_BOUNCE_SOUND	"Weapon_Grenade_Pipebomb.Bounce"
#define TF_WEAPON_GRENADE_DETONATE_TIME 2.0f
#define TF_WEAPON_GRENADE_XBOX_DAMAGE 112

BEGIN_DATADESC( CTFGrenadePipebombProjectile )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tf_projectile_pipe_remote, CTFGrenadePipebombProjectile );
PRECACHE_WEAPON_REGISTER( tf_projectile_pipe_remote );

LINK_ENTITY_TO_CLASS( tf_projectile_pipe, CTFGrenadePipebombProjectile );
PRECACHE_WEAPON_REGISTER( tf_projectile_pipe );

//-----------------------------------------------------------------------------
// Purpose:
// PIPEBOMB = STICKY
// GRENADE = GRENADE (for once)
//-----------------------------------------------------------------------------
CTFGrenadePipebombProjectile* CTFGrenadePipebombProjectile::Create( const Vector &position, const QAngle &angles, 
																    const Vector &velocity, const AngularImpulse &angVelocity, 
																    CBaseCombatCharacter *pOwner, const CTFWeaponInfo &weaponInfo, bool bRemoteDetonate, float flDamageMult )
{
	CTFGrenadePipebombProjectile *pGrenade = static_cast<CTFGrenadePipebombProjectile*>( CBaseEntity::CreateNoSpawn( bRemoteDetonate ? "tf_projectile_pipe_remote" : "tf_projectile_pipe", position, angles, pOwner ) );
	if ( pGrenade )
	{
		// Set the pipebomb mode before calling spawn, so the model & associated vphysics get setup properly
		pGrenade->SetPipebombMode( bRemoteDetonate );
		DispatchSpawn( pGrenade );

		pGrenade->InitGrenade( velocity, angVelocity, pOwner, weaponInfo );

#ifdef _X360 
		if ( pGrenade->m_iType != TF_GL_MODE_REMOTE_DETONATE )
		{
			pGrenade->SetDamage( TF_WEAPON_GRENADE_XBOX_DAMAGE );
		}
#endif

		pGrenade->SetDamage( pGrenade->GetDamage() * flDamageMult );
		pGrenade->m_flFullDamage = pGrenade->GetDamage();

		if ( pGrenade->m_iType != TF_GL_MODE_REMOTE_DETONATE )
		{
			// Some hackery here. Reduce the damage by 25%, so that if we explode on timeout,
			// we'll do less damage. If we explode on contact, we'll restore this to full damage.
			pGrenade->SetDamage( pGrenade->GetDamage() * TF_WEAPON_PIPEBOMB_TIMER_DMG_REDUCTION );
		}

		pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );
	}

	return pGrenade;
}

//-----------------------------------------------------------------------------
// Purpose:
// PIPEBOMB = STICKY
// PIPEGRENADE\GRENADE = GRENADE
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::Spawn()
{
	if ( m_iType == TF_GL_MODE_REMOTE_DETONATE )
	{
		// Set this to max, so effectively they do not self-implode.
		SetModel( TF_WEAPON_PIPEBOMB_MODEL );
		SetDetonateTimerLength( FLT_MAX );
	}
	else
	{
		SetModel( TF_WEAPON_PIPEGRENADE_MODEL );
		SetDetonateTimerLength( TF_WEAPON_GRENADE_DETONATE_TIME );
		SetTouch( &CTFGrenadePipebombProjectile::PipebombTouch );
	}

	BaseClass::Spawn();

	m_bTouched = false;
	m_flCreationTime = gpGlobals->curtime;

	// We want to get touch functions called so we can damage enemy players
	AddSolidFlags( FSOLID_TRIGGER );

	m_flMinSleepTime = 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::Precache()
{
	PrecacheModel( TF_WEAPON_PIPEBOMB_MODEL );
	PrecacheModel( TF_WEAPON_PIPEGRENADE_MODEL );
	PrecacheParticleSystem( "stickybombtrail_blue" );
	PrecacheParticleSystem( "stickybombtrail_red" );
	PrecacheParticleSystem( "stickybombtrail_green" );
	PrecacheParticleSystem( "stickybombtrail_yellow" );
	PrecacheParticleSystem( "stickybombtrail_dm" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::SetPipebombMode( bool bRemoteDetonate )
{
	if ( bRemoteDetonate )
	{
		m_iType.Set( TF_GL_MODE_REMOTE_DETONATE );
	}
	else
	{
		SetModel( TF_WEAPON_PIPEGRENADE_MODEL );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::BounceSound( void )
{
	EmitSound( TF_WEAPON_PIPEBOMB_BOUNCE_SOUND );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::Detonate()
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
void CTFGrenadePipebombProjectile::Fizzle( void )
{
	m_bFizzle = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::PipebombTouch( CBaseEntity *pOther )
{
	if ( pOther == GetThrower() )
		return;

	// Verify a correct "other."
	if ( !pOther->IsSolid() || pOther->IsSolidFlagSet( FSOLID_VOLUME_CONTENTS ) )
		return;

	// Handle hitting skybox (disappear).
	trace_t pTrace;
	Vector velDir = GetAbsVelocity();
	VectorNormalize( velDir );
	Vector vecSpot = GetAbsOrigin() - velDir * 32;
	UTIL_TraceLine( vecSpot, vecSpot + velDir * 64, MASK_SOLID, this, COLLISION_GROUP_NONE, &pTrace );

	if ( pTrace.fraction < 1.0 && pTrace.surface.flags & SURF_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	//If we already touched a surface then we're not exploding on contact anymore.
	if ( m_bTouched == true )
		return;

	// Blow up if we hit an enemy we can damage
	if ( pOther->GetTeamNumber() && ( pOther->GetTeamNumber() != GetTeamNumber() || TFGameRules()->IsDeathmatch() ) && pOther->m_takedamage != DAMAGE_NO )
	{
		// Check to see if this is a respawn room.
		if ( !pOther->IsPlayer() )
		{
			CFuncRespawnRoom *pRespawnRoom = dynamic_cast<CFuncRespawnRoom*>( pOther );
			if ( pRespawnRoom )
			{
				if ( !pRespawnRoom->PointIsWithin( GetAbsOrigin() ) )
					return;
			}
		}

		// Restore damage. See comment in CTFGrenadePipebombProjectile::Create() above to understand this.
		m_flDamage = m_flFullDamage;
		// Save this entity as enemy, they will take 100% damage.
		m_hEnemy = pOther;
		Explode( &pTrace, GetDamageType() );
	}

	// Train hack!
	if ( pOther->GetModelName() == s_iszTrainName && ( pOther->GetAbsVelocity().LengthSqr() > 1.0f ) )
	{
		Explode( &pTrace, GetDamageType() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::VPhysicsCollision( int index, gamevcollisionevent_t *pEvent )
{
	BaseClass::VPhysicsCollision( index, pEvent );

	int otherIndex = !index;
	CBaseEntity *pHitEntity = pEvent->pEntities[otherIndex];

	if ( !pHitEntity )
		return;

	if (m_iType == TF_GL_MODE_REGULAR)
	{
		// Blow up if we hit an enemy we can damage
		if ( pHitEntity->GetTeamNumber() && pHitEntity->GetTeamNumber() != GetTeamNumber() && pHitEntity->m_takedamage != DAMAGE_NO )
		{
			// Save this entity as enemy, they will take 100% damage.
			m_hEnemy = pHitEntity;
			SetThink( &CTFGrenadePipebombProjectile::Detonate );
			SetNextThink( gpGlobals->curtime );
		}
		
		m_bTouched = true;
		return;
	}

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

	// Pipebombs stick to the world when they touch it
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

ConVar tf_grenade_forcefrom_bullet( "tf_grenade_forcefrom_bullet", "0.8", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar tf_grenade_forcefrom_buckshot( "tf_grenade_forcefrom_buckshot", "0.5", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar tf_grenade_forcefrom_blast( "tf_grenade_forcefrom_blast", "0.08", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar tf_grenade_force_sleeptime( "tf_grenade_force_sleeptime", "1.0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );	// How long after being shot will we re-stick to the world.
ConVar tf_pipebomb_force_to_move( "tf_pipebomb_force_to_move", "1500.0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );

//-----------------------------------------------------------------------------
// Purpose: If we are shot after being stuck to the world, move a bit, unless we're a sticky, in which case, fizzle out and die.
// STICKY = STICKY
// PIPEBOMB = GRENADE
//-----------------------------------------------------------------------------
int CTFGrenadePipebombProjectile::OnTakeDamage( const CTakeDamageInfo &info )
{
	if ( !info.GetAttacker() )
	{
		Assert( !info.GetAttacker() );
		return 0;
	}

	bool bSameTeam = ( info.GetAttacker()->GetTeamNumber() == GetTeamNumber() );


	if ( m_bTouched && ( info.GetDamageType() & (DMG_BULLET|DMG_BUCKSHOT|DMG_BLAST|DMG_CLUB|DMG_SLASH) ) && bSameTeam == false )
	{
		Vector vecForce = info.GetDamageForce();
		// Sticky bombs get destroyed by bullets and melee, not pushed
		if ( m_iType == TF_GL_MODE_REMOTE_DETONATE )
		{
			if ( info.GetDamageType() & (DMG_BULLET|DMG_BUCKSHOT|DMG_CLUB|DMG_SLASH) )
			{
				m_bFizzle = true;
				Detonate();
			}
			else if ( info.GetDamageType() & DMG_BLAST )
			{
				vecForce *= tf_grenade_forcefrom_blast.GetFloat();
			}
		}
		else
		{
			if ( info.GetDamageType() & DMG_BULLET )
			{
				vecForce *= tf_grenade_forcefrom_bullet.GetFloat();
			}
			else if ( info.GetDamageType() & DMG_BUCKSHOT )
			{
				vecForce *= tf_grenade_forcefrom_buckshot.GetFloat();
			}
			else if ( info.GetDamageType() & DMG_BLAST )
			{
				vecForce *= tf_grenade_forcefrom_blast.GetFloat();
			}
		}

		// If the force is sufficient, detach & move the pipebomb
		float flForce = tf_pipebomb_force_to_move.GetFloat();
		if ( vecForce.LengthSqr() > (flForce*flForce) )
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
void CTFGrenadePipebombProjectile::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	if ( GetType() == TF_GL_MODE_REMOTE_DETONATE )
	{
		// This is kind of lame.
		Vector vecPushSrc = pDeflectedBy->WorldSpaceCenter();
		Vector vecPushDir = GetAbsOrigin() - vecPushSrc;
		VectorNormalize( vecPushDir );

		CTakeDamageInfo info( pDeflectedBy, pDeflectedBy, 100, DMG_BLAST );
		CalculateExplosiveDamageForce( &info, vecPushDir, vecPushSrc );
		TakeDamage( info );
	}
	else
	{
		BaseClass::Deflected( pDeflectedBy, vecDir );
	}
	// TODO: Live TF2 adds white trail to reflected pipes and stickies. We need one as well.
}


#endif
