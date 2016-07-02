
//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket
//
//=============================================================================
#include "cbase.h"
#include "tf_projectile_rocket.h"
#include "tf_player.h"

//=============================================================================
//
// TF Rocket functions (Server specific).
//
#define ROCKET_MODEL "models/weapons/w_models/w_rocket.mdl"

LINK_ENTITY_TO_CLASS( tf_projectile_rocket, CTFProjectile_Rocket );
PRECACHE_REGISTER( tf_projectile_rocket );

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Rocket, DT_TFProjectile_Rocket )

BEGIN_NETWORK_TABLE( CTFProjectile_Rocket, DT_TFProjectile_Rocket )
	SendPropBool( SENDINFO( m_bCritical ) ),
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Rocket *CTFProjectile_Rocket::Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_Rocket *pRocket = static_cast<CTFProjectile_Rocket*>( CTFBaseRocket::Create( pWeapon, "tf_projectile_rocket", vecOrigin, vecAngles, pOwner ) );

	if ( pRocket )
	{
		pRocket->SetScorer( pScorer );
	}

	return pRocket;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Rocket::Spawn()
{
	SetModel( ROCKET_MODEL );
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Rocket::Precache()
{
	PrecacheModel( ROCKET_MODEL );

	PrecacheTeamParticles( "critical_rocket_%s", true );
	PrecacheParticleSystem( "rockettrail" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Rocket::SetScorer( CBaseEntity *pScorer )
{
	m_Scorer = pScorer;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CBasePlayer *CTFProjectile_Rocket::GetScorer( void )
{
	return dynamic_cast<CBasePlayer *>( m_Scorer.Get() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFProjectile_Rocket::GetDamageType()
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
void CTFProjectile_Rocket::Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir )
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
