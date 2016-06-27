//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: TF Hunter Rifle
//
//=============================================================================//
#include "cbase.h" 
#include "tf_fx_shared.h"
#include "tf_weapon_hunterrifle.h"
#include "in_buttons.h"

// Client specific.
#ifdef CLIENT_DLL
#include "view.h"
#include "beamdraw.h"
#include "vgui/ISurface.h"
#include <vgui/ILocalize.h>
#include "vgui_controls/Controls.h"
#include "hud_crosshair.h"
#include "functionproxy.h"
#include "materialsystem/imaterialvar.h"
#include "toolframework_client.h"
#include "input.h"

// forward declarations
void ToolFramework_RecordMaterialParams( IMaterial *pMaterial );
#endif

#define TF_WEAPON_HUNTERRIFLE_CHARGE_PER_SEC	600.0
#define TF_WEAPON_HUNTERRIFLE_UNCHARGE_PER_SEC	20.0
#define	TF_WEAPON_HUNTERRIFLE_DAMAGE			60
#define TF_WEAPON_HUNTERRIFLE_RELOAD_TIME		1.0f
#define TF_WEAPON_HUNTERRIFLE_ZOOM_TIME			0.3f
#define TF_WEAPON_HUNTERRIFLE_SHOOT_TIME		0.15f
#define TF_WEAPON_HUNTERRIFLE_SPREAD_MIN		0
#define TF_WEAPON_HUNTERRIFLE_SPREAD_MAX		300
#define TF_WEAPON_HUNTERRIFLE_NO_CRIT_AFTER_ZOOM_TIME	0.2f


//=============================================================================
//
// Weapon Hunter Rifles tables.
//

IMPLEMENT_NETWORKCLASS_ALIASED( TFHunterRifle, DT_TFHunterRifle )

BEGIN_NETWORK_TABLE_NOBASE( CTFHunterRifle, DT_HunterRifleLocalData )
#if !defined( CLIENT_DLL )
	SendPropFloat( SENDINFO( m_flChargedSpread ), 0, SPROP_NOSCALE | SPROP_CHANGES_OFTEN ),
#else
	RecvPropFloat( RECVINFO(m_flChargedSpread) ),
#endif
END_NETWORK_TABLE()

BEGIN_NETWORK_TABLE( CTFHunterRifle, DT_TFHunterRifle )
#if !defined( CLIENT_DLL )
	SendPropDataTable( "HunterRifleLocalData", 0, &REFERENCE_SEND_TABLE( DT_HunterRifleLocalData ), SendProxy_SendLocalWeaponDataTable ),
#else
	RecvPropDataTable("HunterRifleLocalData", 0, 0, &REFERENCE_RECV_TABLE(DT_HunterRifleLocalData)),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFHunterRifle )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_flUnzoomTime, FIELD_FLOAT, 0 ),
	DEFINE_PRED_FIELD( m_flRezoomTime, FIELD_FLOAT, 0 ),
	DEFINE_PRED_FIELD( m_bRezoomAfterShot, FIELD_BOOLEAN, 0 ),
	DEFINE_PRED_FIELD( m_flChargedSpread, FIELD_FLOAT, 0 ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_hunterrifle, CTFHunterRifle );
PRECACHE_WEAPON_REGISTER( tf_weapon_hunterrifle );

//=============================================================================
//
// Weapon Hunter Rifles funcions.
//

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHunterRifle::Spawn()
{
	//m_iAltFireHint = HINT_ALTFIRE_SNIPERRIFLE;
	BaseClass::Spawn();
	ResetTimers();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHunterRifle::ResetTimers( void )
{
	m_flUnzoomTime = -1;
	m_flRezoomTime = -1;
	m_bRezoomAfterShot = false;
	m_flChargedSpread = TF_WEAPON_HUNTERRIFLE_SPREAD_MAX;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHunterRifle::CanHolster( void ) const
{
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer )
	{
		// don't allow us to holster this weapon if we're in the process of zooming and 
		// we've just fired the weapon (next primary attack is only 1.5 seconds after firing)
		if ( ( pPlayer->GetFOV() < pPlayer->GetDefaultFOV() ) && ( m_flNextPrimaryAttack > gpGlobals->curtime ) )
		{
			return false;
		}
	}

	return BaseClass::CanHolster();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFHunterRifle::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	// Server specific.
#ifdef GAME_DLL
	// Destroy the Hunter dot.

#endif

	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( pPlayer && pPlayer->m_Shared.InCond( TF_COND_ZOOMED ) )
	{
		ZoomOut();
	}

	m_flChargedSpread = TF_WEAPON_HUNTERRIFLE_SPREAD_MAX;
	ResetTimers();

	return BaseClass::Holster( pSwitchingTo );
}

void CTFHunterRifle::WeaponReset( void )
{
	BaseClass::WeaponReset();

	ZoomOut();
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFHunterRifle::HandleZooms( void )
{
	// Get the owning player.
	CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );
	if ( !pPlayer )
		return;

	// Handle the zoom when taunting.
	if ( pPlayer->m_Shared.InCond( TF_COND_TAUNTING ) )
	{
		if ( pPlayer->m_Shared.InCond( TF_COND_AIMING ) )
		{
			ToggleZoom();
		}

		//Don't rezoom in the middle of a taunt.
		ResetTimers();
	}

	if ( m_flUnzoomTime > 0 && gpGlobals->curtime > m_flUnzoomTime )
	{
		if ( m_bRezoomAfterShot )
		{
			ZoomOutIn();
			m_bRezoomAfterShot = false;
		}
		else
		{
			ZoomOut();
		}

		m_flUnzoomTime = -1;
	}

	if ( m_flRezoomTime > 0 )
	{
		if ( gpGlobals->curtime > m_flRezoomTime )
		{
			ZoomIn();
			m_flRezoomTime = -1;
		}
	}

	if ( ( pPlayer->m_nButtons & IN_ATTACK2 ) && ( m_flNextSecondaryAttack <= gpGlobals->curtime ) )
	{
		// If we're in the process of rezooming, just cancel it
		if ( m_flRezoomTime > 0 || m_flUnzoomTime > 0 )
		{
			// Prevent them from rezooming in less time than they would have
			m_flNextSecondaryAttack = m_flRezoomTime + TF_WEAPON_HUNTERRIFLE_ZOOM_TIME;
			m_flRezoomTime = -1;
		}
		else
		{
			Zoom();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFHunterRifle::Reload( void )
{
	if ( BaseClass::Reload() == true )
	{
		if ( IsZoomed() )
			ZoomOut();

		if ( Clip1() > 0 )
			m_iClip1 = 0;

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFHunterRifle::ItemPostFrame( void )
{
	// If we're lowered, we're not allowed to fire
	if ( m_bLowered )
		return;

	// Get the owning player.
	CTFPlayer *pPlayer = ToTFPlayer( GetOwner() );
	if ( !pPlayer )
		return;

	CheckReload();

	if ( !CanAttack() )
	{
		if ( IsZoomed() )
		{
			ToggleZoom();
		}
		return;
	}

	HandleZooms();

	if ( m_flNextSecondaryAttack <= gpGlobals->curtime )
	{
		// Don't start charging in the time just after a shot before we unzoom to play rack anim.
		if ( pPlayer->m_Shared.InCond( TF_COND_AIMING ) && !m_bRezoomAfterShot )
		{
			m_flChargedSpread = max( m_flChargedSpread - gpGlobals->frametime * TF_WEAPON_HUNTERRIFLE_CHARGE_PER_SEC, TF_WEAPON_HUNTERRIFLE_SPREAD_MIN );
		}
		else
		{
			m_flChargedSpread = min( TF_WEAPON_HUNTERRIFLE_SPREAD_MAX, m_flChargedSpread + gpGlobals->frametime * TF_WEAPON_HUNTERRIFLE_UNCHARGE_PER_SEC );
		}
	}

	// Fire.
	if ( pPlayer->m_nButtons & IN_ATTACK )
	{
		Fire( pPlayer );
	}

	//  Reload pressed / Clip Empty
	if ( ( pPlayer->m_nButtons & IN_RELOAD ) && !m_bInReload )
	{
		// reload when reload is pressed, or if no buttons are down and weapon is empty.
		Reload();
	}

	// Idle.
	if ( !( ( pPlayer->m_nButtons & IN_ATTACK ) || ( pPlayer->m_nButtons & IN_ATTACK2 ) ) )
	{
		// No fire buttons down or reloading
		if ( !ReloadOrSwitchWeapons() && ( m_bInReload == false ) )
		{
			WeaponIdle();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFHunterRifle::Fire( CTFPlayer *pPlayer )
{
	if ( m_flNextPrimaryAttack > gpGlobals->curtime )
		return;

	// Check the ammo.
	if ( Clip1() <= 0 )
	{
		HandleFireOnEmpty();
		return;
	}

	// Fire the Hunter shot.
	PrimaryAttack();

	if ( !IsZoomed() )
	{
		// Prevent primary fire preventing zooms
		m_flNextSecondaryAttack = gpGlobals->curtime + SequenceDuration();
	}

	m_flChargedSpread = TF_WEAPON_HUNTERRIFLE_SPREAD_MAX;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHunterRifle::Lower( void )
{
	if ( BaseClass::Lower() )
	{
		if ( IsZoomed() )
		{
			ToggleZoom();
		}

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Secondary attack.
//-----------------------------------------------------------------------------
void CTFHunterRifle::Zoom( void )
{
	// Don't allow the player to zoom in while jumping
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	ToggleZoom();

	// at least 0.1 seconds from now, but don't stomp a previous value
	m_flNextPrimaryAttack = max( m_flNextPrimaryAttack, gpGlobals->curtime + 0.1 );
	m_flNextSecondaryAttack = gpGlobals->curtime + TF_WEAPON_HUNTERRIFLE_ZOOM_TIME;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHunterRifle::ZoomOutIn( void )
{
	ZoomOut();

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer && pPlayer->ShouldAutoRezoom() )
	{
		m_flRezoomTime = gpGlobals->curtime + 0.9;
	}
	else
	{
		m_flNextSecondaryAttack = gpGlobals->curtime + 1.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHunterRifle::ZoomIn( void )
{
	// Start aiming.
	CTFPlayer *pPlayer = GetTFPlayerOwner();

	if ( !pPlayer )
		return;

	if ( pPlayer->GetAmmoCount( m_iPrimaryAmmoType ) <= 0 || Clip1() <= 0 )
		return;

	BaseClass::ZoomIn();

	pPlayer->m_Shared.AddCond( TF_COND_AIMING );
	pPlayer->TeamFortress_SetSpeed();

#ifdef GAME_DLL
	pPlayer->ClearExpression();
#endif
}

bool CTFHunterRifle::IsZoomed( void )
{
	CTFPlayer *pPlayer = GetTFPlayerOwner();

	if ( pPlayer )
	{
		return pPlayer->m_Shared.InCond( TF_COND_ZOOMED );
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHunterRifle::ZoomOut( void )
{
	BaseClass::ZoomOut();

	// Stop aiming
	CTFPlayer *pPlayer = GetTFPlayerOwner();

	if ( !pPlayer )
		return;

	pPlayer->m_Shared.RemoveCond( TF_COND_AIMING );
	pPlayer->TeamFortress_SetSpeed();

#ifdef GAME_DLL
	pPlayer->ClearExpression();
#endif

	// if we are thinking about zooming, cancel it
	m_flUnzoomTime = -1;
	m_flRezoomTime = -1;
	m_bRezoomAfterShot = false;
	m_flChargedSpread = TF_WEAPON_HUNTERRIFLE_SPREAD_MAX;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHunterRifle::SetRezoom( bool bRezoom, float flDelay )
{
	m_flUnzoomTime = gpGlobals->curtime + flDelay;

	m_bRezoomAfterShot = bRezoom;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : float
//-----------------------------------------------------------------------------
float CTFHunterRifle::GetProjectileDamage( void )
{
	return BaseClass::GetProjectileDamage();
}

float CTFHunterRifle::GetWeaponSpread( void )
{
	return m_flChargedSpread * 0.0001;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int	CTFHunterRifle::GetDamageType( void ) const
{
	// Only do hit location damage if we're zoomed
	CTFPlayer *pPlayer = ToTFPlayer( GetPlayerOwner() );
	if ( pPlayer && pPlayer->m_Shared.InCond( TF_COND_AIMING ) )
		return BaseClass::GetDamageType();

	return ( BaseClass::GetDamageType() & ~DMG_USE_HITLOCATIONS );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFHunterRifle::CanFireCriticalShot( bool bIsHeadshot )
{
	// can only fire a crit shot if this is a headshot
	if ( !bIsHeadshot )
		return false;

	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer )
	{
		// no crits if they're not zoomed
		if ( pPlayer->GetFOV() >= pPlayer->GetDefaultFOV() )
		{
			return false;
		}

		// no crits for 0.2 seconds after starting to zoom
		if ( ( gpGlobals->curtime - pPlayer->GetFOVTime() ) < TF_WEAPON_HUNTERRIFLE_NO_CRIT_AFTER_ZOOM_TIME )
		{
			return false;
		}
	}

	return true;
}