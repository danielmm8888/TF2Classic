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

	Q_strncpy(pDefaulImage, inResourceData->GetString("DefaulImage", ""), sizeof(pDefaulImage));
	Q_strncpy(pArmedImage, inResourceData->GetString("ArmedImage", ""), sizeof(pArmedImage));
	Q_strncpy(pDepressedImage, inResourceData->GetString("DepressedImage", ""), sizeof(pDepressedImage));

	Q_strncpy(pDefaulBorder, inResourceData->GetString("DefaultBorder", ""), sizeof(pDefaulBorder));
	Q_strncpy(pArmedBorder, inResourceData->GetString("ArmedBorder", ""), sizeof(pArmedBorder));
	Q_strncpy(pDepressedBorder, inResourceData->GetString("DepressedBorder", ""), sizeof(pDepressedBorder));

	Q_strncpy(pDefaulText, inResourceData->GetString("DefaultText", ""), sizeof(pDefaulText));
	Q_strncpy(pArmedText, inResourceData->GetString("ArmedText", ""), sizeof(pArmedText));
	Q_strncpy(pDepressedText, inResourceData->GetString("DepressedText", ""), sizeof(pDepressedText));

	Q_strncpy(m_szCommand, inResourceData->GetString("command", ""), sizeof(m_szCommand));

	m_bOnlyInGame = inResourceData->GetFloat("onlyingame", false);
	m_bImageVisible = inResourceData->GetFloat("imagevisible", false);	
	m_bBorderVisible = inResourceData->GetFloat("bordervisible", false);

	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	pImage->SetImage(pDefaulImage);
	pButton->SetCommand(m_szCommand);

	GetText(m_szText, sizeof(m_szText));
	pButton->SetText(m_szText);

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

	pButton->SetDefaultColor(pScheme->GetColor(pDefaulText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetArmedColor(pScheme->GetColor(pArmedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetDepressedColor(pScheme->GetColor(pDepressedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetSelectedColor(pScheme->GetColor(pDepressedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));

	if (m_bBorderVisible)
	{
		pButton->SetDefaultBorder(pScheme->GetBorder(pDefaulBorder));
		pButton->SetArmedBorder(pScheme->GetBorder(pArmedBorder));
		pButton->SetDepressedBorder(pScheme->GetBorder(pDepressedBorder));
		pButton->SetSelectedBorder(pScheme->GetBorder(pDepressedBorder));
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::SetDefaultAnimation()
{
	pImage->SetImage(pDefaulImage);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::SendAnimation(MouseState flag)
{
	switch (flag)
	{
	//We can add additional stuff like animation here
	case MOUSE_DEFAULT:
		pImage->SetImage(pDefaulImage);
		break;
	case MOUSE_ENTERED:
		pImage->SetImage(pArmedImage);
		break;
	case MOUSE_EXITED:
		pImage->SetImage(pDefaulImage);
		break;
	case MOUSE_PRESSED:
		pImage->SetImage(pDepressedImage);
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
	//m_flHoverTimeToWait = inResourceData->GetFloat("hover", -1);
}



//-----------------------------------------------------------------------------
// Purpose: Set armed button border attributes.
//-----------------------------------------------------------------------------
void CTFButton::SetArmedBorder(IBorder *border)
{
	_armedBorder = border;
	InvalidateLayout(false);
}

//-----------------------------------------------------------------------------
// Purpose: Set selected button border attributes.
//-----------------------------------------------------------------------------
void CTFButton::SetSelectedBorder(IBorder *border)
{
	_selectedBorder = border;
	InvalidateLayout(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	_armedBorder = pScheme->GetBorder("ButtonArmedBorder");
	_selectedBorder = pScheme->GetBorder("ButtonSelectedBorder");
	InvalidateLayout();
}


//-----------------------------------------------------------------------------
// Purpose: Get button border attributes.
//-----------------------------------------------------------------------------
IBorder *CTFButton::GetBorder(bool depressed, bool armed, bool selected, bool keyfocus)
{
	if (depressed)
		return _depressedBorder;
	if (armed)
		return _armedBorder;
	if (selected)
		return _selectedBorder;

	return _defaultBorder;
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
	m_pParent->SendAnimation(flag);

	/*
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

	}
	else
	{
		m_bMouseEntered = false;
		m_flHoverTime = -1;

		m_pParent->SendAnimation(flag);
	}
	*/
}