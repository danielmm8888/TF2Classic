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
	pButton = new CTFCheckButton(this, "SubButton", text);
	pCheckImage = new ImagePanel(this, "SubCheckImage");
	pBGBorder = new EditablePanel(this, "BackgroundPanel");
	pButton->SetParent(this);
	pCheckImage->SetParent(this);
	pBGBorder->SetParent(this);
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvCheckButton::~CTFAdvCheckButton()
{
	delete pButton;
	delete pCheckImage;
	delete pBGBorder;
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



	for (KeyValues *pData = inResourceData->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey())
	{
		if (!Q_stricmp(pData->GetName(), "SubButton"))
		{
			pButton->ApplySettings(pData);
		}
		if (!Q_stricmp(pData->GetName(), "SubImage"))
		{
			Q_strncpy(pDefaultCheckImage, pData->GetString("imagecheck", DEFAULT_CHECKIMAGE), sizeof(pDefaultCheckImage));
		}
	}

	GetCommandValue();
	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::PerformLayout()
{
	BaseClass::PerformLayout();

	float fWidth = (m_fImageWidth == 0.0 ? GetTall() : YRES(m_fImageWidth));
	int iShift = (GetTall() - fWidth) / 2.0;

	pButtonImage->SetImage(pDefaultButtonImage);
	pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
	pButtonImage->SetVisible(true);
	pButtonImage->SetPos(iShift, iShift);
	pButtonImage->SetZPos(2);
	pButtonImage->SetWide(fWidth);
	pButtonImage->SetTall(fWidth);
	pButtonImage->SetShouldScaleImage(true);
	

	pCheckImage->SetImage(pDefaultCheckImage);
	pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
	pCheckImage->SetVisible(m_bBorderVisible);
	pCheckImage->SetPos(GetWide() - GetTall() + iShift, iShift);
	pCheckImage->SetZPos(3);
	pCheckImage->SetWide(fWidth);
	pCheckImage->SetTall(fWidth);
	pCheckImage->SetShouldScaleImage(true);
	
	pBGBorder->SetBorder(GETSCHEME()->GetBorder(pSelectedBG));
	pBGBorder->SetVisible(true);
	pBGBorder->SetPos(GetWide() - GetTall(), 0);
	pBGBorder->SetZPos(1);
	pBGBorder->SetWide(GetTall());
	pBGBorder->SetTall(GetTall());

	GetCommandValue();
	SetDefaultAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::OnThink()
{
	BaseClass::OnThink();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvCheckButton::GetCommandValue()
{
	if ( GetCommandString()[0] != '\0' )
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
		pBGBorder->SetBorder(GETSCHEME()->GetBorder(pDefaultBG));
		break;
	case MOUSE_ENTERED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorArmed, Color(255, 255, 255, 255)));
		pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorArmed, Color(255, 255, 255, 255)));
		pBGBorder->SetBorder(GETSCHEME()->GetBorder(pArmedBG));
		break;
	case MOUSE_EXITED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
		pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
		pBGBorder->SetBorder(GETSCHEME()->GetBorder(pDefaultBG));
		break;
	case MOUSE_PRESSED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDepressed, Color(255, 255, 255, 255)));
		pCheckImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDepressed, Color(255, 255, 255, 255)));
		pBGBorder->SetBorder(GETSCHEME()->GetBorder(pDepressedBG));
		break;
	default:
		pBGBorder->SetBorder(GETSCHEME()->GetBorder(pDefaultBG));
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

void CTFCheckButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCheckButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	SetDefaultColor(pScheme->GetColor(DEFAULT_COLOR, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	SetArmedColor(pScheme->GetColor(ARMED_COLOR, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	SetDepressedColor(pScheme->GetColor(DEPRESSED_COLOR, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	SetSelectedColor(pScheme->GetColor(DEPRESSED_COLOR, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	SetDefaultBorder(pScheme->GetBorder(EMPTY_STRING));
	SetArmedBorder(pScheme->GetBorder(EMPTY_STRING));
	SetDepressedBorder(pScheme->GetBorder(EMPTY_STRING));
	SetSelectedBorder(pScheme->GetBorder(EMPTY_STRING));
}

void CTFCheckButton::PerformLayout()
{
	BaseClass::PerformLayout();

	if (!m_pParent)
		return;

	SetTextInset(5, 0);
	SetZPos(4);
	SetZPos(3);
	SetWide(m_pParent->GetWide());
	SetTall(m_pParent->GetTall());
	SetArmedSound("ui/buttonrollover.wav");
	SetDepressedSound("ui/buttonclick.wav");
	SetReleasedSound("ui/buttonclickrelease.wav");

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFCheckButton::OnCursorEntered()
{
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
	if (code == KEY_COUNT && iState != MOUSE_PRESSED)
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
	if (code == KEY_COUNT && (iState == MOUSE_ENTERED || iState == MOUSE_PRESSED) && m_pParent->IsEnabled() && m_pParent->IsEnabled())
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
	if (m_pParent->IsEnabled() && m_pParent->IsEnabled())
		m_pParent->SendAnimation(flag);
}