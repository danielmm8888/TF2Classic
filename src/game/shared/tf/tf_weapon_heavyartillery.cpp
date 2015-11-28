//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: A lightweight minigun variant for use in DM
//
//=============================================================================

#include "cbase.h"
#ifdef GAME_DLL
#include "tf_player.h"
#else
#include "c_tf_player.h"
#endif
#include "tf_weapon_heavyartillery.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFHeavyArtillery, DT_TFHeavyArtillery )
															
BEGIN_NETWORK_TABLE( CTFHeavyArtillery, DT_TFHeavyArtillery )			
END_NETWORK_TABLE()										

BEGIN_PREDICTION_DATA( CTFHeavyArtillery )				
END_PREDICTION_DATA()									
														
LINK_ENTITY_TO_CLASS( tf_weapon_heavyartillery, CTFHeavyArtillery );		
PRECACHE_WEAPON_REGISTER( tf_weapon_heavyartillery );

//-----------------------------------------------------------------------------
// Purpose: Constructor.
//-----------------------------------------------------------------------------
CTFHeavyArtillery::CTFHeavyArtillery()
{
	//WeaponReset();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor.
//-----------------------------------------------------------------------------
CTFHeavyArtillery::~CTFHeavyArtillery()
{
	//WeaponReset();
}

void CTFHeavyArtillery::PrimaryAttack( void )
{
	CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );

	if ( !pPlayer )
		return;

	BaseClass::PrimaryAttack();

	float maxVerticalKickAngle = m_pWeaponInfo->GetWeaponData(TF_WEAPON_PRIMARY_MODE).m_flPunchAngle;
	float slideLimitTime = 10.5f;

	// Find how far into our accuracy degradation we are
	float kickPerc = m_fFireDuration * slideLimitTime;

	// do this to get a hard discontinuity, clear out anything under 10 degrees punch
	pPlayer->ViewPunchReset( 10 );

	//Apply this to the view angles as well
	QAngle vecScratch;
	vecScratch.x = -( 0.2f + ( maxVerticalKickAngle * kickPerc ) );
	vecScratch.y = -( 0.2f + ( maxVerticalKickAngle * kickPerc ) ) / 3;
	vecScratch.z = 0.1f + ( maxVerticalKickAngle * kickPerc) / 8;

	//Wibble left and right
	if ( random->RandomInt( -1, 1 ) >= 0 )
		vecScratch.y *= -1;

	//Wobble up and down
	if ( random->RandomInt( -1, 1 ) >= 0 )
		vecScratch.z *= -1;

	//Clip this to our desired min/max
	ClipPunchAngleOffset( vecScratch, pPlayer->m_Local.m_vecPunchAngle, QAngle( 24.0f, 3.0f, 1.0f ) );

	//Add it to the view punch
	// NOTE: 0.5 is just tuned to match the old effect before the punch became simulated
	pPlayer->ViewPunch( vecScratch * 0.5 );
}

void CTFHeavyArtillery::ClipPunchAngleOffset( QAngle &in, const QAngle &punch, const QAngle &clip )
{
	QAngle	final = in + punch;

	//Clip each component
	for ( int i = 0; i < 3; i++ )
	{
		if ( final[i] > clip[i] )
		{
			final[i] = clip[i];
		}
		else if ( final[i] < -clip[i] )
		{
			final[i] = -clip[i];
		}

		//Return the result
		in[i] = final[i] - punch[i];
	}
}
