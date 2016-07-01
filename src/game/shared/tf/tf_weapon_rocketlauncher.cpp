//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket Launcher
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_rocketlauncher.h"
#include "tf_fx_shared.h"
#include "tf_weaponbase_rocket.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Rocket Launcher tables.
//
CREATE_SIMPLE_WEAPON_TABLE( TFRocketLauncher, tf_weapon_rocketlauncher )

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFRocketLauncher::CTFRocketLauncher()
{
	m_bReloadsSingly = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFRocketLauncher::~CTFRocketLauncher()
{
}

#ifndef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::Precache()
{
	BaseClass::Precache();
	PrecacheParticleSystem( "rocketbackblast" );
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CTFRocketLauncher::FireProjectile( CTFPlayer *pPlayer )
{
	m_flShowReloadHintAt = gpGlobals->curtime + 30;
	return BaseClass::FireProjectile( pPlayer );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::ItemPostFrame( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( !pOwner )
		return;

	BaseClass::ItemPostFrame();

#ifdef GAME_DLL

	if ( m_flShowReloadHintAt && m_flShowReloadHintAt < gpGlobals->curtime )
	{
		if ( Clip1() < GetMaxClip1() )
		{
			pOwner->HintMessage( HINT_SOLDIER_RPG_RELOAD );
		}
		m_flShowReloadHintAt = 0;
	}

#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFRocketLauncher::Deploy( void )
{
	if ( BaseClass::Deploy() )
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFRocketLauncher::DefaultReload( int iClipSize1, int iClipSize2, int iActivity )
{
	m_flShowReloadHintAt = 0;
	return BaseClass::DefaultReload( iClipSize1, iClipSize2, iActivity );
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncher::CreateMuzzleFlashEffects( C_BaseEntity *pAttachEnt, int nIndex )
{
	BaseClass::CreateMuzzleFlashEffects( pAttachEnt, nIndex );

	// Don't do backblast effects in first person
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( pOwner->IsLocalPlayer() )
		return;

	ParticleProp()->Create( "rocketbackblast", PATTACH_POINT_FOLLOW, "backblast" );
}

#endif