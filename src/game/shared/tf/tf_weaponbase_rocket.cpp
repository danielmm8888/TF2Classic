//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Base Rockets.
//
//=============================================================================//
#include "cbase.h"
#include "tf_weaponbase_rocket.h"
#include "tf_gamerules.h"

// Server specific.
#ifdef GAME_DLL
#include "soundent.h"
#include "te_effect_dispatch.h"
#include "tf_fx.h"
#include "iscorer.h"
extern void SendProxy_Origin( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID );
extern void SendProxy_Angles( const SendProp *pProp, const void *pStruct, const void *pData, DVariant *pOut, int iElement, int objectID );
#endif

//=============================================================================
//
// TF Base Rocket tables.
//

IMPLEMENT_NETWORKCLASS_ALIASED( TFBaseRocket, DT_TFBaseRocket )

BEGIN_NETWORK_TABLE( CTFBaseRocket, DT_TFBaseRocket )
	// Client specific.
#ifdef CLIENT_DLL
	RecvPropVector( RECVINFO( m_vInitialVelocity ) ),

	RecvPropVector( RECVINFO_NAME( m_vecNetworkOrigin, m_vecOrigin ) ),
	RecvPropQAngles( RECVINFO_NAME( m_angNetworkAngles, m_angRotation ) ),

	RecvPropInt( RECVINFO( m_iDeflected ) ),
	RecvPropEHandle( RECVINFO( m_hLauncher ) ),

	RecvPropVector( RECVINFO( m_vecVelocity ), 0, RecvProxy_LocalVelocity ),

	// Server specific.
#else
	SendPropVector( SENDINFO( m_vInitialVelocity ), 12 /*nbits*/, 0 /*flags*/, -3000 /*low value*/, 3000 /*high value*/ ),

	SendPropExclude( "DT_BaseEntity", "m_vecOrigin" ),
	SendPropExclude( "DT_BaseEntity", "m_angRotation" ),

	SendPropVector( SENDINFO( m_vecOrigin ), -1, SPROP_COORD_MP_INTEGRAL | SPROP_CHANGES_OFTEN, 0.0f, HIGH_DEFAULT, SendProxy_Origin ),
	SendPropQAngles( SENDINFO( m_angRotation ), 6, SPROP_CHANGES_OFTEN, SendProxy_Angles ),

	SendPropInt( SENDINFO( m_iDeflected ), 4, SPROP_UNSIGNED ),
	SendPropEHandle( SENDINFO( m_hLauncher ) ),

	SendPropVector( SENDINFO( m_vecVelocity ), -1, SPROP_NOSCALE | SPROP_CHANGES_OFTEN ),
#endif
END_NETWORK_TABLE()

// Server specific.
#ifdef GAME_DLL
BEGIN_DATADESC( CTFBaseRocket )
	DEFINE_ENTITYFUNC( RocketTouch ),
	DEFINE_THINKFUNC( FlyThink ),
END_DATADESC()
#endif

ConVar tf_rocket_show_radius( "tf_rocket_show_radius", "0", FCVAR_REPLICATED | FCVAR_CHEAT /*| FCVAR_DEVELOPMENTONLY*/, "Render rocket radius." );
#ifdef GAME_DLL
ConVar tf2c_homing_rockets( "tf2c_homing_rockets", "0", FCVAR_CHEAT, "What is \"Rocket + x = Death\"?" );
ConVar tf2c_homing_deflected_rockets( "tf2c_homing_deflected_rockets", "0", FCVAR_CHEAT, "Homing Crit Rockets 2: Back with Vengeance" );
#endif

//=============================================================================
//
// Shared (client/server) functions.
//

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFBaseRocket::CTFBaseRocket()
{
	m_vInitialVelocity.Init();
	m_iDeflected = 0;
	m_hLauncher = NULL;

	// Client specific.
#ifdef CLIENT_DLL

	m_flSpawnTime = 0.0f;
	m_iOldTeamNum = TEAM_UNASSIGNED;

	// Server specific.
#else

	m_flDamage = 0.0f;

#endif
}

//-----------------------------------------------------------------------------
// Purpose: Destructor.
//-----------------------------------------------------------------------------
CTFBaseRocket::~CTFBaseRocket()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBaseRocket::Precache( void )
{
	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBaseRocket::Spawn( void )
{
	// Precache.
	Precache();

	// Client specific.
#ifdef CLIENT_DLL

	m_flSpawnTime = gpGlobals->curtime;
	BaseClass::Spawn();

	// Server specific.
#else

	//Derived classes must have set model.
	Assert( GetModel() );

	SetSolid( SOLID_BBOX );
	SetMoveType( MOVETYPE_FLY, MOVECOLLIDE_FLY_CUSTOM );
	AddEFlags( EFL_NO_WATER_VELOCITY_CHANGE );
	AddEffects( EF_NOSHADOW );

	SetCollisionGroup( TFCOLLISION_GROUP_ROCKETS );

	UTIL_SetSize( this, -Vector( 0, 0, 0 ), Vector( 0, 0, 0 ) );

	// Setup attributes.
	m_takedamage = DAMAGE_NO;
	SetGravity( 0.0f );

	// Setup the touch and think functions.
	SetTouch( &CTFBaseRocket::RocketTouch );
	SetThink( &CTFBaseRocket::FlyThink );
	SetNextThink( gpGlobals->curtime );

	// Don't collide with players on the owner's team for the first bit of our life
	m_flCollideWithTeammatesTime = gpGlobals->curtime + 0.25;
	m_bCollideWithTeammates = TFGameRules()->IsDeathmatch() ? true : false;

#endif
}

//=============================================================================
//
// Client specific functions.
//
#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBaseRocket::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

	m_iOldTeamNum = m_iTeamNum;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBaseRocket::PostDataUpdate( DataUpdateType_t type )
{
	// Pass through to the base class.
	BaseClass::PostDataUpdate( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		// Now stick our initial velocity and angles into the interpolation history.
		CInterpolatedVar<Vector> &interpolator = GetOriginInterpolator();
		interpolator.ClearHistory();

		CInterpolatedVar<QAngle> &rotInterpolator = GetRotationInterpolator();
		rotInterpolator.ClearHistory();

		float flChangeTime = GetLastChangeTime( LATCH_SIMULATION_VAR );

		// Add a sample 1 second back.
		Vector vCurOrigin = GetLocalOrigin() - m_vInitialVelocity;
		interpolator.AddToHead( flChangeTime - 1.0f, &vCurOrigin, false );

		QAngle vCurAngles = GetLocalAngles();
		rotInterpolator.AddToHead( flChangeTime - 1.0f, &vCurAngles, false );

		// Add the current sample.
		vCurOrigin = GetLocalOrigin();
		interpolator.AddToHead( flChangeTime, &vCurOrigin, false );

		rotInterpolator.AddToHead( flChangeTime - 1.0, &vCurAngles, false );
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFBaseRocket::DrawModel( int flags )
{
	// During the first 0.2 seconds of our life, don't draw ourselves.
	if ( gpGlobals->curtime - m_flSpawnTime < 0.2f )
		return 0;

	return BaseClass::DrawModel( flags );
}

void CTFBaseRocket::Simulate( void )
{
	// Make sure the rocket is facing movement direction.
	if ( GetMoveType() == MOVETYPE_FLYGRAVITY )
	{
		QAngle angForward;
		VectorAngles( GetAbsVelocity(), angForward );
		SetAbsAngles( angForward );
	}

	BaseClass::Simulate();
}

//=============================================================================
//
// Server specific functions.
//
#else

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFBaseRocket *CTFBaseRocket::Create( CBaseEntity *pWeapon, const char *pszClassname, const Vector &vecOrigin,
	const QAngle &vecAngles, CBaseEntity *pOwner )
{
	CTFBaseRocket *pRocket = static_cast<CTFBaseRocket*>( CBaseEntity::CreateNoSpawn( pszClassname, vecOrigin, vecAngles, pOwner ) );
	if ( !pRocket )
		return NULL;

	// Set firing weapon.
	pRocket->SetLauncher( pWeapon );

	// Spawn.
	pRocket->Spawn();

	float flGravity = 0.0f;
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flGravity, mod_rocket_gravity );
	if ( flGravity )
	{
		pRocket->SetMoveType( MOVETYPE_FLYGRAVITY, MOVECOLLIDE_FLY_CUSTOM );
		pRocket->SetGravity( flGravity );
	}

	// Setup the initial velocity.
	Vector vecForward, vecRight, vecUp;
	AngleVectors( vecAngles, &vecForward, &vecRight, &vecUp );

	float flVelocity = 1100.0f;
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( pWeapon, flVelocity, mult_projectile_speed );

	Vector vecVelocity = vecForward * flVelocity;
	pRocket->SetAbsVelocity( vecVelocity );
	pRocket->SetupInitialTransmittedGrenadeVelocity( vecVelocity );

	// Setup the initial angles.
	QAngle angles;
	VectorAngles( vecVelocity, angles );
	pRocket->SetAbsAngles( angles );

	// Set team.
	pRocket->ChangeTeam( pOwner->GetTeamNumber() );

	return pRocket;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBaseRocket::RocketTouch( CBaseEntity *pOther )
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

	trace_t trace;
	memcpy( &trace, pTrace, sizeof( trace_t ) );
	Explode( &trace, pOther );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
unsigned int CTFBaseRocket::PhysicsSolidMaskForEntity( void ) const
{
	int teamContents = 0;

	if ( m_bCollideWithTeammates == false )
	{
		// Only collide with the other team

		switch ( GetTeamNumber() )
		{
		case TF_TEAM_RED:
			teamContents = CONTENTS_BLUETEAM | CONTENTS_GREENTEAM | CONTENTS_YELLOWTEAM;
			break;

		case TF_TEAM_BLUE:
			teamContents = CONTENTS_REDTEAM | CONTENTS_GREENTEAM | CONTENTS_YELLOWTEAM;
			break;

		case TF_TEAM_GREEN:
			teamContents = CONTENTS_REDTEAM | CONTENTS_BLUETEAM | CONTENTS_YELLOWTEAM;
			break;

		case TF_TEAM_YELLOW:
			teamContents = CONTENTS_REDTEAM | CONTENTS_BLUETEAM | CONTENTS_GREENTEAM;
			break;
		}
	}
	else
	{
		// Collide with all teams
		teamContents = CONTENTS_REDTEAM | CONTENTS_BLUETEAM | CONTENTS_GREENTEAM | CONTENTS_YELLOWTEAM;
	}

	return BaseClass::PhysicsSolidMaskForEntity() | teamContents;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFBaseRocket::Explode( trace_t *pTrace, CBaseEntity *pOther )
{
	// Save this entity as enemy, they will take 100% damage.
	m_hEnemy = pOther;

	// Invisible.
	SetModelName( NULL_STRING );
	AddSolidFlags( FSOLID_NOT_SOLID );
	m_takedamage = DAMAGE_NO;

	// Figure out Econ ID.
	int iItemID = -1;
	if ( m_hLauncher.Get() )
	{
		CTFWeaponBase *pWeapon = dynamic_cast<CTFWeaponBase *>( m_hLauncher.Get() );
		if ( pWeapon )
		{
			iItemID = pWeapon->GetItemID();
		}
	}

	// Pull out a bit.
	if ( pTrace->fraction != 1.0 )
	{
		SetAbsOrigin( pTrace->endpos + ( pTrace->plane.normal * 1.0f ) );
	}

	// Play explosion sound and effect.
	Vector vecOrigin = GetAbsOrigin();
	CPVSFilter filter( vecOrigin );
	TE_TFExplosion( filter, 0.0f, vecOrigin, pTrace->plane.normal, GetWeaponID(), pOther->entindex(), iItemID );
	CSoundEnt::InsertSound( SOUND_COMBAT, vecOrigin, 1024, 3.0 );

	// Damage.
	CBaseEntity *pAttacker = GetOwnerEntity();
	IScorer *pScorerInterface = dynamic_cast<IScorer*>( pAttacker );
	if ( pScorerInterface )
	{
		pAttacker = pScorerInterface->GetScorer();
	}

	float flRadius = GetRadius();

	CTFRadiusDamageInfo radiusInfo;
	radiusInfo.info.Set( this, pAttacker, m_hLauncher, vec3_origin, vecOrigin, GetDamage(), GetDamageType() );
	radiusInfo.m_vecSrc = vecOrigin;
	radiusInfo.m_flRadius = flRadius;
	radiusInfo.m_flSelfDamageRadius = 121.0f; // Original rocket radius?

	TFGameRules()->RadiusDamage( radiusInfo );

	// Debug!
	if ( tf_rocket_show_radius.GetBool() )
	{
		DrawRadius( flRadius );
	}

	// Don't decal players with scorch.
	if ( !pOther->IsPlayer() )
	{
		UTIL_DecalTrace( pTrace, "Scorch" );
	}

	// Remove the rocket.
	UTIL_Remove( this );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CTFBaseRocket::GetRadius( void )
{
	float flRadius = TF_ROCKET_RADIUS;
	CALL_ATTRIB_HOOK_FLOAT_ON_OTHER( m_hLauncher.Get(), flRadius, mult_explosion_radius );
	return flRadius;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFBaseRocket::DrawRadius( float flRadius )
{
	Vector pos = GetAbsOrigin();
	int r = 255;
	int g = 0, b = 0;
	float flLifetime = 10.0f;
	bool bDepthTest = true;

	Vector edge, lastEdge;
	NDebugOverlay::Line( pos, pos + Vector( 0, 0, 50 ), r, g, b, !bDepthTest, flLifetime );

	lastEdge = Vector( flRadius + pos.x, pos.y, pos.z );
	float angle;
	for ( angle = 0.0f; angle <= 360.0f; angle += 22.5f )
	{
		edge.x = flRadius * cos( angle ) + pos.x;
		edge.y = pos.y;
		edge.z = flRadius * sin( angle ) + pos.z;

		NDebugOverlay::Line( edge, lastEdge, r, g, b, !bDepthTest, flLifetime );

		lastEdge = edge;
	}

	lastEdge = Vector( pos.x, flRadius + pos.y, pos.z );
	for ( angle = 0.0f; angle <= 360.0f; angle += 22.5f )
	{
		edge.x = pos.x;
		edge.y = flRadius * cos( angle ) + pos.y;
		edge.z = flRadius * sin( angle ) + pos.z;

		NDebugOverlay::Line( edge, lastEdge, r, g, b, !bDepthTest, flLifetime );

		lastEdge = edge;
	}

	lastEdge = Vector( pos.x, flRadius + pos.y, pos.z );
	for ( angle = 0.0f; angle <= 360.0f; angle += 22.5f )
	{
		edge.x = flRadius * cos( angle ) + pos.x;
		edge.y = flRadius * sin( angle ) + pos.y;
		edge.z = pos.z;

		NDebugOverlay::Line( edge, lastEdge, r, g, b, !bDepthTest, flLifetime );

		lastEdge = edge;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Increment deflects counter
//-----------------------------------------------------------------------------
void CTFBaseRocket::IncremenentDeflected( void )
{
	m_iDeflected++;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFBaseRocket::SetLauncher( CBaseEntity *pLauncher )
{
	m_hLauncher = pLauncher;
}

void CTFBaseRocket::FlyThink( void )
{
	if ( gpGlobals->curtime > m_flCollideWithTeammatesTime && m_bCollideWithTeammates == false )
	{
		m_bCollideWithTeammates = true;
	}

	if ( tf2c_homing_rockets.GetBool() || ( tf2c_homing_deflected_rockets.GetBool() && m_iDeflected ) )
	{
		// Find the closest visible enemy player.
		CUtlVector<CTFPlayer *> vecPlayers;
		int count = CollectPlayers( &vecPlayers, TEAM_ANY, true );
		float flClosest = FLT_MAX;
		Vector vecClosestTarget = vec3_origin;

		for ( int i = 0; i < count; i++ )
		{
			CTFPlayer *pPlayer = vecPlayers[i];
			if ( pPlayer == GetOwnerEntity() )
				continue;

			if ( pPlayer->GetTeamNumber() == GetTeamNumber() && !TFGameRules()->IsDeathmatch() )
				continue;

			Vector vecTarget;
			QAngle angTarget;
			if ( GetWeaponID() == TF_WEAPON_COMPOUND_BOW )
			{
				int iBone = pPlayer->LookupBone( "bip_head" );
				pPlayer->GetBonePosition( iBone, vecTarget, angTarget );;
			}
			else
			{
				vecTarget = pPlayer->EyePosition();
			}

			if ( FVisible( vecTarget ) )
			{
				float flDistSqr = ( vecTarget - GetAbsOrigin() ).LengthSqr();
				if ( flDistSqr < flClosest )
				{
					flClosest = flDistSqr;
					vecClosestTarget = vecTarget;
				}
			}
		}

		// Head towards him.
		if ( vecClosestTarget != vec3_origin )
		{
			Vector vecTarget = vecClosestTarget;
			Vector vecDir = vecTarget - GetAbsOrigin();
			VectorNormalize( vecDir );

			float flSpeed = GetAbsVelocity().Length();
			QAngle angForward;
			VectorAngles( vecDir, angForward );
			SetAbsAngles( angForward );
			SetAbsVelocity( vecDir * flSpeed );
		}
	}

	SetNextThink( gpGlobals->curtime + 0.1f );
}

#endif
