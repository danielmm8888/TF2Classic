//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "c_tf_projectile_rocket.h"
#include "particles_new.h"
#include "tf_gamerules.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFProjectile_Rocket, DT_TFProjectile_Rocket )

BEGIN_NETWORK_TABLE( C_TFProjectile_Rocket, DT_TFProjectile_Rocket )
	RecvPropBool( RECVINFO( m_bCritical ) ),
	RecvPropBool( RECVINFO( m_bForceUpdate )),
END_NETWORK_TABLE()

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFProjectile_Rocket::C_TFProjectile_Rocket( void )
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TFProjectile_Rocket::~C_TFProjectile_Rocket( void )
{
	ParticleProp()->StopEmission();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Rocket::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged(updateType);

	if (m_bForceUpdate)
	{
		m_bForceUpdate = false;
		ParticleProp()->StopEmission();
		CreateRocketTrails();
	}

	if ( updateType == DATA_UPDATE_CREATED )
	{
		CreateRocketTrails();		
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TFProjectile_Rocket::CreateRocketTrails( void )
{
	if ( IsDormant() )
		return;

	if ( enginetrace->GetPointContents( GetAbsOrigin() ) & MASK_WATER )
	{
		ParticleProp()->Create( "rockettrail_underwater", PATTACH_POINT_FOLLOW, "trail" );
	}
	else
	{
		ParticleProp()->Create( GetTrailParticleName(), PATTACH_POINT_FOLLOW, "trail" );
	}

	if ( m_bCritical )
	{
		if ( TFGameRules() && TFGameRules()->IsDeathmatch() )
		{
			C_TFPlayer *pPlayer = ToTFPlayer(GetOwnerEntity());
			if (pPlayer)
			{
				pPlayer->m_Shared.SetParticleToMercColor(
					ParticleProp()->Create("critical_rocket_dm", PATTACH_ABSORIGIN_FOLLOW)
					);
				return;
			}
			ParticleProp()->Create("critical_rocket_red", PATTACH_ABSORIGIN_FOLLOW);
		}

		switch( GetTeamNumber() )
		{
		case TF_TEAM_RED:
			ParticleProp()->Create("critical_rocket_red", PATTACH_ABSORIGIN_FOLLOW);
			break;
		case TF_TEAM_BLUE:
			ParticleProp()->Create("critical_rocket_blue", PATTACH_ABSORIGIN_FOLLOW );
			break;
		case TF_TEAM_GREEN:
			ParticleProp()->Create("critical_rocket_green", PATTACH_ABSORIGIN_FOLLOW);
			break;
		case TF_TEAM_YELLOW:
			ParticleProp()->Create("critical_rocket_yellow", PATTACH_ABSORIGIN_FOLLOW);
			break;
		default:
			break;
		}
	}
}
