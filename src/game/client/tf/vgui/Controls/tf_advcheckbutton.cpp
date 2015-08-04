#include "cbase.h"
#include "tf_advcheckbutton.h"
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

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFAdvCheckButton, CTFAdvButtonBase);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvCheckButton::CTFAdvCheckButton(vgui::Panel *parent, const char *panelName, const char *text) : CTFAdvButtonBase(parent, panelName, text)
{
	pButton = new CTFCheckButton(this, "ButtonNew", text);
	pCheckImage = new ImagePanel(this, "CheckImageNew");
	pButton->SetParent(this);
	pCheckImage->SetParent(this);
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvCheckButton::~CTFAdvCheckButton()
{
	delete pButton;
	delete pCheckImage;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::Init()
{
	BaseClass::Init();
	Q_strncpy(m_szValueFalse, "0", sizeof(m_szValueFalse));
	Q_strncpy(m_szValueTrue, "1", sizeof(m_szValueTrue));
	Q_strncpy(pDefaultBG, DEFAULT_BG, sizeof(pDefaultBG));
	Q_strncpy(pArmedBG, ARMED_BG, sizeof(pArmedBG));
	Q_strncpy(pDepressedBG, DEPRESSED_BG, sizeof(pDepressedBG));
	Q_strncpy(pDefaultCheckImage, DEFAULT_CHECKIMAGE, sizeof(pDefaultCheckImage));
	m_bState = false;
	m_bBGVisible = true;
	m_bBorderVisible = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_bState = inResourceData->GetBool("defaultstate", false);
	Q_strncpy(m_szValueFalse, inResourceData->GetString("valuefalse", "0"), sizeof(m_szValueFalse));
	Q_strncpy(m_szValueTrue, inResourceData->GetString("valuetrue", "1"), sizeof(m_szValueTrue));

	Q_strncpy(pDefaultCheckImage, inResourceData->GetString("DefaultCheckImage", DEFAULT_CHECKIMAGE), sizeof(pDefaultCheckImage));

	GetCommandValue();
	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	pButton->SetDefaultColor(pScheme->GetColor(DEFAULT_COLOR, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetArmedColor(pScheme->GetColor(ARMED_COLOR, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetDepressedColor(pScheme->GetColor(DEPRESSED_COLOR, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetSelectedColor(pScheme->GetColor(DEPRESSED_COLOR, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
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
void CTFAdvCheckButton::PerformLayout()
{
	BaseClass::PerformLayout();

	GetText(m_szText, sizeof(m_szText));
	pButton->SetText(m_szText);
	pButton->SetCommand(GetCommandString());
	pButton->SetVisible(true);
	pButton->SetEnabled(true);
	pButton->SetFont(GETSCHEME()->GetFont(m_szFont, true));
	pButton->SetPos(0, 0);
	pButton->SetZPos(4);
	pButton->SetTextInset(5, 0);
	pButton->SetWide(GetWide());
	pButton->SetTall(GetTall());
	pButton->SetContentAlignment(GetAlignment(m_szTextAlignment));
	//pButton->SetFont(GetFont());
	pButton->SetArmedSound("ui/buttonrollover.wav");
	pButton->SetDepressedSound("ui/buttonclick.wav");
	pButton->SetReleasedSound("ui/buttonclickrelease.wav");

	pBGBorder->SetPos(GetWide() - GetTall(), 0);
	pBGBorder->SetWide(GetTall());

	float h = GetProportionalTallScale();
	float fWidth = (m_fWidth == 0.0 ? GetTall() : m_fWidth * h);
	int iShift = (GetTall() - fWidth) / 2.0;

	pButtonImage->SetImage(pDefaultButtonImage);
	pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
	pButtonImage->SetVisible(IsVisible());
	pButtonImage->SetEnabled(IsEnabled());
	pButtonImage->SetPos(iShift, iShift);
	pButtonImage->SetZPos(2);
	pButtonImage->SetWide(fWidth);
	pButtonImage->SetTall(fWidth);
	pButtonImage->SetShouldScaleImage(true);
	

	pCheckImage->SetImage(pDefaultCheckImage);
	pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
	pCheckImage->SetVisible(m_bBGVisible);
	pCheckImage->SetEnabled(IsEnabled());
	pCheckImage->SetPos(GetWide() - GetTall() + iShift, iShift);
	//pCheckImage->SetPos(iShift, iShift);
	pCheckImage->SetZPos(3);
	//pCheckImage->SetWide(GetTall());
	//pCheckImage->SetTall(GetTall());
	pCheckImage->SetWide(fWidth);
	pCheckImage->SetTall(fWidth);
	pCheckImage->SetShouldScaleImage(true);

	GetCommandValue();
	SetDefaultAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::OnThink()
{
	BaseClass::OnThink();
	//GetCommandValue();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::GetCommandValue()
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
void CTFAdvCheckButton::OnTick()
{
	BaseClass::OnTick();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::SetDefaultAnimation()
{
	SendAnimation(MOUSE_DEFAULT);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::SendAnimation(MouseState flag)
{
	BaseClass::SendAnimation(flag);
	pCheckImage->SetVisible(m_bState);
	switch (flag)
	{
		//We can add additional stuff like animation here
	case MOUSE_DEFAULT:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
		pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
		//pCheckImage->SetImage(pDefaultCheckImage);
		break;
	case MOUSE_ENTERED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorArmed, Color(255, 255, 255, 255)));
		pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorArmed, Color(255, 255, 255, 255)));
		//pCheckImage->SetImage(pArmedCheckImage);
		break;
	case MOUSE_EXITED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
		pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
		//pCheckImage->SetImage(pDefaultCheckImage);
		break;
	case MOUSE_PRESSED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDepressed, Color(255, 255, 255, 255)));
		pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDepressed, Color(255, 255, 255, 255)));
		//pCheckImage->SetImage(pDepressedCheckImage);
		break;
	default:
		//pCheckImage->SetImage(pDefaultCheckImage);
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

	if (code == MOUSE_LEFT && (iState == MOUSE_ENTERED || iState == MOUSE_PRESSED) && !m_pParent->IsDisabled() && m_pParent->IsEnabled())
	{
		m_pParent->m_bState = !m_pParent->m_bState;
		m_pParent->PostActionSignal(new KeyValues("CheckButtonChecked"));

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
	if (!m_pParent->IsDisabled() && m_pParent->IsEnabled())
		m_pParent->SendAnimation(flag);
}