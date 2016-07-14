//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. ========//
//
// Purpose: Flare used by the flaregun.
//
//=============================================================================//
#include "cbase.h"
#include "tf_projectile_flare.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include "particles_new.h"
#else
#include "tf_player.h"
#include "tf_fx.h"
#endif

#define TF_WEAPON_FLARE_MODEL		"models/weapons/w_models/w_flaregun_shell.mdl"

BEGIN_DATADESC( CTFProjectile_Flare )
END_DATADESC()

LINK_ENTITY_TO_CLASS( tf_projectile_flare, CTFProjectile_Flare );
PRECACHE_REGISTER( tf_projectile_flare );

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Flare, DT_TFProjectile_Flare )
BEGIN_NETWORK_TABLE( CTFProjectile_Flare, DT_TFProjectile_Flare )
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFProjectile_Flare::CTFProjectile_Flare()
{
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFProjectile_Flare::~CTFProjectile_Flare()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#else
	m_bCollideWithTeammates = false;
#endif
}

#ifdef GAME_DLL
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::Precache()
{
	PrecacheModel( TF_WEAPON_FLARE_MODEL );

	PrecacheTeamParticles( "flaregun_trail_%s", false );
	PrecacheTeamParticles( "flaregun_trail_crit_%s", false );

	PrecacheScriptSound( "TFPlayer.FlareImpact" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::Spawn()
{
	SetModel( TF_WEAPON_FLARE_MODEL );
	BaseClass::Spawn();
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	SetGravity( 0.3f );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFProjectile_Flare::GetRocketSpeed( void )
{
	return 2000.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	// Save this entity as enemy, they will take 100% damage.
	m_hEnemy = pOther;

	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Pull out a bit.
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	// Damage.
	CBaseEntity *pAttacker = GetOwnerEntity();
	IScorer *pScorerInterface = dynamic_cast<IScorer*>( pAttacker );
	if ( pScorerInterface )
	{
		pAttacker = pScorerInterface->GetScorer();
	}

	// Play explosion sound and effect.
	Vector vecOrigin = GetAbsOrigin();
	CTFPlayer *pPlayer = ToTFPlayer( pOther );

	if ( pPlayer )
	{
		// Hit player, do impact sound.
		if ( pPlayer->m_Shared.InCond( TF_COND_BURNING ) )
		{
			// Jeez, hardcoding this doesn't seem like a good idea.
			m_bCritical = true;
		}
		
		CPVSFilter filter( vecOrigin );
		EmitSound( filter, pPlayer->entindex(), "TFPlayer.FlareImpact" );
	}
	else
	{
		// Hit world, do the explosion effect.
		CPVSFilter filter( vecOrigin );
		TE_TFExplosion( filter, 0.0f, vecOrigin, pTrace->plane.normal, GetWeaponID(), pOther->entindex(), ToBasePlayer( pAttacker ), GetTeamNumber(), m_bCritical );
	}

	CTakeDamageInfo info( this, pAttacker, m_hLauncher.Get(), GetDamage(), GetDamageType(), TF_DMG_CUSTOM_BURNING );
	info.SetReportedPosition( pAttacker ? pAttacker->GetAbsOrigin() : vec3_origin );
	pOther->TakeDamage( info );

	// Remove.
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Flare *CTFProjectile_Flare::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_Flare *pFlare = static_cast<CTFProjectile_Flare *>( CTFBaseRocket::Create( pWeapon, "tf_projectile_flare", vecOrigin, vecAngles, pOwner ) );

	if ( pFlare )
	{
		// Set scorer.
		pFlare->SetScorer( pScorer );
	}

	return pFlare;
}

#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	if ( updateType == DATA_UPDATE_CREATED )
	{
		CreateTrails();		
	}
	// Watch team changes and change trail accordingly.
	else if ( m_iOldTeamNum && m_iOldTeamNum != m_iTeamNum )
	{
		ParticleProp()->StopEmission();
		CreateTrails();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	const char *pszFormat = m_bCritical ? "flaregun_trail_crit_%s" : "flaregun_trail_%s";
	const char *pszEffectName = ConstructTeamParticle( pszFormat, GetTeamNumber(), false );

	ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );
}

#endif