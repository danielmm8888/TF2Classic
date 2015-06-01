#include "cbase.h"
#include "tf_mainmenucheckbutton.h"
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

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFMainMenuCheckButton, CTFMainMenuButtonBase);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuCheckButton::CTFMainMenuCheckButton(vgui::Panel *parent, const char *panelName, const char *text) : CTFMainMenuButtonBase(parent, panelName, text)
{
	pButton = new CTFCheckButton(this, "ButtonNew", text);
	pCheckImage = new CTFImagePanel(this, "CheckImageNew");
	pButton->SetParent(this);
	pCheckImage->SetParent(this);
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuCheckButton::~CTFMainMenuCheckButton()
{
	delete pButton;
	delete pCheckImage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::Init()
{
	BaseClass::Init();
	Q_strncpy(m_szValueFalse, "0", sizeof(m_szValueFalse));
	Q_strncpy(m_szValueTrue, "1", sizeof(m_szValueTrue));
	Q_strncpy(pDefaultImage, DEFAULT_IMAGE, sizeof(pDefaultImage));
	Q_strncpy(pArmedImage, ARMED_IMAGE, sizeof(pArmedImage));
	Q_strncpy(pDepressedImage, DEPRESSED_IMAGE, sizeof(pDepressedImage));
	Q_strncpy(pDefaultCheckImage, DEFAULT_CHECKIMAGE, sizeof(pDefaultCheckImage));
	Q_strncpy(pArmedCheckImage, ARMED_CHECKIMAGE, sizeof(pArmedCheckImage));
	Q_strncpy(pDepressedCheckImage, DEPRESSED_CHECKIMAGE, sizeof(pDepressedCheckImage));
	m_bState = false;
	m_bImageVisible = true;
	m_bBorderVisible = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_bState = inResourceData->GetBool("defaultstate", false);
	Q_strncpy(m_szValueFalse, inResourceData->GetString("valuefalse", "0"), sizeof(m_szValueFalse));
	Q_strncpy(m_szValueTrue, inResourceData->GetString("valuetrue", "1"), sizeof(m_szValueTrue));

	Q_strncpy(pDefaultCheckImage, inResourceData->GetString("DefaultCheckImage", DEFAULT_CHECKIMAGE), sizeof(pDefaultCheckImage));
	Q_strncpy(pArmedCheckImage, inResourceData->GetString("ArmedCheckImage", ARMED_CHECKIMAGE), sizeof(pArmedCheckImage));
	Q_strncpy(pDepressedCheckImage, inResourceData->GetString("DepressedCheckImage", DEPRESSED_CHECKIMAGE), sizeof(pDepressedCheckImage));

	GetCommandValue();
	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	pButton->SetDefaultColor(pScheme->GetColor(DEFAULT_TEXT, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetArmedColor(pScheme->GetColor(ARMED_TEXT, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetDepressedColor(pScheme->GetColor(DEPRESSED_TEXT, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetSelectedColor(pScheme->GetColor(DEPRESSED_TEXT, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	//pButton->SetFont(pScheme->GetFont(m_szFont));
	if (m_bBorderVisible)
	{
		pButton->SetDefaultBorder(pScheme->GetBorder(pDefaultBorder));
		pButton->SetArmedBorder(pScheme->GetBorder(pArmedBorder));
		pButton->SetDepressedBorder(pScheme->GetBorder(pDepressedBorder));
		pButton->SetSelectedBorder(pScheme->GetBorder(pDepressedBorder));
	}
	else
	{
		pButton->SetDefaultBorder(pScheme->GetBorder(EMPTY_STRING));
		pButton->SetArmedBorder(pScheme->GetBorder(EMPTY_STRING));
		pButton->SetDepressedBorder(pScheme->GetBorder(EMPTY_STRING));
		pButton->SetSelectedBorder(pScheme->GetBorder(EMPTY_STRING));
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::PerformLayout()
{
	BaseClass::PerformLayout();

	GetText(m_szText, sizeof(m_szText));
	pButton->SetText(m_szText);
	pButton->SetCommand(GetCommandString());
	pButton->SetVisible(true);
	pButton->SetEnabled(true);
	pButton->SetFont(GETSCHEME()->GetFont(m_szFont));
	pButton->SetPos(0, 0);
	pButton->SetZPos(2);
	pButton->SetWide(GetWide());
	pButton->SetTall(GetTall());
	pButton->SetContentAlignment(GetAlignment(m_szTextAlignment));
	//pButton->SetFont(GetFont());
	pButton->SetArmedSound("ui/buttonrollover.wav");
	pButton->SetDepressedSound("ui/buttonclick.wav");
	pButton->SetReleasedSound("ui/buttonclickrelease.wav");

	pImage->SetPos(GetWide() - GetTall(), 0);
	pImage->SetWide(GetTall());
	pCheckImage->SetImage(pDefaultImage);
	pCheckImage->SetVisible(m_bImageVisible);
	pCheckImage->SetEnabled(true);
	pCheckImage->SetPos(GetWide() - GetTall(), 0);
	pCheckImage->SetZPos(2);
	pCheckImage->SetWide(GetTall());
	pCheckImage->SetTall(GetTall());
	pCheckImage->SetShouldScaleImage(true);

	GetCommandValue();
	SetDefaultAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::OnThink()
{
	BaseClass::OnThink();
	//GetCommandValue();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::GetCommandValue()
{
	if (Q_strcmp(GetCommandString(), EMPTY_STRING))
	{
		ConVarRef CheckButtonCommand(GetCommandString());
		//Msg("Command %s %s\n", GetCommandString(), CheckButtonCommand.GetString());
		if (!Q_strcmp(CheckButtonCommand.GetString(), m_szValueFalse))
		{
			m_bState = false;
		}
		else if (!Q_strcmp(CheckButtonCommand.GetString(), m_szValueTrue))
		{
			m_bState = true;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::OnTick()
{
	BaseClass::OnTick();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::SetDefaultAnimation()
{
	SendAnimation(MOUSE_DEFAULT);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuCheckButton::SendAnimation(MouseState flag)
{
	BaseClass::SendAnimation(flag);
	pCheckImage->SetVisible(m_bState);
	switch (flag)
	{
		//We can add additional stuff like animation here
	case MOUSE_DEFAULT:
		pCheckImage->SetImage(pDefaultCheckImage);
		break;
	case MOUSE_ENTERED:
		pCheckImage->SetImage(pArmedCheckImage);
		break;
	case MOUSE_EXITED:
		pCheckImage->SetImage(pDefaultCheckImage);
		break;
	case MOUSE_PRESSED:
		pCheckImage->SetImage(pDepressedCheckImage);
		break;
	default:
		pCheckImage->SetImage(pDefaultCheckImage);
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFCheckButton::CTFCheckButton(vgui::Panel *parent, const char *panelName, const char *text) : CTFButtonBase(parent, panelName, text)
{
	iState = MOUSE_DEFAULT;
	vgui::ivgui()->AddTickSignal(GetVPanel());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCheckButton::OnCursorEntered()
{
	//BaseClass::OnCursorEntered();
	BaseClass::BaseClass::OnCursorEntered();
	if (iState != MOUSE_ENTERED)
	{
		SetMouseEnteredState(MOUSE_ENTERED);
	}
	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCheckButton::OnCursorExited()
{
	BaseClass::BaseClass::OnCursorExited();
	
	if (iState != MOUSE_EXITED)
	{
		SetMouseEnteredState(MOUSE_EXITED);
	}
	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCheckButton::OnMousePressed(vgui::MouseCode code)
{
	BaseClass::BaseClass::OnMousePressed(code);
	
	if (code == MOUSE_LEFT && iState != MOUSE_PRESSED)
	{
		SetMouseEnteredState(MOUSE_PRESSED);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCheckButton::OnMouseReleased(vgui::MouseCode code)
{
	BaseClass::BaseClass::OnMouseReleased(code);
	
	if (code == MOUSE_LEFT && (iState == MOUSE_ENTERED || iState == MOUSE_PRESSED))
	{
		m_pParent->m_bState = !m_pParent->m_bState;
		if (Q_strcmp(GetCommandStr(), EMPTY_STRING) && m_pParent->IsAutoChange())
		{
			char sCommand[30];
			Q_snprintf(sCommand, sizeof(sCommand), "%s %s", GetCommandStr(), GetCommandValue(m_pParent->m_bState));
			engine->ExecuteClientCmd(sCommand);
			//m_pParent->GetParent()->OnCommand(sCommand);
		}
	}
	if (code == MOUSE_LEFT && iState == MOUSE_ENTERED)
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
void CTFCheckButton::SetMouseEnteredState(MouseState flag)
{
	BaseClass::SetMouseEnteredState(flag);
	if (!m_pParent->IsDisabled())
		m_pParent->SendAnimation(flag);
}