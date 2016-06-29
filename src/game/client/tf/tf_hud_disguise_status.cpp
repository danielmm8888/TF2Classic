//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: HUD Target ID element
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "c_tf_player.h"
#include "c_playerresource.h"
#include "iclientmode.h"
#include "vgui/ILocalize.h"
#include "vgui/ISurface.h"
#include <vgui_controls/Panel.h>
#include <vgui_controls/EditablePanel.h>
#include "tf_spectatorgui.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CDisguiseStatus : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CDisguiseStatus, vgui::EditablePanel );

public:
	CDisguiseStatus( const char *pElementName );
	void			Init( void );
	virtual void	ApplySchemeSettings( vgui::IScheme *scheme );
	virtual void	Paint( void );
	virtual bool	ShouldDraw( void );
	void			HideStatus( void );
	void			ShowAndUpdateStatus( void );
	void			CheckWeapon( void );

private:
	CPanelAnimationVar( vgui::HFont, m_hFont, "TextFont", "TargetID" );
	CTFImagePanel		*m_pDisguiseStatusBG;
	vgui::Label			*m_pDisguiseNameLabel;
	vgui::Label			*m_pWeaponNameLabel;
	CTFSpectatorGUIHealth	*m_pTargetHealth;
	//CEmbeddedItemModelPanel *m_pItemModelPanel;
	
	bool				m_bVisible;
	int					m_iCurrentDisguiseTeam;
};

DECLARE_HUDELEMENT( CDisguiseStatus );

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CDisguiseStatus::CDisguiseStatus( const char *pElementName ) :
	CHudElement( pElementName ), BaseClass( NULL, "DisguiseStatus" )
{
	vgui::Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pTargetHealth = new CTFSpectatorGUIHealth( this, "SpectatorGUIHealth" );

	SetHiddenBits( HIDEHUD_MISCSTATUS );
	m_iCurrentDisguiseTeam = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Setup
//-----------------------------------------------------------------------------
void CDisguiseStatus::Init( void )
{
	HideStatus();
}

//-----------------------------------------------------------------------------
// Purpose: Hide all elements
//-----------------------------------------------------------------------------
void CDisguiseStatus::HideStatus( void )
{
	if ( m_pDisguiseStatusBG )
		m_pDisguiseStatusBG->SetVisible( false );

	if ( m_pDisguiseNameLabel )
		m_pDisguiseNameLabel->SetVisible( false );

	if ( m_pWeaponNameLabel )
		m_pWeaponNameLabel->SetVisible( false );

	if ( m_pTargetHealth )
		m_pTargetHealth->SetVisible( false );

	m_bVisible = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CDisguiseStatus::ShouldDraw( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer )
		return false;

	if ( pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
	{
		if ( !m_bVisible || m_iCurrentDisguiseTeam != pPlayer->m_Shared.GetDisguiseTeam() )
			ShowAndUpdateStatus();

		return true;
	}
	else 
	{
		HideStatus();
		return false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDisguiseStatus::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );
	LoadControlSettings( "resource/UI/DisguiseStatusPanel.res" );

	m_pTargetHealth = dynamic_cast< CTFSpectatorGUIHealth *>( FindChildByName( "SpectatorGUIHealth" ) );
	//m_pItemModelPanel = dynamic_cast< CEmbeddedItemModelPanel *>( FindChildByName( "CEmbeddedItemModelPanel" ) );
	m_pDisguiseStatusBG = dynamic_cast< CTFImagePanel * >( FindChildByName( "DisguiseStatusBG" ) );
	m_pDisguiseNameLabel = dynamic_cast< vgui::Label *>( FindChildByName( "DisguiseNameLabel" ) );
	m_pWeaponNameLabel = dynamic_cast< vgui::Label *>( FindChildByName( "WeaponNameLabel" ) );

	SetPaintBackgroundEnabled( false );

	HideStatus();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDisguiseStatus::CheckWeapon( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer )
		return;

	CEconItemDefinition *pItem = pPlayer->m_Shared.GetDisguiseItem()->GetStaticData();
	if ( pItem )
		SetDialogVariable( "weaponname", g_pVGuiLocalize->Find( pItem->item_name ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDisguiseStatus::ShowAndUpdateStatus( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer )
		return;

	m_iCurrentDisguiseTeam = pPlayer->m_Shared.GetDisguiseTeam();

	if ( m_pDisguiseStatusBG )
	{
		m_pDisguiseStatusBG->SetVisible( true );

		// This isn't how live tf2 does it, they simply call UpdateBGImage, however I'm not sure what exactly
		// they're doing.
		m_pDisguiseStatusBG->SetBGImage( m_iCurrentDisguiseTeam );
		//m_pDisguiseStatusBG->UpdateBGImage();
	}

	if ( m_pDisguiseNameLabel )
	{
		m_pDisguiseNameLabel->SetVisible( true );
		C_BasePlayer *pDisguiseTarget = ToBasePlayer( pPlayer->m_Shared.GetDisguiseTarget() );
		if ( pDisguiseTarget )
			SetDialogVariable( "disguisename", pDisguiseTarget->GetPlayerName() );
	}

	if ( m_pWeaponNameLabel )
	{
		m_pWeaponNameLabel->SetVisible( true );
		CheckWeapon();
	}

	if ( m_pTargetHealth )
	{
		m_pTargetHealth->SetVisible( true );
		m_pTargetHealth->SetHealth( pPlayer->m_Shared.GetDisguiseHealth(), pPlayer->m_Shared.GetDisguiseMaxHealth(), pPlayer->m_Shared.GetDisguiseMaxBuffedHealth() );
	}

	m_bVisible = true;
}

//-----------------------------------------------------------------------------
// Purpose: Draw function for the element
//-----------------------------------------------------------------------------
void CDisguiseStatus::Paint( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer )
		return;

	// We don't print anything until we're fully disguised
	if ( !pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
		return;

	C_BasePlayer *pDisguiseTarget = ToBasePlayer( pPlayer->m_Shared.GetDisguiseTarget() );
	if ( pDisguiseTarget )
	{
		SetDialogVariable( "disguisename", pDisguiseTarget->GetPlayerName() );
	}

	CheckWeapon();
	
	if ( m_pTargetHealth )
		m_pTargetHealth->SetHealth( pPlayer->m_Shared.GetDisguiseHealth(), pPlayer->m_Shared.GetDisguiseMaxHealth(), pPlayer->m_Shared.GetDisguiseMaxBuffedHealth() );
}
