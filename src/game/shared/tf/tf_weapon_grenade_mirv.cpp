//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Mirv Grenade.
//
//=============================================================================//
#include "cbase.h"
#include "tf_weaponbase.h"
#include "tf_gamerules.h"
#include "engine/IEngineSound.h"
#include "tf_weapon_grenade_mirv.h"

// Server specific.
#ifdef GAME_DLL
#include "tf_player.h"
#include "tf_weaponbase_grenadeproj.h"
#include "soundent.h"
#include "particle_parse.h"
#endif

#define MIRV_WARN_TIME				0.5f

#define MIRV_LEADIN_SOUND			"Weapon_Grenade_Mirv.LeadIn"

//=============================================================================
//
// TF Mirv Grenade Projectile functions (Server specific).
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFGrenadeMirvProjectile, DT_TFProjectile_Mirv );
BEGIN_NETWORK_TABLE( CTFGrenadeMirvProjectile, DT_TFProjectile_Mirv )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_weapon_grenade_mirv_projectile, CTFGrenadeMirvProjectile );
PRECACHE_WEAPON_REGISTER( tf_weapon_grenade_mirv_projectile );

CTFGrenadeMirvProjectile::CTFGrenadeMirvProjectile()
{
#ifdef GAME_DLL
	m_bPlayedLeadIn = false;
	m_bDefused = false;
#endif
}

CTFGrenadeMirvProjectile::~CTFGrenadeMirvProjectile()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#endif
}

#ifdef GAME_DLL

#define GRENADE_MODEL "models/weapons/w_models/w_grenade_mirv.mdl"

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFGrenadeMirvProjectile* CTFGrenadeMirvProjectile::Create( const Vector &position, const QAngle &angles,
	const Vector &velocity, const AngularImpulse &angVelocity,
	CBaseCombatCharacter *pOwner, CBaseEntity *pWeapon )
{
	CTFGrenadeMirvProjectile *pGrenade = static_cast<CTFGrenadeMirvProjectile *>( CTFWeaponBaseGrenadeProj::Create( "tf_weapon_grenade_mirv_projectile", position, angles, velocity, angVelocity, pOwner, pWeapon ) );
	return pGrenade;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::Spawn()
{
	SetModel( GRENADE_MODEL );
	SetDetonateTimerLength( TF_MIRV_TIMER );

	BaseClass::Spawn();

	// Players need to be able to hit it with their weapons.
	AddSolidFlags( FSOLID_TRIGGER );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::Precache()
{
	PrecacheModel( GRENADE_MODEL );
	PrecacheScriptSound( MIRV_LEADIN_SOUND );
	PrecacheScriptSound( TF_MIRV_BLIP_SOUND );

	PrecacheTeamParticles( "MIRV_trail_%s" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFGrenadeMirvProjectile::OnTakeDamage( const CTakeDamageInfo &info )
{
	if ( !info.GetAttacker() )
		return 0;

	if ( info.GetAttacker()->GetTeamNumber() == GetTeamNumber() )
		return 0;

	// Wrench hit defuses the dynamite pack.
	if ( info.GetDamageCustom() == TF_DMG_WRENCH_FIX )
	{
		m_bDefused = true;
		EmitSound( "Weapon_Grenade_Mirv.Disarm" );
		StopParticleEffects( this );

		SetThink( &CBaseEntity::SUB_Remove );
		SetNextThink( gpGlobals->curtime + 5.0f );

		return 1;
	}

	return 0;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::BounceSound( void )
{
	EmitSound( "Weapon_Grenade_Mirv.Bounce" );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::Detonate()
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
void CTFGrenadeMirvProjectile::Explode( trace_t *pTrace, int bitsDamageType )
{
	// Pass through.
	BaseClass::Explode( pTrace, bitsDamageType );

	// Create the bomblets.
	for ( int iBomb = 0; iBomb < TF_WEAPON_GRENADE_MIRV_BOMB_COUNT; ++iBomb )
	{
		Vector vecSrc = pTrace->endpos + Vector( 0, 0, 1.0f ); 
		Vector vecVelocity( random->RandomFloat( -75.0f, 75.0f ) * 3.0f,
			                random->RandomFloat( -75.0f, 75.0f ) * 3.0f,
							random->RandomFloat( 30.0f, 70.0f ) * 5.0f );
		Vector vecZero( 0,0,0 );
		CTFPlayer *pPlayer = ToTFPlayer( GetThrower() );
		float flTime = 2.0f + random->RandomFloat( 0.0f, 1.0f );

		CTFGrenadeMirvBomb *pBomb = CTFGrenadeMirvBomb::Create( vecSrc, GetAbsAngles(), vecVelocity, vecZero, pPlayer, flTime );
		pBomb->SetDamage( GetDamage() * 0.5f );
		pBomb->SetDamageRadius( GetDamageRadius() );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::BlipSound( void )
{
	if ( GetDetonateTime() - gpGlobals->curtime <= MIRV_WARN_TIME )
	{
		if ( !m_bPlayedLeadIn )
		{
			EmitSound( MIRV_LEADIN_SOUND );
			m_bPlayedLeadIn = true;
		}
	}
	else if ( gpGlobals->curtime >= m_flNextBlipTime )
	{
		EmitSound( TF_MIRV_BLIP_SOUND );
		m_flNextBlipTime = gpGlobals->curtime + TF_MIRV_BLIP_FREQUENCY;
	}
}

#else

void CTFGrenadeMirvProjectile::OnDataChanged( DataUpdateType_t updateType )
{
	if ( updateType == DATA_UPDATE_CREATED )
	{
		CreateTrails();
	}
	else if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
	}
}

void CTFGrenadeMirvProjectile::CreateTrails( void )
{
	const char *pszParticle = ConstructTeamParticle( "MIRV_trail_%s", GetTeamNumber(), false );
	ParticleProp()->Create( pszParticle, PATTACH_ABSORIGIN_FOLLOW );
}

#endif

//=============================================================================
//
// TF Mirv Bomb functions (Server specific).
//

IMPLEMENT_NETWORKCLASS_ALIASED( TFGrenadeMirvBomb, DT_TFProjectile_MirvBomb );
BEGIN_NETWORK_TABLE( CTFGrenadeMirvBomb, DT_TFProjectile_MirvBomb )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_weapon_grenade_mirv_bomb, CTFGrenadeMirvBomb );
PRECACHE_WEAPON_REGISTER( tf_weapon_grenade_mirv_bomb );

#ifdef GAME_DLL

#define GRENADE_MODEL_BOMBLET "models/weapons/w_models/w_grenade_bomblet.mdl"

#define TF_WEAPON_GRENADE_MIRV_BOMB_GRAVITY		0.5f
#define TF_WEAPON_GRENADE_MIRV_BOMB_FRICTION	0.8f
#define TF_WEAPON_GRENADE_MIRV_BOMB_ELASTICITY	0.45f

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFGrenadeMirvBomb *CTFGrenadeMirvBomb::Create( const Vector &position, const QAngle &angles, const Vector &velocity, 
							                    const AngularImpulse &angVelocity, CBaseCombatCharacter *pOwner, float timer )
{
	CTFGrenadeMirvBomb *pBomb = static_cast<CTFGrenadeMirvBomb*>( CBaseEntity::Create( "tf_weapon_grenade_mirv_bomb", position, angles, pOwner ) );
	if ( pBomb )
	{
		pBomb->SetDetonateTimerLength( timer );
		pBomb->SetupInitialTransmittedGrenadeVelocity( velocity );
		pBomb->SetThrower( pOwner ); 
		pBomb->SetOwnerEntity( NULL );

		pBomb->SetGravity( TF_WEAPON_GRENADE_MIRV_BOMB_GRAVITY );
		pBomb->SetFriction( TF_WEAPON_GRENADE_MIRV_BOMB_GRAVITY );
		pBomb->SetElasticity( TF_WEAPON_GRENADE_MIRV_BOMB_ELASTICITY );

		// To be overriden.
		pBomb->m_flDamage = 180.0f;
		pBomb->m_DmgRadius = 198.0f;
		
		if ( pOwner )
			pBomb->ChangeTeam( pOwner->GetTeamNumber() );

		pBomb->SetCollisionGroup( TF_COLLISIONGROUP_GRENADES );

		IPhysicsObject *pPhysicsObject = pBomb->VPhysicsGetObject();
		if ( pPhysicsObject )
		{
			pPhysicsObject->AddVelocity( &velocity, &angVelocity );
		}
	}

	return pBomb;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvBomb::Spawn()
{
	SetModel( GRENADE_MODEL_BOMBLET );

	BaseClass::Spawn();

	EmitSound( "Weapon_Grenade_Mirv.Fuse" );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvBomb::Precache()
{
	PrecacheModel( GRENADE_MODEL_BOMBLET );
	PrecacheScriptSound( "Weapon_Grenade_Mirv.Fuse" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvBomb::UpdateOnRemove( void )
{
	StopSound( "Weapon_Grenade_Mirv.Fuse" );
	BaseClass::UpdateOnRemove();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvBomb::BounceSound( void )
{
	EmitSound( "Weapon_Grenade_MirvBomb.Bounce" );
}

#endif
