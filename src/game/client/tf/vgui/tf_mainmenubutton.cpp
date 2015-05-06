//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================


#include "cbase.h"
#include "tf_mainmenubutton.h"
#include "vgui_controls/Frame.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include "vgui_controls/Button.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_controls.h"
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>
#include "basemodelpanel.h"


using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFMainMenuButton, CTFMainMenuButton);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuButton::CTFMainMenuButton(vgui::Panel *parent, const char *panelName, const char *text) : CExButton(parent, panelName, text)
{
	pImage = new CTFImagePanel(this, "TestImage");
	pButton = new CTFButton(this, "TestText", text);
	pButton->SetParent(this);

	vgui::ivgui()->AddTickSignal(GetVPanel());
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	Q_strncpy(pImageIdle, inResourceData->GetString("imageIdle", ""), sizeof(pImageIdle));
	Q_strncpy(pImageHover, inResourceData->GetString("imageHover", ""), sizeof(pImageHover));
	Q_strncpy(pImageClick, inResourceData->GetString("imageClick", ""), sizeof(pImageClick));
	Q_strncpy(m_szCommand, inResourceData->GetString("command", ""), sizeof(m_szCommand));

	m_bOnlyInGame = inResourceData->GetFloat("onlyingame", false);
	m_bImageVisible = inResourceData->GetFloat("imagevisible", false);	

	pImage->SetImage(pImageIdle);
	pButton->SetCommand(m_szCommand);

	GetText(m_szText, sizeof(m_szText));
	pButton->SetText(m_szText);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	pImage->SetVisible(m_bImageVisible);
	pImage->SetEnabled(true);
	pImage->SetPos(0, 0);
	pImage->SetZPos(1);
	pImage->SetWide(GetWide());
	pImage->SetTall(GetTall());
	pImage->SetShouldScaleImage(true);

	pButton->SetVisible(true);
	pButton->SetEnabled(true);
	pButton->SetPos(0, 0);
	pButton->SetZPos(2);
	pButton->SetWide(GetWide());
	pButton->SetTall(GetTall());
	pButton->SetContentAlignment(a_center);
	pButton->SetFont(GetFont());

	SetDefaultColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetArmedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetDepressedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetSelectedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));

	pButton->SetDefaultColor(GetFgColor(), Color(0, 0, 0, 0));
	pButton->SetArmedColor(GetFgColor(), Color(0, 0, 0, 0));
	pButton->SetDepressedColor(GetFgColor(), Color(0, 0, 0, 0));
	pButton->SetSelectedColor(GetFgColor(), Color(0, 0, 0, 0));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::SetDefaultAnimation()
{
	pImage->SetImage(pImageIdle);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::SendAnimation(MouseState flag)
{
	switch (flag)
	{
	case MOUSE_DEFAULT:
		pImage->SetImage(pImageIdle);
		break;
	case MOUSE_ENTERED:
		pImage->SetImage(pImageHover);
		break;
	case MOUSE_EXITED:
		pImage->SetImage(pImageIdle);
		break;
	case MOUSE_PRESSED:
		pImage->SetImage(pImageClick);
		break;
	default:
		break;
	}
}

///
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFButton::CTFButton(vgui::Panel *parent, const char *panelName, const char *text) : CExButton(parent, panelName, text)
{
	m_flHoverTimeToWait = -1;
	m_flHoverTime = -1;
	m_bMouseEntered = false;
	iState = MOUSE_DEFAULT;
	vgui::ivgui()->AddTickSignal(GetVPanel());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_flHoverTimeToWait = inResourceData->GetFloat("hover", -1);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnCursorEntered()
{
	BaseClass::OnCursorEntered();
	if (iState != MOUSE_ENTERED)
	{
		SetMouseEnteredState(MOUSE_ENTERED);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnCursorExited()
{
	BaseClass::OnCursorExited();
	if (iState != MOUSE_EXITED)
	{
		SetMouseEnteredState(MOUSE_EXITED);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnMousePressed(vgui::MouseCode code)
{
	BaseClass::OnMousePressed(code);
	if (iState != MOUSE_PRESSED)
	{
		SetMouseEnteredState(MOUSE_PRESSED);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnMouseReleased(vgui::MouseCode code)
{
	BaseClass::OnMouseReleased(code);
	if (iState == MOUSE_ENTERED || iState == MOUSE_PRESSED)
	{
		engine->ExecuteClientCmd(m_pParent->m_szCommand);
	}
	if (iState != MOUSE_ENTERED)
	{
		SetMouseEnteredState(MOUSE_ENTERED);
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::SetMouseEnteredState(MouseState flag)
{
	iState = flag;

	if (flag == MOUSE_ENTERED)
	{
		m_bMouseEntered = true;

		if (m_flHoverTimeToWait > 0)
		{
			m_flHoverTime = gpGlobals->curtime + m_flHoverTimeToWait;
		}
		else
		{
			m_flHoverTime = -1;
		}
		m_pParent->SendAnimation(flag);
	}
	else
	{
		m_bMouseEntered = false;
		m_flHoverTime = -1;

		m_pParent->SendAnimation(flag);
	}
}