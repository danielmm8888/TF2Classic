//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// TF Rocket Launcher
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_rocketlauncherbeta.h"
#include "tf_fx_shared.h"
#include "tf_weaponbase_rocket.h"
#include "in_buttons.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
// Server specific.
#else
#include "tf_player.h"
#endif

//=============================================================================
//
// Weapon Rocket Launcher tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFRocketLauncherBeta, DT_WeaponRocketLauncherBeta )

BEGIN_NETWORK_TABLE(CTFRocketLauncherBeta, DT_WeaponRocketLauncherBeta)
#ifndef CLIENT_DLL
	//SendPropBool( SENDINFO(m_bLockedOn) ),
#else
	//RecvPropInt( RECVINFO(m_bLockedOn) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFRocketLauncherBeta )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_rocketlauncherbeta, CTFRocketLauncherBeta );
PRECACHE_WEAPON_REGISTER(tf_weapon_rocketlauncherbeta);

// Server specific.
#ifndef CLIENT_DLL
BEGIN_DATADESC( CTFRocketLauncherBeta )
END_DATADESC()
#endif

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFRocketLauncherBeta::CTFRocketLauncherBeta()
{
	m_bReloadsSingly = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  :  - 
//-----------------------------------------------------------------------------
CTFRocketLauncherBeta::~CTFRocketLauncherBeta()
{
}

#ifndef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncherBeta::Precache()
{
	BaseClass::Precache();
	PrecacheParticleSystem( "rocketbackblast" );
}
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CTFRocketLauncherBeta::FireProjectile( CTFPlayer *pPlayer )
{
	m_flShowReloadHintAt = gpGlobals->curtime + 30;
	return BaseClass::FireProjectile( pPlayer );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncherBeta::ItemPostFrame( void )
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

	/*
	Vector forward;
	AngleVectors( pOwner->EyeAngles(), &forward );
	trace_t tr;
	CTraceFilterSimple filter( pOwner, COLLISION_GROUP_NONE );
	UTIL_TraceLine( pOwner->EyePosition(), pOwner->EyePosition() + forward * 2000, MASK_SOLID, &filter, &tr );

	if ( tr.m_pEnt &&
		tr.m_pEnt->IsPlayer() &&
		tr.m_pEnt->IsAlive() &&
		tr.m_pEnt->GetTeamNumber() != pOwner->GetTeamNumber() )
	{
		m_bLockedOn = true;
	}
	else
	{
		m_bLockedOn = false;
	}
	*/

#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFRocketLauncherBeta::Deploy( void )
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
bool CTFRocketLauncherBeta::DefaultReload( int iClipSize1, int iClipSize2, int iActivity )
{
	m_flShowReloadHintAt = 0;
	return BaseClass::DefaultReload( iClipSize1, iClipSize2, iActivity );
}

#ifdef CLIENT_DLL
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncherBeta::CreateMuzzleFlashEffects( C_BaseEntity *pAttachEnt, int nIndex )
{
	BaseClass::CreateMuzzleFlashEffects( pAttachEnt, nIndex );

	// Don't do backblast effects in first person
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	if ( pOwner->IsLocalPlayer() )
		return;

	ParticleProp()->Create( "rocketbackblast", PATTACH_POINT_FOLLOW, "backblast" );
}
/*
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRocketLauncherBeta::DrawCrosshair( void )
{
	BaseClass::DrawCrosshair();

	if ( m_bLockedOn )
	{
		int iXpos = XRES(340);
		int iYpos = YRES(260);
		int iWide = XRES(8);
		int iTall = YRES(8);

		Color col( 0, 255, 0, 255 );
		vgui::surface()->DrawSetColor( col );

		vgui::surface()->DrawFilledRect( iXpos, iYpos, iXpos + iWide, iYpos + iTall );

		// Draw the charge level onscreen
		vgui::HScheme scheme = vgui::scheme()->GetScheme( "ClientScheme" );
		vgui::HFont hFont = vgui::scheme()->GetIScheme(scheme)->GetFont( "Default" );
		vgui::surface()->DrawSetTextFont( hFont );
		vgui::surface()->DrawSetTextColor( col );
		vgui::surface()->DrawSetTextPos(iXpos + XRES(12), iYpos );
		vgui::surface()->DrawPrintText(L"Lock", wcslen(L"Lock"));

		vgui::surface()->DrawLine( XRES(320), YRES(240), iXpos, iYpos );
	}
}
*/
#endif