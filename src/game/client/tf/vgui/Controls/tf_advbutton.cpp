#include "cbase.h"
#include "tf_advbutton.h"
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

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFAdvButton, CTFAdvButtonBase);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvButton::CTFAdvButton(vgui::Panel *parent, const char *panelName, const char *text) : CTFAdvButtonBase(parent, panelName, text)
{
	pButton = new CTFButton(this, "ButtonNew", text);
	pButton->SetParent(this);
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvButton::~CTFAdvButton()
{
	delete pButton;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::Init()
{
	BaseClass::Init();
	m_bBGVisible = true;
	m_bBorderVisible = false;
	bGlowing = false;
	m_fXShift = 0.0;
	m_fYShift = 0.0;
	m_flActionThink = -1;
	m_flAnimationThink = -1;
	m_bAnimationIn = true;
}

void CTFAdvButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_fXShift = inResourceData->GetFloat("xshift", 0.0);
	m_fYShift = inResourceData->GetFloat("yshift", 0.0);


	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	pButton->SetDefaultColor(pScheme->GetColor(pDefaultColor, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetArmedColor(pScheme->GetColor(pArmedColor, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetDepressedColor(pScheme->GetColor(pDepressedColor, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetSelectedColor(pScheme->GetColor(pArmedColor, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));

	pButtonImage->SetDrawColor(pScheme->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
	if (m_bBorderVisible)
	{
		pButton->SetDefaultBorder(pScheme->GetBorder(pDefaultBorder));
		pButton->SetArmedBorder(pScheme->GetBorder(pArmedBorder));
		pButton->SetDepressedBorder(pScheme->GetBorder(pDepressedBorder));
		pButton->SetSelectedBorder(pScheme->GetBorder(pArmedBorder));
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
void CTFAdvButton::PerformLayout()
{
	BaseClass::PerformLayout();

	GetText(m_szText, sizeof(m_szText));
	pButton->SetText(m_szText);
	pButton->SetCommand(GetCommandString());
	pButton->SetFont(GETSCHEME()->GetFont(m_szFont, true));
	pButton->SetVisible(IsVisible());
	pButton->SetEnabled(IsEnabled());
	pButton->SetSelected(IsSelected());
	pButton->SetZPos(3);
	pButton->SetWide(GetWide());
	pButton->SetTall(GetTall());
	pButton->SetContentAlignment(GetAlignment(m_szTextAlignment));
	pButton->SetArmedSound("ui/buttonrollover.wav");
	pButton->SetDepressedSound("ui/buttonclick.wav");
	pButton->SetReleasedSound("ui/buttonclickrelease.wav");

	float h = GetProportionalTallScale();
	float fWidth = (m_fWidth == 0.0 ? GetTall() : m_fWidth * h);
	int iShift = (GetTall() - fWidth) / 2.0;
	
	float fXOrigin = (m_fWidth == 0.0 ? 0 : iShift * 2 + fWidth);
	pButton->SetTextInset(fXOrigin, 0);

	pButtonImage->SetImage(pDefaultButtonImage);
	pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
	pButtonImage->SetVisible(IsVisible());
	pButtonImage->SetEnabled(IsEnabled());
	pButtonImage->SetPos(iShift, iShift);
	pButtonImage->SetZPos(2);
	pButtonImage->SetWide(fWidth);
	pButtonImage->SetTall(fWidth);	
	pButtonImage->SetShouldScaleImage(true);
}

void CTFAdvButton::SetText(const char *tokenName)
{
	pButton->SetText(tokenName);
	BaseClass::SetText(tokenName);
}

void CTFAdvButton::SetCommand(const char *command)
{
	pButton->SetCommand(command);
	BaseClass::SetCommand(command);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::OnTick()
{
	BaseClass::OnTick();

	if (bGlowing && m_flAnimationThink < gpGlobals->curtime)
	{
		float m_fAlpha = (m_bAnimationIn ? 50.0f : 255.0f);
		float m_fDelay = (m_bAnimationIn ? 0.75f : 0.0f);
		float m_fDuration = (m_bAnimationIn ? 0.15f : 0.25f);
		vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", m_fAlpha, m_fDelay, m_fDuration, vgui::AnimationController::INTERPOLATOR_LINEAR);
		m_bAnimationIn = !m_bAnimationIn;
		m_flAnimationThink = gpGlobals->curtime + 1.0f;
	}
}


void CTFAdvButton::SetGlowing(bool Glowing)
{
	bGlowing = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::SetDefaultAnimation()
{
	BaseClass::SetDefaultAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::SendAnimation(MouseState flag)
{
	BaseClass::SendAnimation(flag);

	AnimationController::PublicValue_t p_AnimLeave = { 0, 0, 0, 0 };
	AnimationController::PublicValue_t p_AnimHover = { m_fXShift, m_fYShift, 0, 0 };
	switch (flag)
	{
	//We can add additional stuff like animation here
	case MOUSE_DEFAULT:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
		break;
	case MOUSE_ENTERED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorArmed, Color(255, 255, 255, 255)));
		vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimHover, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		break;
	case MOUSE_EXITED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
		vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimLeave, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		break;
	case MOUSE_PRESSED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDepressed, Color(255, 255, 255, 255)));
		break;
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFButton::CTFButton(vgui::Panel *parent, const char *panelName, const char *text) : CTFButtonBase(parent, panelName, text)
{
	iState = MOUSE_DEFAULT;	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnCursorEntered()
{
	//BaseClass::OnCursorEntered();
	BaseClass::BaseClass::OnCursorEntered();
	if (iState != MOUSE_ENTERED && iState != MOUSE_PRESSED)
	{
		SetMouseEnteredState(MOUSE_ENTERED);
	}
	
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnCursorExited()
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
void CTFButton::OnMousePressed(vgui::MouseCode code)
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
void CTFButton::OnMouseReleased(vgui::MouseCode code)
{
	BaseClass::BaseClass::OnMouseReleased(code);
	
	if (code == MOUSE_LEFT && (iState == MOUSE_ENTERED || iState == MOUSE_PRESSED))
	{
		m_pParent->GetParent()->OnCommand(m_pParent->GetCommandString());
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
void CTFButton::SetMouseEnteredState(MouseState flag)
{
	BaseClass::SetMouseEnteredState(flag);
	if (!m_pParent->IsDisabled())
		m_pParent->SendAnimation(flag);
}