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
#ifdef GAME_DLL
	SendPropBool( SENDINFO( m_bCritical ) ),
#else
	RecvPropBool( RECVINFO( m_bCritical ) ),
#endif
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

	PrecacheParticleSystem( "flaregun_trail_red" );
	PrecacheParticleSystem( "flaregun_trail_crit_red" );
	PrecacheParticleSystem( "flaregun_trail_blue" );
	PrecacheParticleSystem( "flaregun_trail_crit_blue" );
	PrecacheParticleSystem( "flaregun_trail_green" );
	PrecacheParticleSystem( "flaregun_trail_crit_green" );
	PrecacheParticleSystem( "flaregun_trail_yellow" );
	PrecacheParticleSystem( "flaregun_trail_crit_yellow" );
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
void CTFProjectile_Flare::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_Flare::GetScorer( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_Flare::GetDamageType() 
{ 
	int iDmgType = BaseClass::GetDamageType();
	if ( m_bCritical )
	{
		iDmgType |= DMG_CRITICAL;
	}

	return iDmgType;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
{
	// Get rocket's speed.
	float flVel = GetAbsVelocity().Length();

	QAngle angForward;
	VectorAngles( vecDir, angForward );

	// Now change rocket's direction.
	SetAbsAngles( angForward );
	SetAbsVelocity( vecDir * flVel );

	// And change owner.
	IncremenentDeflected();
	SetOwnerEntity( pDeflectedBy );
	ChangeTeam( pDeflectedBy->GetTeamNumber() );
	SetScorer( pDeflectedBy );
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
		// Hit player, do damage.
		CTakeDamageInfo info( this, pAttacker, m_hLauncher, GetDamage(), GetDamageType(), TF_DMG_CUSTOM_BURNING );
		info.SetReportedPosition( GetScorer()->GetAbsOrigin() );
		
		// Crit on burning players.
		// TODO: Once we implement attributes, change it to use them and move this to CTFPlayer::OnTakeDamage.
		if ( pPlayer->m_Shared.InCond( TF_COND_BURNING ) )
			info.AddDamageType( DMG_CRITICAL );

		pPlayer->TakeDamage( info );
		
		CPVSFilter filter( vecOrigin );
		EmitSound( filter, pPlayer->entindex(), "TFPlayer.FlareImpact" );
	}
	else
	{
		// Hit world, do the explosion effect.
		CPVSFilter filter( vecOrigin );
		TE_TFExplosion( filter, 0.0f, vecOrigin, pTrace->plane.normal, GetWeaponID(), pOther->entindex() );
	}

	// Remove.
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Flare *CTFProjectile_Flare::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_Flare *pFlare = static_cast<CTFProjectile_Flare*>( CBaseEntity::CreateNoSpawn( "tf_projectile_flare", vecOrigin, vecAngles, pOwner ) );

	if ( pFlare )
	{
		// Set team.
		pFlare->ChangeTeam( pOwner->GetTeamNumber() );

		// Set scorer.
		pFlare->SetScorer( pScorer );

		// Set firing weapon.
		pFlare->SetLauncher( pWeapon );

		// Initialize the owner.
		pFlare->SetOwnerEntity( pOwner );

		// Spawn.
		DispatchSpawn( pFlare );

		// Setup the initial velocity.
		Vector vecForward, vecRight, vecUp;
		AngleVectors( vecAngles, &vecForward, &vecRight, &vecUp );

		float flVelocity = 1100.0f;
		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flVelocity, mult_projectile_speed );

		Vector vecVelocity = vecForward * flVelocity;
		pFlare->SetAbsVelocity( vecVelocity );
		pFlare->SetupInitialTransmittedGrenadeVelocity( vecVelocity );

		// Setup the initial angles.
		QAngle angles;
		VectorAngles( vecVelocity, angles );
		pFlare->SetAbsAngles( angles );
		return pFlare;
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
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Flare::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	if ( m_bCritical )
	{
		switch( GetTeamNumber() )
		{
		case TF_TEAM_RED:
			ParticleProp()->Create( "flaregun_trail_crit_red", PATTACH_ABSORIGIN_FOLLOW );
			break;
		case TF_TEAM_BLUE:
			ParticleProp()->Create( "flaregun_trail_crit_blue", PATTACH_ABSORIGIN_FOLLOW );
			break;
		case TF_TEAM_GREEN:
			ParticleProp()->Create( "flaregun_trail_crit_green", PATTACH_ABSORIGIN_FOLLOW );
			break;
		case TF_TEAM_YELLOW:
			ParticleProp()->Create( "flaregun_trail_crit_yellow", PATTACH_ABSORIGIN_FOLLOW );
			break;
		default:
			ParticleProp()->Create( "flaregun_trail_crit_red", PATTACH_ABSORIGIN_FOLLOW );
			break;
		}
	}
	else
	{
		switch( GetTeamNumber() )
		{
		case TF_TEAM_RED:
			ParticleProp()->Create( "flaregun_trail_red", PATTACH_ABSORIGIN_FOLLOW );
			break;
		case TF_TEAM_BLUE:
			ParticleProp()->Create( "flaregun_trail_blue", PATTACH_ABSORIGIN_FOLLOW );
			break;
		case TF_TEAM_GREEN:
			ParticleProp()->Create( "flaregun_trail_green", PATTACH_ABSORIGIN_FOLLOW );
			break;
		case TF_TEAM_YELLOW:
			ParticleProp()->Create( "flaregun_trail_yellow", PATTACH_ABSORIGIN_FOLLOW );
			break;
		default:
			ParticleProp()->Create( "flaregun_trail_red", PATTACH_ABSORIGIN_FOLLOW );
			break;
		}
	}
}
#endif