//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <vgui/ILocalize.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Label.h>
#include "controls/tf_advbuttonbase.h"

#include "tf_hud_weaponswitch.h"
#include "c_tf_player.h"
#include "tf_hud_freezepanel.h"

#include "tf_gamerules.h"
#include "tf_weapon_parse.h"
#include "c_tf_playerresource.h"

using namespace vgui;

DECLARE_HUDELEMENT( CTFHudWeaponSwitch );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CItemModelPanel::CItemModelPanel( Panel *parent, const char* name ) : EditablePanel( parent, name )
{
	m_pWeapon = NULL;
	m_pWeaponName = new vgui::Label( this, "WeaponName", "text" );
	m_pSlotID = new vgui::Label( this, "SlotID", "0" );
	m_pWeaponImage = new vgui::ImagePanel( this, "WeaponImage" );
	m_iBorderStyle = -1;
	m_ID = -1;
}

void CItemModelPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_pSlotID->SetFgColor( pScheme->GetColor( "TanLight", Color( 255, 255, 255, 255 ) ) );
	m_pDefaultFont = pScheme->GetFont( "ItemFontNameSmallest", true );
	m_pSelectedFont = pScheme->GetFont( "ItemFontNameSmall", true );
	m_pNumberDefaultFont = pScheme->GetFont( "FontStorePromotion", true );
	m_pNumberSelectedFont = pScheme->GetFont( "HudFontSmall", true );
	m_pDefaultBorder = pScheme->GetBorder( "TFFatLineBorder" );
	m_pSelectedRedBorder = pScheme->GetBorder( "TFFatLineBorderRedBG" );
	m_pSelectedBlueBorder = pScheme->GetBorder( "TFFatLineBorderBlueBG" );
}

void CItemModelPanel::PerformLayout( void )
{
	if ( m_iBorderStyle == -1 )
	{
		SetPaintBorderEnabled( false );
	}
	else if ( m_iBorderStyle == 0 )
	{
		SetPaintBorderEnabled( true );
		IBorder *border = m_pDefaultBorder;
		SetBorder( border );
	}
	else if ( m_iBorderStyle == 1 )
	{
		C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
		IBorder *border;
		if ( !pPlayer )
			return;
		int iTeam = pPlayer->GetTeamNumber();
		if ( iTeam == TF_TEAM_RED )
		{
			border = m_pSelectedRedBorder;
		}
		else
		{
			border = m_pSelectedBlueBorder;
		}
		SetBorder( border );
	}
	m_pWeaponImage->SetShouldScaleImage( true );
	m_pWeaponImage->SetZPos( -1 );
	m_pWeaponName->SetBounds( XRES( 5 ), GetTall() - YRES( 20 ), GetWide() - XRES( 10 ), YRES( 20 ) );
	m_pWeaponName->SetFont( m_iBorderStyle ? m_pSelectedFont : m_pDefaultFont );
	m_pWeaponName->SetContentAlignment( CTFAdvButtonBase::GetAlignment( "center" ) );
	m_pWeaponName->SetCenterWrap( true );
	if ( m_pWeapon && !m_pWeapon->CanBeSelected() )
	{
		wchar_t *pText = g_pVGuiLocalize->Find( "#TF_OUT_OF_AMMO" );
		m_pWeaponName->SetText( pText );
		m_pWeaponName->SetFgColor( GETSCHEME()->GetColor( "RedSolid", Color( 255, 255, 255, 255 ) ) );
	}
	else
	{
		m_pWeaponName->SetFgColor( GETSCHEME()->GetColor( "TanLight", Color( 255, 255, 255, 255 ) ) );
	}
	m_pSlotID->SetBounds( 0, YRES( 5 ), GetWide() - XRES( 5 ), YRES( 10 ) );
	m_pSlotID->SetFont( m_iBorderStyle ? m_pNumberSelectedFont : m_pNumberDefaultFont );
	m_pSlotID->SetContentAlignment( CTFAdvButtonBase::GetAlignment( "east" ) );
}

void CItemModelPanel::SetWeapon( C_BaseCombatWeapon *pWeapon, int iBorderStyle, int ID )
{
	m_pWeapon = pWeapon;
	m_ID = ID;
	m_iBorderStyle = iBorderStyle;

	int iItemID = m_pWeapon->GetItemID();
	CEconItemDefinition *pItemDefinition = GetItemSchema()->GetItemDefinition( iItemID );
	wchar_t *pText = NULL;
	if ( pItemDefinition )
	{
		pText = g_pVGuiLocalize->Find( pItemDefinition->item_name );
		char szImage[128];
		Q_snprintf( szImage, sizeof( szImage ), "../%s_large", pItemDefinition->image_inventory );
		m_pWeaponImage->SetImage( szImage );
		m_pWeaponImage->SetBounds( XRES( 4 ), -1 * ( GetTall() / 5.0 ) + XRES( 4 ), GetWide() - XRES( 8 ), GetWide() - XRES( 8 ) );
	}
	else
	{
		pText = g_pVGuiLocalize->Find( m_pWeapon->GetWpnData().szPrintName );
		const CHudTexture *pTexture = pWeapon->GetSpriteInactive(); // red team
		if ( pTexture )
		{
			char szImage[64];
			Q_snprintf( szImage, sizeof( szImage ), "../%s", pTexture->szTextureFile );
			m_pWeaponImage->SetImage( szImage );
		}
		m_pWeaponImage->SetBounds( XRES( 4 ), -1 * ( GetTall() / 10.0 ) + XRES( 4 ), ( GetWide() * 1.5 ) - XRES( 8 ), ( GetWide() * 0.75 ) - XRES( 8 ) );
	}

	m_pWeaponName->SetText( pText );

	if ( ID != -1 )
	{
		char szSlotID[8];
		Q_snprintf( szSlotID, sizeof( szSlotID ), "%d", m_ID + 1 );
		m_pSlotID->SetText( szSlotID );
	}
	else
	{
		m_pSlotID->SetText( "" );
	}

	InvalidateLayout( true );
}

void CItemModelPanel::SetWeapon( CEconItemDefinition *pItemDefinition, int iBorderStyle, int ID )
{
	m_pWeapon = NULL;
	m_ID = ID;
	m_iBorderStyle = iBorderStyle;

	wchar_t *pText = NULL;
	if ( pItemDefinition )
	{
		pText = g_pVGuiLocalize->Find( pItemDefinition->item_name );
		char szImage[128];
		Q_snprintf( szImage, sizeof( szImage ), "../%s_large", pItemDefinition->image_inventory );
		m_pWeaponImage->SetImage( szImage );
		m_pWeaponImage->SetBounds( XRES( 4 ), -1 * ( GetTall() / 5.0 ) + XRES( 4 ), GetWide() - XRES( 8 ), GetWide() - XRES( 8 ) );
	}

	m_pWeaponName->SetText( pText );
	if ( ID != -1 )
	{
		char szSlotID[8];
		Q_snprintf( szSlotID, sizeof( szSlotID ), "%d", m_ID + 1 );
		m_pSlotID->SetText( szSlotID );
	}
	else
	{
		m_pSlotID->SetText( "" );
	}

	InvalidateLayout( true );
}



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudWeaponSwitch::CTFHudWeaponSwitch( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudWeaponSwitch" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pItemDefFrom = NULL;
	m_pItemDefTo = NULL;

	m_pWeaponFrom = new CItemModelPanel( this, "WeaponFrom" );
	m_pWeaponTo = new CItemModelPanel( this, "WeaponTo" );

	ivgui()->AddTickSignal( GetVPanel() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudWeaponSwitch::ShouldDraw( void )
{
	if ( IsTakingAFreezecamScreenshot() )
		return false;

	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pPlayer || !pPlayer->IsAlive() )
		return false;

	int iWeaponTo = pPlayer->m_Shared.GetDesiredWeaponIndex();
	if ( iWeaponTo == -1 )
		return false;

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponSwitch::SetVisible( bool bVisible )
{
	if ( bVisible )
	{
		const char *key = engine->Key_LookupBinding( "+use" );
		if ( !key )
		{
			key = "< not bound >";
		}
		SetDialogVariable( "use", key );
	}

	BaseClass::SetVisible( bVisible );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponSwitch::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/HudWeaponSwitch.res" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponSwitch::OnTick()
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pPlayer )
		return;

	int iWeaponTo = pPlayer->m_Shared.GetDesiredWeaponIndex();
	if ( iWeaponTo == -1 )
		return;

	CEconItemDefinition *pItemTo = GetItemSchema()->GetItemDefinition( iWeaponTo );
	if ( !pItemTo )
		return;

	if ( pItemTo != m_pItemDefTo )
	{
		m_pItemDefTo = pItemTo;
		m_pWeaponTo->SetWeapon( m_pItemDefTo );
	}

	int iSlot = pItemTo->GetLoadoutSlot( TF_CLASS_MERCENARY );
	C_EconEntity *pWeaponFrom = pPlayer->GetEntityForLoadoutSlot( iSlot );
	if ( !pWeaponFrom )
		return;

	CEconItemDefinition *pItemFrom = pWeaponFrom->GetItem()->GetStaticData();

	if ( pItemFrom != m_pItemDefFrom )
	{
		m_pItemDefFrom = pItemFrom;
		m_pWeaponFrom->SetWeapon( m_pItemDefFrom );
	}
}
