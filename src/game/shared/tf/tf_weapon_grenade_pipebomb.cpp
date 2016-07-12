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
#include "engine/IEngineSound.h"
#include "tf_weapon_grenade_pipebomb.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "c_team.h"
// Server specific.
#else
#include "tf_player.h"
#include "func_respawnroom.h"
#endif

#define TF_WEAPON_PIPEBOMB_TIMER		3.0f //Seconds

#define TF_WEAPON_PIPEBOMB_GRAVITY		0.5f
#define TF_WEAPON_PIPEBOMB_FRICTION		0.8f
#define TF_WEAPON_PIPEBOMB_ELASTICITY	0.45f

#define TF_WEAPON_PIPEBOMB_TIMER_DMG_REDUCTION		0.6

IMPLEMENT_NETWORKCLASS_ALIASED( TFGrenadePipebombProjectile, DT_TFProjectile_Pipebomb )

BEGIN_NETWORK_TABLE( CTFGrenadePipebombProjectile, DT_TFProjectile_Pipebomb )
END_NETWORK_TABLE()

#ifdef GAME_DLL
static string_t s_iszTrainName;
#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFGrenadePipebombProjectile::CTFGrenadePipebombProjectile()
{
#ifdef GAME_DLL
	m_bTouched = false;
	m_flFullDamage = 0.0f;
	s_iszTrainName = AllocPooledString( "models/props_vehicles/train_enginecar.mdl" );
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

#ifdef CLIENT_DLL
//=============================================================================
//
// TF Pipebomb Grenade Projectile functions (Client specific).
//

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CTFGrenadePipebombProjectile::GetTrailParticleName( void )
{
	return ConstructTeamParticle( "pipebombtrail_%s", GetTeamNumber(), true );
}

//-----------------------------------------------------------------------------
// Purpose: 
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
		const char *pszEffectName = ConstructTeamParticle(  "critical_pipe_%s", GetTeamNumber(), true );

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
void CTFGrenadePipebombProjectile::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		m_flCreationTime = gpGlobals->curtime;

		CreateTrails();
	}
	else if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
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
#define TF_WEAPON_PIPEBOMB_BOUNCE_SOUND	"Weapon_Grenade_Pipebomb.Bounce"
#define TF_WEAPON_GRENADE_DETONATE_TIME 2.0f
#define TF_WEAPON_GRENADE_XBOX_DAMAGE 112

BEGIN_DATADESC( CTFGrenadePipebombProjectile )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tf_projectile_pipe, CTFGrenadePipebombProjectile );
PRECACHE_WEAPON_REGISTER( tf_projectile_pipe );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFGrenadePipebombProjectile *CTFGrenadePipebombProjectile::Create( const Vector &position, const QAngle &angles,
	const Vector &velocity, const AngularImpulse &angVelocity,
	CBaseCombatCharacter *pOwner, CBaseEntity *pWeapon )
{
	CTFGrenadePipebombProjectile *pGrenade = static_cast<CTFGrenadePipebombProjectile *>( CBaseEntity::Create( "tf_projectile_pipe", position, angles, pOwner ) );
	if ( pGrenade )
	{
		pGrenade->InitGrenade( velocity, angVelocity, pOwner, pWeapon );

#ifdef _X360 
		pGrenade->SetDamage( TF_WEAPON_GRENADE_XBOX_DAMAGE );
#endif

		pGrenade->m_flFullDamage = pGrenade->GetDamage();

		// Some hackery here. Reduce the damage by 25%, so that if we explode on timeout,
		// we'll do less damage. If we explode on contact, we'll restore this to full damage.
		pGrenade->SetDamage( pGrenade->GetDamage() * TF_WEAPON_PIPEBOMB_TIMER_DMG_REDUCTION );

		pGrenade->ApplyLocalAngularVelocityImpulse( angVelocity );
	}

	return pGrenade;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::Spawn()
{
	SetModel( TF_WEAPON_PIPEGRENADE_MODEL );
	SetDetonateTimerLength( TF_WEAPON_GRENADE_DETONATE_TIME );
	SetTouch( &CTFGrenadePipebombProjectile::PipebombTouch );

	BaseClass::Spawn();

	m_flCreationTime = gpGlobals->curtime;

	// We want to get touch functions called so we can damage enemy players
	AddSolidFlags( FSOLID_TRIGGER );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadePipebombProjectile::Precache()
{
	PrecacheModel( TF_WEAPON_PIPEGRENADE_MODEL );

	BaseClass::Precache();
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
	if ( ShouldNotDetonate() )
	{
		RemoveGrenade();
		return;
	}

	BaseClass::Detonate();
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

	// Blow up if we hit an enemy we can damage
	if ( pHitEntity->GetTeamNumber() && pHitEntity->GetTeamNumber() != GetTeamNumber() && pHitEntity->m_takedamage != DAMAGE_NO )
	{
		// Save this entity as enemy, they will take 100% damage.
		m_hEnemy = pHitEntity;
		SetThink( &CTFGrenadePipebombProjectile::Detonate );
		SetNextThink( gpGlobals->curtime );
	}

	m_bTouched = true;
}

ConVar tf_grenade_forcefrom_bullet( "tf_grenade_forcefrom_bullet", "0.8", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar tf_grenade_forcefrom_buckshot( "tf_grenade_forcefrom_buckshot", "0.5", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar tf_grenade_forcefrom_blast( "tf_grenade_forcefrom_blast", "0.08", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar tf_pipebomb_force_to_move( "tf_pipebomb_force_to_move", "1500.0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );

//-----------------------------------------------------------------------------
// Purpose: If we are shot after being stuck to the world, move a bit, unless we're a sticky, in which case, fizzle out and die.
//-----------------------------------------------------------------------------
int CTFGrenadePipebombProjectile::OnTakeDamage( const CTakeDamageInfo &info )
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
	BaseClass::Deflected( pDeflectedBy, vecDir );

	// TODO: Live TF2 adds white trail to reflected pipes and stickies. We need one as well.
}


#endif
