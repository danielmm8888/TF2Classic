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

	Q_strncpy(pDefaultImage, inResourceData->GetString("DefaultImage", ""), sizeof(pDefaultImage));
	Q_strncpy(pArmedImage, inResourceData->GetString("ArmedImage", ""), sizeof(pArmedImage));
	Q_strncpy(pDepressedImage, inResourceData->GetString("DepressedImage", ""), sizeof(pDepressedImage));

	Q_strncpy(pDefaultBorder, inResourceData->GetString("DefaultBorder", ""), sizeof(pDefaultBorder));
	Q_strncpy(pArmedBorder, inResourceData->GetString("ArmedBorder", ""), sizeof(pArmedBorder));
	Q_strncpy(pDepressedBorder, inResourceData->GetString("DepressedBorder", ""), sizeof(pDepressedBorder));

	Q_strncpy(pDefaultText, inResourceData->GetString("DefaultText", ""), sizeof(pDefaultText));
	Q_strncpy(pArmedText, inResourceData->GetString("ArmedText", ""), sizeof(pArmedText));
	Q_strncpy(pDepressedText, inResourceData->GetString("DepressedText", ""), sizeof(pDepressedText));

	Q_strncpy(m_szCommand, inResourceData->GetString("command", ""), sizeof(m_szCommand));
	Q_strncpy(m_szTextAlignment, inResourceData->GetString("textAlignment", "center"), sizeof(m_szTextAlignment));		

	m_bOnlyInGame = inResourceData->GetBool("onlyingame", false);
	m_bOnlyAtMenu = inResourceData->GetBool("onlyatmenu", false);
	m_bImageVisible = inResourceData->GetBool("imagevisible", false);	
	m_bBorderVisible = inResourceData->GetBool("bordervisible", false);

	m_fXShift = inResourceData->GetFloat("xshift", 0.0);
	m_fYShift = inResourceData->GetFloat("yshift", 0.0);

	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	pButton->SetCommand(m_szCommand);
	GetText(m_szText, sizeof(m_szText));
	pButton->SetText(m_szText);
	pImage->SetImage(pDefaultImage);

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
	pButton->SetContentAlignment(GetAlignment(m_szTextAlignment));

	pButton->SetFont(GetFont());

	SetWide(GetWide() + 30);

	SetDefaultColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetArmedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetDepressedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetSelectedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));

	pButton->SetDefaultColor(pScheme->GetColor(pDefaultText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetArmedColor(pScheme->GetColor(pArmedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetDepressedColor(pScheme->GetColor(pDepressedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetSelectedColor(pScheme->GetColor(pDepressedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));

	if (m_bBorderVisible)
	{
		pButton->SetDefaultBorder(pScheme->GetBorder(pDefaultBorder));
		pButton->SetArmedBorder(pScheme->GetBorder(pArmedBorder));
		pButton->SetDepressedBorder(pScheme->GetBorder(pDepressedBorder));
		pButton->SetSelectedBorder(pScheme->GetBorder(pDepressedBorder));
	}

	pButton->SetArmedSound("ui/buttonrollover.wav");
	pButton->SetDepressedSound("ui/buttonclick.wav");
	pButton->SetReleasedSound("ui/buttonclickrelease.wav");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::OnThink()
{
	BaseClass::OnThink();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::SetDefaultAnimation()
{
	pImage->SetImage(pDefaultImage);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButton::SendAnimation(MouseState flag)
{
	AnimationController::PublicValue_t p_AnimLeave = { 0, 0, 0, 0 };
	AnimationController::PublicValue_t p_AnimHover = { m_fXShift, m_fYShift, 0, 0 };
	switch (flag)
	{
	//We can add additional stuff like animation here
	case MOUSE_DEFAULT:
		pImage->SetImage(pDefaultImage);
		break;
	case MOUSE_ENTERED:
		pImage->SetImage(pArmedImage);
		vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimHover, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		break;
	case MOUSE_EXITED:
		pImage->SetImage(pDefaultImage);
		vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimLeave, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
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
	iState = MOUSE_DEFAULT;
	vgui::ivgui()->AddTickSignal(GetVPanel());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);
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
		m_pParent->GetParent()->OnCommand(m_pParent->m_szCommand);
	}
	if (iState == MOUSE_ENTERED)
	{
		SetMouseEnteredState(MOUSE_ENTERED);
	} 
	else
	{
		SetMouseEnteredState(MOUSE_EXITED);
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::SetMouseEnteredState(MouseState flag)
{
	iState = flag;
	m_pParent->SendAnimation(flag);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
vgui::Label::Alignment CTFMainMenuButton::GetAlignment(char* m_szAlignment)
{
	// text alignment
	const char *alignmentString = m_szAlignment;
	int align = -1;

	if (!stricmp(alignmentString, "north-west"))
	{
		align = a_northwest;
	}
	else if (!stricmp(alignmentString, "north"))
	{
		align = a_north;
	}
	else if (!stricmp(alignmentString, "north-east"))
	{
		align = a_northeast;
	}
	else if (!stricmp(alignmentString, "west"))
	{
		align = a_west;
	}
	else if (!stricmp(alignmentString, "center"))
	{
		align = a_center;
	}
	else if (!stricmp(alignmentString, "east"))
	{
		align = a_east;
	}
	else if (!stricmp(alignmentString, "south-west"))
	{
		align = a_southwest;
	}
	else if (!stricmp(alignmentString, "south"))
	{
		align = a_south;
	}
	else if (!stricmp(alignmentString, "south-east"))
	{
		align = a_southeast;
	}

	if (align != -1)
	{
		return (Alignment)align;
	}

	return a_center;
}
