//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "hud_numericdisplay.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include "iclientmode.h"
#include "tf_shareddefs.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "tf_controls.h"
#include "in_buttons.h"
#include "tf_imagepanel.h"
#include "c_team.h"
#include "c_tf_player.h"
#include "ihudlcd.h"
#include "tf_hud_ammostatus.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_HUDELEMENT( CTFHudWeaponAmmo );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFHudWeaponAmmo::CTFHudWeaponAmmo( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudWeaponAmmo" ) 
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_HEALTH | HIDEHUD_PLAYERDEAD );

	hudlcd->SetGlobalStat( "(ammo_primary)", "0" );
	hudlcd->SetGlobalStat( "(ammo_secondary)", "0" );
	hudlcd->SetGlobalStat( "(weapon_print_name)", "" );
	hudlcd->SetGlobalStat( "(weapon_name)", "" );

	m_pInClip = NULL;
	m_pInClipShadow = NULL;
	m_pInReserve = NULL;
	m_pInReserveShadow = NULL;
	m_pNoClip = NULL;
	m_pNoClipShadow = NULL;

	m_pWeaponBucket = NULL;

	m_nAmmo	= -1;
	m_nAmmo2 = -1;
	m_hCurrentActiveWeapon = NULL;
	m_flNextThink = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponAmmo::Reset()
{
	m_flNextThink = gpGlobals->curtime + 0.05f;
}

ConVar tf2c_ammobucket("tf2c_ammobucket", "0", FCVAR_ARCHIVE, "Shows weapon bucket in the ammo section. 1 = ON, 0 = OFF.");

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponAmmo::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/HudAmmoWeapons.res" );

	m_pInClip = dynamic_cast<CExLabel *>(FindChildByName("AmmoInClip"));
	m_pInClipShadow = dynamic_cast<CExLabel *>(FindChildByName("AmmoInClipShadow"));

	m_pInReserve = dynamic_cast<CExLabel *>(FindChildByName("AmmoInReserve"));
	m_pInReserveShadow = dynamic_cast<CExLabel *>(FindChildByName("AmmoInReserveShadow"));

	m_pNoClip = dynamic_cast<CExLabel *>(FindChildByName("AmmoNoClip"));
	m_pNoClipShadow = dynamic_cast<CExLabel *>(FindChildByName("AmmoNoClipShadow"));

	m_pWeaponBucket = dynamic_cast<ImagePanel *>(FindChildByName("WeaponBucket"));

	m_nAmmo	= -1;
	m_nAmmo2 = -1;
	m_hCurrentActiveWeapon = NULL;
	m_flNextThink = 0.0f;

	UpdateAmmoLabels( false, false, false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudWeaponAmmo::ShouldDraw( void )
{
	// Get the player and active weapon.
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer )
	{
		return false;
	}

	C_TFWeaponBase *pWeapon = pPlayer->GetActiveTFWeapon();

	if ( !pWeapon )
	{
		return false;
	}

	if ( !pWeapon->UsesPrimaryAmmo() && !tf2c_ammobucket.GetBool() )
	{
		return false;
	}

	CHudElement *pMedicCharge = GET_NAMED_HUDELEMENT( CHudElement, CHudMedicChargeMeter );

	if ( pMedicCharge && pMedicCharge->ShouldDraw() )
	{
		return false;
	}

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponAmmo::UpdateAmmoLabels( bool bPrimary, bool bReserve, bool bNoClip )
{
	if ( m_pInClip && m_pInClipShadow )
	{
		if ( m_pInClip->IsVisible() != bPrimary )
		{
			m_pInClip->SetVisible( bPrimary );
			m_pInClipShadow->SetVisible( bPrimary );
		}
	}

	if ( m_pInReserve && m_pInReserveShadow )
	{
		if ( m_pInReserve->IsVisible() != bReserve )
		{
			m_pInReserve->SetVisible( bReserve );
			m_pInReserveShadow->SetVisible( bReserve );
		}
	}

	if ( m_pNoClip && m_pNoClipShadow )
	{
		if ( m_pNoClip->IsVisible() != bNoClip )
		{
			m_pNoClip->SetVisible( bNoClip );
			m_pNoClipShadow->SetVisible( bNoClip );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get ammo info from the weapon and update the displays.
//-----------------------------------------------------------------------------
void CTFHudWeaponAmmo::OnThink()
{
	// Get the player and active weapon.
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pPlayer )
		return;

	C_BaseCombatWeapon *pWeapon = pPlayer->GetActiveWeapon();

	if ( m_flNextThink < gpGlobals->curtime )
	{
		bool bShowIcon = false;

		if ( tf2c_ammobucket.GetBool() && pWeapon && m_pWeaponBucket )
		{
			int iItemID = pWeapon->GetItemID();
			CEconItemDefinition *pItemDefinition = GetItemSchema()->GetItemDefinition( iItemID );
			if ( pItemDefinition )
			{
				char szImage[128];
				Q_snprintf (szImage, sizeof( szImage ), "../%s_large", pItemDefinition->image_inventory );
				m_pWeaponBucket->SetImage( szImage );
				bShowIcon = true;
			}
		}
		if ( m_pWeaponBucket && m_pWeaponBucket->IsVisible() != bShowIcon )
		{
			m_pWeaponBucket->SetVisible( bShowIcon );
		}

		hudlcd->SetGlobalStat( "(weapon_print_name)", pWeapon ? pWeapon->GetPrintName() : " " );
		hudlcd->SetGlobalStat( "(weapon_name)", pWeapon ? pWeapon->GetName() : " " );

		if ( !pPlayer || !pWeapon || !pWeapon->UsesPrimaryAmmo() )
		{
			hudlcd->SetGlobalStat( "(ammo_primary)", "n/a" );
			hudlcd->SetGlobalStat( "(ammo_secondary)", "n/a" );

			// turn off our ammo counts
			UpdateAmmoLabels( false, false, false );

			m_nAmmo = -1;
			m_nAmmo2 = -1;
		}
		else
		{
			// Get the ammo in our clip.
			int nAmmo1 = pWeapon->Clip1();
			int nAmmo2 = 0;
			// Clip ammo not used, get total ammo count.
			if ( nAmmo1 < 0 )
			{
				nAmmo1 = pPlayer->GetAmmoCount( pWeapon->GetPrimaryAmmoType() );
			}
			// Clip ammo, so the second ammo is the total ammo.
			else
			{
				nAmmo2 = pPlayer->GetAmmoCount( pWeapon->GetPrimaryAmmoType() );
			}
			
			hudlcd->SetGlobalStat( "(ammo_primary)", VarArgs( "%d", nAmmo1 ) );
			hudlcd->SetGlobalStat( "(ammo_secondary)", VarArgs( "%d", nAmmo2 ) );

			if ( m_nAmmo != nAmmo1 || m_nAmmo2 != nAmmo2 || m_hCurrentActiveWeapon.Get() != pWeapon )
			{
				m_nAmmo = nAmmo1;
				m_nAmmo2 = nAmmo2;
				m_hCurrentActiveWeapon = pWeapon;

				if ( m_hCurrentActiveWeapon.Get()->UsesClipsForAmmo1() )
				{
					UpdateAmmoLabels( true, true, false );

					SetDialogVariable( "Ammo", m_nAmmo );
					SetDialogVariable( "AmmoInReserve", m_nAmmo2 );
				}
				else
				{
					UpdateAmmoLabels( false, false, true );
					SetDialogVariable( "Ammo", m_nAmmo );
				}
			}
		}

		m_flNextThink = gpGlobals->curtime + 0.1f;
	}
}
