
//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
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
	SendPropBool(SENDINFO(m_bForceUpdate)),
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFProjectile_Rocket *CTFProjectile_Rocket::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CBaseEntity *pScorer )
{
	CTFProjectile_Rocket *pRocket = static_cast<CTFProjectile_Rocket*>( CTFBaseRocket::Create( "tf_projectile_rocket", vecOrigin, vecAngles, pOwner ) );

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
	CTFPlayer *pPlayer = dynamic_cast< CTFPlayer* >(GetOwnerEntity());
	if (pPlayer) //THIS IS HORRIBLE
	{
		if (pPlayer->IsActiveTFWeapon(TF_WEAPON_ROCKETLAUNCHER)) 
			SetModel(ROCKET_MODEL);
		else if (pPlayer->IsActiveTFWeapon(TF_WEAPON_ROCKETLAUNCHERBETA))
			SetModel("models/weapons/w_models/w_rocketbeta.mdl");
	}
	//SetModel( ROCKET_MODEL );
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFProjectile_Rocket::Precache()
{
	PrecacheModel( ROCKET_MODEL );
	PrecacheModel("models/weapons/w_models/w_rocketbeta.mdl");
	PrecacheParticleSystem( "critical_rocket_blue" );
	PrecacheParticleSystem( "critical_rocket_red" );
	PrecacheParticleSystem( "critical_rocket_green" );
	PrecacheParticleSystem( "critical_rocket_yellow" );
	PrecacheParticleSystem( "critical_rocket_dm" );
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