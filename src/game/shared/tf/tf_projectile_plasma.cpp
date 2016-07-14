//=============================================================================//
//
// Purpose: Plasma blast used by MLG.
//
//=============================================================================//
#include "cbase.h"
#include "tf_projectile_plasma.h"

#ifdef CLIENT_DLL
#include "c_tf_player.h"
#else
#include "tf_player.h"
#include "tf_fx.h"
#include "tf_gamerules.h"
#endif

#define TF_WEAPON_PLASMA_MODEL "models/weapons/w_models/w_rocket.mdl"

ConVar tf_debug_plasma( "tf_debug_plasma", "0", FCVAR_CHEAT );

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Plasma, DT_TFProjectile_Plasma );
BEGIN_NETWORK_TABLE( CTFProjectile_Plasma, DT_TFProjectile_Plasma )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_projectile_plasma, CTFProjectile_Plasma );
PRECACHE_REGISTER( tf_projectile_plasma );

CTFProjectile_Plasma::CTFProjectile_Plasma()
{
}

CTFProjectile_Plasma::~CTFProjectile_Plasma()
{
#ifdef GAME_DLL
	m_bCollideWithTeammates = false;
#else
	ParticleProp()->StopEmission();
#endif
}

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Plasma *CTFProjectile_Plasma::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_Plasma *pRocket = static_cast<CTFProjectile_Plasma*>( CTFBaseRocket::Create( pWeapon, "tf_projectile_plasma", vecOrigin, vecAngles, pOwner ) );

	if ( pRocket )
	{
		pRocket->SetScorer( pScorer );
	}

	return pRocket;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Plasma::Spawn()
{
	SetModel( TF_WEAPON_PLASMA_MODEL );
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Plasma::Precache()
{
	PrecacheModel( TF_WEAPON_PLASMA_MODEL );

	PrecacheTeamParticles( "critical_rocket_%s", true );
	PrecacheParticleSystem( "rockettrail" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Plasma::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	BaseClass::Explode( pTrace, pOther );

	// Create the bomblets.
	const int nBombs = 20;
	const float flStep = 360.0f / (float)nBombs;

	for ( int i = 0; i < nBombs; i++ )
	{
		CTFProjectile_PlasmaBomblet *pBomb = CTFProjectile_PlasmaBomblet::Create( m_hLauncher.Get(), GetAbsOrigin(), GetAbsAngles(), GetOwnerEntity(), GetScorer() );

		// FIXME: Need to throw them outwards related to impact plane.
		Vector vecDir;
		QAngle angles( -45.0f, (float)i * flStep, 0.0f );
		AngleVectors( angles, &vecDir );

		// Pull it out a bit so it doesn't collide with other bomblets.
		pBomb->SetAbsOrigin( GetAbsOrigin() + vecDir * 1.0f );
		pBomb->SetAbsVelocity( vecDir * RandomFloat( 250, 750 ) );
		pBomb->SetAbsAngles( angles );

		if ( tf_debug_plasma.GetBool() )
		{
			NDebugOverlay::Line( pBomb->GetAbsOrigin(), pBomb->GetAbsOrigin() + vecDir * 50.0f, 0, 255, 0, true, 10.0f );
		}

		pBomb->SetDamage( GetDamage() / 3 );
		pBomb->SetCritical( m_bCritical );
	}
}

#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_Plasma::OnDataChanged( DataUpdateType_t updateType )
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
void CTFProjectile_Plasma::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	if ( enginetrace->GetPointContents( GetAbsOrigin() ) & MASK_WATER )
	{
		ParticleProp()->Create( "rockettrail_underwater", PATTACH_POINT_FOLLOW, "trail" );
	}
	else
	{
		ParticleProp()->Create( "rockettrail", PATTACH_POINT_FOLLOW, "trail" );
	}

	if ( m_bCritical )
	{
		const char *pszEffectName = ConstructTeamParticle( "critical_rocket_%s", GetTeamNumber(), true );
		CNewParticleEffect *pParticle = ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );

		C_TFPlayer *pPlayer = ToTFPlayer( GetOwnerEntity() );
		if ( pPlayer )
		{
			pPlayer->m_Shared.SetParticleToMercColor( pParticle );
		}
	}
}

#endif

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_PlasmaBomblet, DT_TFProjectile_PlasmaBomblet );
BEGIN_NETWORK_TABLE( CTFProjectile_PlasmaBomblet, DT_TFProjectile_PlasmaBomblet )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_projectile_plasma_bomblet, CTFProjectile_PlasmaBomblet );
PRECACHE_REGISTER( tf_projectile_plasma_bomblet );

#define TF_WEAPON_PLASMA_BOMBLET_MODEL "models/weapons/w_models/w_flaregun_shell.mdl"

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_PlasmaBomblet *CTFProjectile_PlasmaBomblet::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_PlasmaBomblet *pRocket = static_cast<CTFProjectile_PlasmaBomblet*>( CTFBaseRocket::Create( pWeapon, "tf_projectile_plasma_bomblet", vecOrigin, vecAngles, pOwner ) );

	if ( pRocket )
	{
		pRocket->SetScorer( pScorer );
	}

	return pRocket;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_PlasmaBomblet::Precache()
{
	PrecacheModel( TF_WEAPON_PLASMA_BOMBLET_MODEL );

	PrecacheTeamParticles( "flaregun_trail_%s", false );
	PrecacheTeamParticles( "flaregun_trail_crit_%s", false );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_PlasmaBomblet::Spawn( void )
{
	SetModel( TF_WEAPON_PLASMA_BOMBLET_MODEL );
	BaseClass::Spawn();
	SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
	SetGravity( 1.0f );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_PlasmaBomblet::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	// Need to think of something here.
	BaseClass::Explode( pTrace, pOther );
}

#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFProjectile_PlasmaBomblet::OnDataChanged( DataUpdateType_t updateType )
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
void CTFProjectile_PlasmaBomblet::CreateTrails( void )
{
	if ( IsDormant() )
		return;

	const char *pszFormat = m_bCritical ? "flaregun_trail_crit_%s" : "flaregun_trail_%s";
	const char *pszEffectName = ConstructTeamParticle( pszFormat, GetTeamNumber(), false );

	ParticleProp()->Create( pszEffectName, PATTACH_ABSORIGIN_FOLLOW );
}

#endif
