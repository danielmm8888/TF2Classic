//====== Copyright © 1996-2006, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "world.h"
#include "tf_generic_bomb.h"
#include "tf_gamerules.h"
#include "te_particlesystem.h"
#include "particle_parse.h"
#include "tf_fx.h"
#include "tf_player.h"
#include "baseanimating.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

LINK_ENTITY_TO_CLASS( tf_generic_bomb, CTFGenericBomb );

BEGIN_DATADESC( CTFGenericBomb )	
	DEFINE_KEYFIELD( m_flDamage,		FIELD_FLOAT,		"damage" ),
	DEFINE_KEYFIELD( m_flRadius,		FIELD_FLOAT,		"radius" ),
	DEFINE_KEYFIELD( m_iHealth,			FIELD_INTEGER,		"health" ),
	DEFINE_KEYFIELD( m_iszParticleName,	FIELD_STRING,		"explode_particle"),
	DEFINE_KEYFIELD( m_iszExplodeSound,	FIELD_SOUNDNAME,	"sound" ),
	DEFINE_KEYFIELD( m_bFriendlyFire,	FIELD_INTEGER,		"friendlyfire" ),

	DEFINE_INPUTFUNC( FIELD_VOID, "Detonate", InputDetonate ),
	DEFINE_OUTPUT( m_OnDetonate, "OnDetonate" ),
END_DATADESC()


CTFGenericBomb::CTFGenericBomb()
{
	SetMaxHealth( 1 );
	SetHealth( 1 );
	m_flDamage = 50.0f;
	m_flRadius = 100.0f;
	m_bFriendlyFire = false;
}

void CTFGenericBomb::Precache()
{
	PrecacheParticleSystem( STRING( m_iszParticleName ) );
	PrecacheScriptSound( STRING( m_iszExplodeSound ) );
	BaseClass::Precache();
}

void CTFGenericBomb::Spawn()
{
	SetMoveType( MOVETYPE_VPHYSICS );
	SetSolid( SOLID_VPHYSICS );
	BaseClass::Spawn();
	m_takedamage = DAMAGE_YES;
}

int CTFGenericBomb::OnTakeDamage( const CTakeDamageInfo &info )
{
	IPhysicsObject *pPhysics = VPhysicsGetObject();
	if ( pPhysics && !pPhysics->IsMoveable() )
	{
		pPhysics->EnableMotion( true );
		VPhysicsTakeDamage( info );
	}

	return BaseClass::OnTakeDamage( info );
}

void CTFGenericBomb::Event_Killed( const CTakeDamageInfo &info )
{
	Vector absOrigin = WorldSpaceCenter();
	QAngle absAngles = GetAbsAngles();

	trace_t	tr;
	Vector vecForward = GetAbsVelocity();
	VectorNormalize( vecForward );
	UTIL_TraceLine ( absOrigin, absOrigin + 60*vecForward , MASK_SHOT, 
		this, COLLISION_GROUP_NONE, &tr);

#if 0
	int iAttachment = LookupAttachment("alt-origin");

	if ( iAttachment > 0 )
		GetAttachment( iAttachment, absOrigin, absAngles );
#endif

	CPVSFilter filter( GetAbsOrigin() );

	if ( STRING( m_iszParticleName ) )
		TE_TFParticleEffect( filter, 0.0, STRING( m_iszParticleName ), GetAbsOrigin(), GetAbsAngles(), NULL, PATTACH_CUSTOMORIGIN );
	if ( STRING( m_iszExplodeSound ) )
		EmitSound( STRING( m_iszExplodeSound ) );

	SetSolid( SOLID_NONE ); 

	CBaseEntity *pAttacker = this;

	if ( info.GetAttacker() && info.GetAttacker()->IsPlayer() )
		pAttacker = info.GetAttacker();

	CTakeDamageInfo info_modified( this, pAttacker, m_flDamage, DMG_BLAST );

	if ( m_bFriendlyFire )
		info_modified.SetForceFriendlyFire( true );

	RadiusDamage( info_modified, absOrigin, m_flRadius, CLASS_NONE, this );

	if ( tr.m_pEnt && !tr.m_pEnt->IsPlayer() )
		UTIL_DecalTrace( &tr, "Scorch");

	/*UserMessageBegin( filter, "BreakModel" );
		WRITE_SHORT(v17);
		WRITE_VEC3COORD( absOrigin );
		WRITE_ANGLES( absAngles );
		WRITE_SHORT(*((_DWORD *)this + 222));
	MessageEnd();*/

	m_OnDetonate.FireOutput( this, this, 0.0f );
	BaseClass::Event_Killed( info );
}

void CTFGenericBomb::InputDetonate( inputdata_t &inputdata )
{
	Vector absOrigin = GetAbsOrigin();
	QAngle absAngles = GetAbsAngles();

	// Trace used for ground scortching
	trace_t	tr;
	Vector vecForward = GetAbsVelocity();
	VectorNormalize( vecForward );
	UTIL_TraceLine ( absOrigin, absOrigin + 60*vecForward , MASK_SHOT, 
		this, COLLISION_GROUP_NONE, &tr);

	// Do explosion effects
	CPVSFilter filter( GetAbsOrigin() );
	if ( STRING( m_iszParticleName ) )
		TE_TFParticleEffect( filter, 0.0, STRING( m_iszParticleName ), GetAbsOrigin(), GetAbsAngles(), NULL, PATTACH_CUSTOMORIGIN );
	if ( STRING( m_iszExplodeSound ) )
		EmitSound( STRING( m_iszExplodeSound ) );

	SetSolid( SOLID_NONE ); 

	CBaseEntity *pAttacker = this;

	if ( inputdata.pActivator && inputdata.pActivator->IsPlayer() )
		pAttacker = inputdata.pActivator;

	CTakeDamageInfo info( this, this, m_flDamage, DMG_BLAST );

	if ( m_bFriendlyFire )
		info.SetForceFriendlyFire( true );

	RadiusDamage( info, absOrigin, m_flRadius, CLASS_NONE, this );

	if ( tr.m_pEnt && !tr.m_pEnt->IsPlayer() )
		UTIL_DecalTrace( &tr, "Scorch");

	/*UserMessageBegin( filter, "BreakModel" );
		WRITE_SHORT(v17);
		WRITE_VEC3COORD( absOrigin );
		WRITE_ANGLES( absAngles );
		WRITE_SHORT(*((_DWORD *)this + 222));
	MessageEnd();*/
}

class CTFPumpkinBomb : public CTFGenericBomb
{
public:
	DECLARE_CLASS( CTFPumpkinBomb, CTFGenericBomb );

	virtual void	Spawn( void );

};

LINK_ENTITY_TO_CLASS( tf_pumpkin_bomb, CTFPumpkinBomb );

void CTFPumpkinBomb::Spawn( void )
{
	m_iszExplodeSound = MAKE_STRING( "Halloween.PumpkinExplode" );
	SetModelName( MAKE_STRING( "models/props_halloween/pumpkin_explode.mdl") );
	m_iszParticleName = MAKE_STRING( "pumpkin_explode" );
	m_flDamage = 140.0f;
	BaseClass::Spawn();
}