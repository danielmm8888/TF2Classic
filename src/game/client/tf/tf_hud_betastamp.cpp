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

DECLARE_HUDELEMENT(CTFHudBetaStamp);
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudBetaStamp::CTFHudBetaStamp( const char *pElementName ) : CHudElement(pElementName), BaseClass(NULL, "TFHudBetaStamp")
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	bGlowing = false;
	m_flAnimationThink = 0.0f;
	m_pBetaImage = new ImagePanel(this, "BetaStamp");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

void CTFHudBetaStamp::PerformLayout(void)
{
	BaseClass::PerformLayout();

	SetBounds(0, 0, XRES(100), XRES(100));

	m_pBetaImage->SetVisible(true);
	m_pBetaImage->SetBounds(XRES(15), 0, XRES(60), XRES(60));
	m_pBetaImage->SetShouldScaleImage(true);
	m_pBetaImage->SetImage("../vgui/main_menu/beta_stamp");
	m_pBetaImage->SetDrawColor(Color(200, 0, 20, 180));
	bGlowing = true;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudBetaStamp::OnThink()
{
	if (bGlowing && m_flAnimationThink < gpGlobals->curtime)
	{
		float m_fAlpha = (m_bAnimationIn ? 120.0f : 255.0f);
		float m_fDelay = (m_bAnimationIn ? 0.75f : 0.0f);
		float m_fDuration = (m_bAnimationIn ? 0.15f : 0.25f);
		vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", m_fAlpha, m_fDelay, m_fDuration, vgui::AnimationController::INTERPOLATOR_LINEAR);
		m_flAnimationThink = gpGlobals->curtime + (m_bAnimationIn ? 1.0f : 3.0f);
		m_bAnimationIn = !m_bAnimationIn;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudBetaStamp::ShouldDraw(void)
{
	return true;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudBetaStamp::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );
}