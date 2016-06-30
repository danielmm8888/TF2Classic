//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "tf_hud_betastamp.h"
#include "tf_gamerules.h"
#include "tf_hud_freezepanel.h"
#include "tf_mainmenu.h"

using namespace vgui;

DECLARE_HUDELEMENT( CTFHudBetaStamp );
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudBetaStamp::CTFHudBetaStamp( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudBetaStamp" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_bGlowing = false;
	m_flAnimationThink = 0.0f;
	m_pBetaImage = new ImagePanel( this, "BetaStampImage" );

	vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );
}

void CTFHudBetaStamp::PerformLayout( void )
{
	BaseClass::PerformLayout();

	m_bGlowing = true;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudBetaStamp::OnThink()
{
	if ( m_bGlowing && m_flAnimationThink < gpGlobals->curtime )
	{
		float m_fAlpha = ( m_bAnimationIn ? 120.0f : 255.0f );
		float m_fDelay = ( m_bAnimationIn ? 0.75f : 0.0f );
		float m_fDuration = ( m_bAnimationIn ? 0.15f : 0.25f );
		vgui::GetAnimationController()->RunAnimationCommand( this, "Alpha", m_fAlpha, m_fDelay, m_fDuration, vgui::AnimationController::INTERPOLATOR_LINEAR );
		m_flAnimationThink = gpGlobals->curtime + ( m_bAnimationIn ? 1.0f : 3.0f );
		m_bAnimationIn = !m_bAnimationIn;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudBetaStamp::ShouldDraw( void )
{
	return false;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudBetaStamp::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/HudBetaStamp.res" );
}
