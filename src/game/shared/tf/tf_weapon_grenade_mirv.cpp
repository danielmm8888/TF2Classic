//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Mirv Grenade.
//
//=============================================================================//
#include "cbase.h"
#include "tf_weaponbase.h"
#include "tf_gamerules.h"
#include "npcevent.h"
#include "engine/IEngineSound.h"
#include "tf_weapon_grenade_mirv.h"

// Server specific.
#ifdef GAME_DLL
#include "tf_player.h"
#include "items.h"
#include "tf_weaponbase_grenadeproj.h"
#include "soundent.h"
#include "KeyValues.h"
#endif

#define GRENADE_MIRV_TIMER	3.0f // seconds

#define MIRV_BLIP_FREQUENCY			1.0f
#define MIRV_BLIP_FAST_FREQUENCY	0.3f
#define MIRV_WARN_TIME				1.0f
#define MIRV_BLIP_SOUND				"Weapon_Grenade_Mirv.Timer"

//=============================================================================
//
// TF Mirv Grenade Projectile functions (Server specific).
//
#ifdef GAME_DLL

BEGIN_DATADESC( CTFGrenadeMirvProjectile )
	DEFINE_THINKFUNC( DetonateThink ),
END_DATADESC()

#define GRENADE_MODEL "models/weapons/w_models/w_grenade_mirv.mdl"

LINK_ENTITY_TO_CLASS( tf_weapon_grenade_mirv_projectile, CTFGrenadeMirvProjectile );
PRECACHE_WEAPON_REGISTER( tf_weapon_grenade_mirv_projectile );

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFGrenadeMirvProjectile* CTFGrenadeMirvProjectile::Create( const Vector &position, const QAngle &angles, 
															const Vector &velocity, const AngularImpulse &angVelocity, 
															CBaseCombatCharacter *pOwner, const CTFWeaponInfo &weaponInfo, float timer, int iFlags )
{
	CTFGrenadeMirvProjectile *pGrenade = static_cast<CTFGrenadeMirvProjectile*>( CTFWeaponBaseGrenadeProj::Create( "tf_weapon_grenade_mirv_projectile", position, angles, velocity, angVelocity, pOwner, weaponInfo, timer, iFlags ) );
	return pGrenade;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::Spawn()
{
	SetModel( GRENADE_MODEL );

	BaseClass::Spawn();

	SetDetonateTimerLength( GRENADE_MIRV_TIMER );
	BlipSound();
	m_flNextBlipTime = gpGlobals->curtime + MIRV_BLIP_FREQUENCY;

	SetThink( &CTFGrenadeMirvProjectile::DetonateThink );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::Precache()
{
	PrecacheModel( GRENADE_MODEL );
	PrecacheScriptSound( "Weapon_Grenade_Mirv.Fuse" );
	PrecacheScriptSound( "Weapon_Grenade_Mirv.Timer" );

	BaseClass::Precache();
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
void CTFGrenadeMirvProjectile::DetonateThink( void )
{
	if ( gpGlobals->curtime > m_flNextBlipTime )
	{
		BlipSound();

		if ( GetDetonateTime() - gpGlobals->curtime <= MIRV_WARN_TIME )
		{
			m_flNextBlipTime = gpGlobals->curtime + MIRV_BLIP_FAST_FREQUENCY;
		}
		else
		{
			m_flNextBlipTime = gpGlobals->curtime + MIRV_BLIP_FREQUENCY;
		}
	}

	BaseClass::DetonateThink();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::Explode( trace_t *pTrace, int bitsDamageType )
{
	// Pass through.
	BaseClass::Explode( pTrace, bitsDamageType );
// Server specific.
#ifdef GAME_DLL

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

#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvProjectile::BlipSound( void )
{
	EmitSound_t params;
	params.m_pSoundName = MIRV_BLIP_SOUND;

	float flTime = GetDetonateTime() - gpGlobals->curtime;
	if ( flTime <= MIRV_WARN_TIME )
	{
		params.m_nPitch = RemapValClamped( flTime, 0.0f, 1.0f, 125, 100 );
		params.m_nFlags |= SND_CHANGE_PITCH;
	}

	CPASAttenuationFilter filter( this, MIRV_BLIP_SOUND );

	EmitSound( filter, entindex(), params );
}

//=============================================================================
//
// TF Mirv Bomb functions (Server specific).
//

#define GRENADE_MODEL_BOMBLET "models/weapons/w_models/w_grenade_bomblet.mdl"

#define TF_WEAPON_GRENADE_MIRV_BOMB_GRAVITY		0.5f
#define TF_WEAPON_GRENADE_MIRV_BOMB_FRICTION	0.8f
#define TF_WEAPON_GRENADE_MIRV_BOMB_ELASTICITY	0.45f

LINK_ENTITY_TO_CLASS( tf_weapon_grenade_mirv_bomb, CTFGrenadeMirvBomb );
PRECACHE_WEAPON_REGISTER( tf_weapon_grenade_mirv_bomb );

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

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvBomb::Detonate( void )
{
	StopSound( "Weapon_Grenade_Mirv.Fuse" );
	BaseClass::Detonate();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFGrenadeMirvBomb::BounceSound( void )
{
	EmitSound( "Weapon_Grenade_MirvBomb.Bounce" );
}

#endif
