//=============================================================================//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "tf_projectile_arrow.h"

#ifdef GAME_DLL
#include "spritetrail.h"
#include "props_shared.h"
#include "tf_player.h"
#include "debugoverlay_shared.h"
#endif

ConVar tf_debug_arrows( "tf_debug_arrows", "0", FCVAR_CHEAT );

#define TF_ARROW_FIX

const char *g_pszArrowModels[] =
{
	"models/weapons/w_models/w_arrow.mdl",
	"models/weapons/w_models/w_syringe_proj.mdl",
	"models/weapons/w_models/w_repair_claw.mdl",
	//"models/weapons/w_models/w_arrow_xmas.mdl",
};

IMPLEMENT_NETWORKCLASS_DT( CTFProjectile_Arrow, DT_ProjectileArrow )
#ifdef CLIENT_DLL
	RecvPropBool( RECVINFO( m_bCritical ) ),
	RecvPropInt( RECVINFO( m_iType ) ),
#else
	SendPropBool( SENDINFO( m_bCritical ) ),
	SendPropInt( SENDINFO( m_iType ), 3, SPROP_UNSIGNED ),
#endif
END_NETWORK_TABLE()

#ifdef GAME_DLL
BEGIN_DATADESC( CTFProjectile_Arrow )
	DEFINE_ENTITYFUNC( ArrowTouch )
END_DATADESC()
#endif

LINK_ENTITY_TO_CLASS( tf_projectile_arrow, CTFProjectile_Arrow );
PRECACHE_REGISTER( tf_projectile_arrow );

CTFProjectile_Arrow::CTFProjectile_Arrow()
{
}

CTFProjectile_Arrow::~CTFProjectile_Arrow()
{
#ifdef CLIENT_DLL
	ParticleProp()->StopEmission();
#else
	m_bCollideWithTeammates = false;
#endif
}

#ifdef GAME_DLL

CTFProjectile_Arrow *CTFProjectile_Arrow::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, float flSpeed, float flGravity, CBaseEntity *pOwner, CBaseEntity *pScorer, int iType )
{
	CTFProjectile_Arrow *pArrow = static_cast<CTFProjectile_Arrow *>( CBaseEntity::CreateNoSpawn( "tf_projectile_arrow", vecOrigin, vecAngles, pOwner ) );

	if ( pArrow )
	{
		// Set team.
		pArrow->ChangeTeam( pOwner->GetTeamNumber() );

		// Set scorer.
		pArrow->SetScorer( pScorer );

		// Set firing weapon.
		pArrow->SetLauncher( pWeapon );

		// Set arrow type.
		pArrow->SetType( iType );

		// Spawn.
		DispatchSpawn( pArrow );

		// Setup the initial velocity.
		Vector vecForward, vecRight, vecUp;
		AngleVectors( vecAngles, &vecForward, &vecRight, &vecUp );

		CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flSpeed, mult_projectile_speed );

		Vector vecVelocity = vecForward * flSpeed;
		pArrow->SetAbsVelocity( vecVelocity );
		pArrow->SetupInitialTransmittedGrenadeVelocity( vecVelocity );

		// Setup the initial angles.
		QAngle angles;
		VectorAngles( vecVelocity, angles );
		pArrow->SetAbsAngles( angles );

		pArrow->SetGravity( flGravity );

		return pArrow;
	}

	return pArrow;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::Precache( void )
{
	// Precache all arrow models we have.
	for ( int i = 0; i < ARRAYSIZE( g_pszArrowModels ); i++ )
	{
		int iIndex = PrecacheModel( g_pszArrowModels[i] );
		PrecacheGibsForModel( iIndex );
	}

	for ( int i = FIRST_GAME_TEAM; i < TF_TEAM_COUNT; i++ )
	{
		PrecacheModel( ConstructTeamParticle( "effects/arrowtrail_%s.vmt", i, false, g_aTeamNamesShort ) );
		PrecacheModel( ConstructTeamParticle( "effects/healingtrail_%s.vmt", i, false, g_aTeamNamesShort ) );
		PrecacheModel( ConstructTeamParticle( "effects/repair_claw_trail_%s.vmt", i, false, g_aTeamParticleNames ) );
	}

	PrecacheScriptSound( "Weapon_Arrow.ImpactFlesh" );
	PrecacheScriptSound( "Weapon_Arrow.ImpactMetal" );
	PrecacheScriptSound( "Weapon_Arrow.ImpactWood" );
	PrecacheScriptSound( "Weapon_Arrow.ImpactConcrete" );
	PrecacheScriptSound( "Weapon_Arrow.Nearmiss" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::Spawn( void )
{
	switch ( m_iType )
	{
	case TF_PROJECTILE_BUILDING_REPAIR_BOLT:
		SetModel( g_pszArrowModels[2] );
		break;
	case TF_PROJECTILE_HEALING_BOLT:
	case TF_PROJECTILE_FESTITIVE_HEALING_BOLT:
		SetModel( g_pszArrowModels[1] );
		break;
	default:
		SetModel( g_pszArrowModels[0] );
		break;
	}

	BaseClass::Spawn();

#ifdef TF_ARROW_FIX
	SetSolidFlags( FSOLID_NOT_SOLID | FSOLID_TRIGGER );
#endif

	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	SetGravity( 0.3f ); // TODO: Check again later.

	UTIL_SetSize( this, -Vector( 1, 1, 1 ), Vector( 1, 1, 1 ) );

	CreateTrail();

	SetTouch( &CTFProjectile_Arrow::ArrowTouch );

	// TODO: Set skin here...
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_Arrow::GetScorer( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::ArrowTouch( CBaseEntity *pOther )
{
	// Verify a correct "other."
	Assert( pOther );
	if ( pOther->IsSolidFlagSet( FSOLID_TRIGGER | FSOLID_VOLUME_CONTENTS ) )
		return;

	// Handle hitting skybox (disappear).
	const trace_t *pTrace = &CBaseEntity::GetTouchTrace();
	if ( pTrace->surface.flags & SURF_SKY )
	{
		UTIL_Remove( this );
		return;
	}

	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Damage.
	CBaseEntity *pAttacker = GetOwnerEntity();
	IScorer *pScorerInterface = dynamic_cast<IScorer*>( pAttacker );
	if ( pScorerInterface )
	{
		pAttacker = pScorerInterface->GetScorer();
	}

	Vector vecOrigin = GetAbsOrigin();
	Vector vecDir = GetAbsVelocity();
	CTFPlayer *pPlayer = ToTFPlayer( pOther );
	CTFWeaponBase *pWeapon = dynamic_cast<CTFWeaponBase *>( m_hLauncher.Get() );

	if ( pPlayer )
	{
#ifndef TF_ARROW_FIX
		CStudioHdr *pStudioHdr = pPlayer->GetModelPtr();
		if ( !pStudioHdr )
			return;

		mstudiohitboxset_t *set = pStudioHdr->pHitboxSet( pPlayer->GetHitboxSet() );
		if ( !set )
			return;

		Vector vecDir = GetAbsVelocity();
		VectorNormalize( vecDir );

		// Oh boy... we gotta figure out the closest hitbox on player model to land a hit on.
		// Trace a bit ahead, to get closer to player's body.
		trace_t trFly;
		UTIL_TraceLine( vecOrigin, vecOrigin + vecDir * 16.0f, MASK_SHOT, this, COLLISION_GROUP_NONE, &trFly );

		QAngle angHit;
		trace_t trHit;
		float flClosest = FLT_MAX;
		for ( int i = 0; i < set->numhitboxes; i++ )
		{
			mstudiobbox_t *pBox = set->pHitbox( i );

			Vector boxPosition;
			QAngle boxAngles;
			pPlayer->GetBonePosition( pBox->bone, boxPosition, boxAngles );

			trace_t tr;
			UTIL_TraceLine( trFly.endpos, boxPosition, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
			float flLengthSqr = ( tr.endpos - trFly.endpos ).LengthSqr();

			if ( flLengthSqr < flClosest )
			{
				flClosest = flLengthSqr;
				trHit = tr;
			}
		}


		if ( tf_debug_arrows.GetBool() )
		{
			NDebugOverlay::Line( trHit.endpos, trFly.endpos, 0, 255, 0, true, 5.0f );
		}

		// Place arrow at hitbox.
		SetAbsOrigin( trHit.endpos );

		Vector vecHitDir = trHit.plane.normal * -1.0f;
		AngleVectors( angHit, &vecHitDir );
		SetAbsAngles( angHit );
#else
		// Trace ahead to see if we're going to hit player's hitbox.
		trace_t trHit;
		UTIL_TraceLine( vecOrigin, vecOrigin + vecDir * gpGlobals->frametime, MASK_SHOT, this, COLLISION_GROUP_NONE, &trHit );
		if ( trHit.m_pEnt != pOther ) // Didn't hit, keep going.
			return;
#endif

		// Do damage.
		CTakeDamageInfo info( this, pAttacker, pWeapon, GetDamage(), GetDamageType() | DMG_PREVENT_PHYSICS_FORCE );
		CalculateBulletDamageForce( &info, pWeapon ? pWeapon->GetTFWpnData().iAmmoType : 0, vecDir, vecOrigin );
		info.SetReportedPosition( pAttacker ? pAttacker->GetAbsOrigin() : vec3_origin );

		pPlayer->DispatchTraceAttack( info, vecDir, &trHit );
		ApplyMultiDamage();

		pPlayer->EmitSound( "Weapon_Arrow.ImpactFlesh" );
	}
	else if ( pOther->IsBaseObject() )
	{
		CTakeDamageInfo info( this, pAttacker, pWeapon, GetDamage(), GetDamageType() | DMG_PREVENT_PHYSICS_FORCE );
		CalculateBulletDamageForce( &info, pWeapon->GetTFWpnData().iAmmoType, vecDir, vecOrigin );
		
		pOther->TakeDamage( info );
		pOther->EmitSound( "Weapon_Arrow.ImpactMetal" );
	}
	else
	{
		EmitSound( "Weapon_Arrow.ImpactConcrete" );
	}

	// Remove.
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_Arrow::GetDamageType()
{
	int iDmgType = BaseClass::GetDamageType();
	if ( m_bCritical )
	{
		iDmgType |= DMG_CRITICAL;
	}

	return iDmgType;
}

const char *CTFProjectile_Arrow::GetTrailParticleName( void )
{
	const char *pszFormat = NULL;
	bool bLongTeamName = false;

	switch( m_iType )
	{
	case TF_PROJECTILE_BUILDING_REPAIR_BOLT:
		pszFormat = "effects/repair_claw_trail_%s.vmt";
		bLongTeamName = true;
		break;
	case TF_PROJECTILE_HEALING_BOLT:
	case TF_PROJECTILE_FESTITIVE_HEALING_BOLT:
		pszFormat = "effects/healingtrail_%s.vmt";
		break;
	default:
		pszFormat = "effects/arrowtrail_%s.vmt";
		break;
	}

	return ConstructTeamParticle( pszFormat, GetTeamNumber(), false, bLongTeamName ? g_aTeamParticleNames : g_aTeamNamesShort );
}

// ---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::CreateTrail( void )
{
	CSpriteTrail *pTrail = CSpriteTrail::SpriteTrailCreate( GetTrailParticleName(), GetAbsOrigin(), true );

	if ( pTrail )
	{
		pTrail->FollowEntity( this );
		pTrail->SetTransparency( kRenderTransAlpha, -1, -1, -1, 255, kRenderFxNone );
		pTrail->SetStartWidth( m_iType == TF_PROJECTILE_BUILDING_REPAIR_BOLT ? 5.0f : 3.0f );
		pTrail->SetTextureResolution( 0.01f );
		pTrail->SetLifeTime( 0.3f );
		pTrail->TurnOn();

		pTrail->SetContextThink( &CBaseEntity::SUB_Remove, gpGlobals->curtime + 3.0f, "RemoveThink" );

		m_hSpriteTrail.Set( pTrail );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Arrow::UpdateOnRemove( void )
{
	UTIL_Remove( m_hSpriteTrail.Get() );

	BaseClass::UpdateOnRemove();
}

#endif
