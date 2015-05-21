#include "cbase.h"
#include "tf_mainmenuscrollbar.h"
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

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFMainMenuScrollBar, CTFMainMenuScrollBar);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuScrollBar::CTFMainMenuScrollBar(vgui::Panel *parent, const char *panelName, const char *text) : CTFMainMenuButtonBase(parent, panelName, text)
{
	pButton = new CTFScrollButton(this, "ScrollBar", text);
	pButton->SetParent(this);
	fMinValue = 54;
	fMaxValue = 180;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuScrollBar::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuScrollBar::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	GetText(m_szText, sizeof(m_szText));
	pButton->SetText(m_szText);
	pButton->SetCommand(m_szCommand);
	pButton->SetVisible(true);
	pButton->SetEnabled(true);
	pButton->SetPos(0, 0);
	pButton->SetZPos(2);
	pButton->SetWide(20);  //scroll wide
	pButton->SetTall(GetTall());
	pButton->SetContentAlignment(GetAlignment(m_szTextAlignment));
	pButton->SetFont(GetFont());
	pButton->SetDefaultColor(pScheme->GetColor(pDefaultText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetArmedColor(pScheme->GetColor(pArmedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetDepressedColor(pScheme->GetColor(pDepressedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetSelectedColor(pScheme->GetColor(pDepressedText, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetArmedSound("ui/buttonrollover.wav");
	pButton->SetDepressedSound("ui/buttonclick.wav");
	pButton->SetReleasedSound("ui/buttonclickrelease.wav");
	if (m_bBorderVisible)
	{
		pButton->SetDefaultBorder(pScheme->GetBorder(pDefaultBorder));
		pButton->SetArmedBorder(pScheme->GetBorder(pArmedBorder));
		pButton->SetDepressedBorder(pScheme->GetBorder(pDepressedBorder));
		pButton->SetSelectedBorder(pScheme->GetBorder(pDepressedBorder));
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuScrollBar::OnTick()
{
	BaseClass::OnTick();

	if (pButton->GetState() == MOUSE_PRESSED)
	{
		SetPercentage();
		RunCommand();
	}
}

float CTFMainMenuScrollBar::GetPercentage()
{
	int mx = pImage->GetWide() - pButton->GetWide();  //max local xpos
	int scroll_x = 0, scroll_y = 0;
	pButton->GetPos(scroll_x, scroll_y); //scroll local pos
	float pers = (float)scroll_x / (float)mx;
	//Msg("Percentage: %f%%\n", pers * 100.0);
	return pers;
}

float CTFMainMenuScrollBar::GetValue()
{
	return GetPercentage() * (fMaxValue - fMinValue) + fMinValue;
}

void CTFMainMenuScrollBar::RunCommand()
{
	//Msg("SCROLLBAR VALUE: %f\n", GetValue());
	//Q_snprintf(szCommand, sizeof(szCommand), "%s %f", m_szCommand, GetValue
	char szCommand[MAX_PATH];
	Q_snprintf(szCommand, sizeof(szCommand), "scrolled");
	GetParent()->OnCommand(szCommand);
}


void CTFMainMenuScrollBar::SetPercentage()
{
	int x = 0, y = 0;
	int ix = 0, iy = 0;
	int mx = pImage->GetWide() - pButton->GetWide();  //max local xpos
	surface()->SurfaceGetCursorPos(x, y); //cursor global pos
	GetParent()->ScreenToLocal(x, y);//cursor global to local
	GetPos(ix, iy); //control global pos
	int lx = x - ix - pButton->GetWide() / 2; //cursor local pos
	lx = Min(Max(0, lx), mx); //check bounds
	AnimationController::PublicValue_t p_AnimHover = { lx, 0, 0, 0 };
	vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimHover, 0.0f, 0.05f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}

void CTFMainMenuScrollBar::SetPercentage(float fPerc)
{
	int mx = pImage->GetWide() - pButton->GetWide();  //max local xpos
	float fPers = 0.25f;
	int scroll_x = 0, scroll_y = 0;
	pButton->GetPos(scroll_x, scroll_y); //scroll local pos
	float fPos = fPers * mx;
	AnimationController::PublicValue_t p_AnimHover = { fPos, scroll_y, 0, 0 };
	vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimHover, 0.0f, 0.05f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuScrollBar::SetDefaultAnimation()
{
	BaseClass::SetDefaultAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuScrollBar::SendAnimation(MouseState flag)
{
	BaseClass::SendAnimation(flag);
}

///
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFScrollButton::CTFScrollButton(vgui::Panel *parent, const char *panelName, const char *text) : CTFButtonBase(parent, panelName, text)
{
	iState = MOUSE_DEFAULT;
	vgui::ivgui()->AddTickSignal(GetVPanel());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFScrollButton::OnCursorEntered()
{
	BaseClass::BaseClass::OnCursorEntered();
	if (iState != MOUSE_ENTERED && iState != MOUSE_PRESSED)
	{
		SetMouseEnteredState(MOUSE_ENTERED);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFScrollButton::OnCursorExited()
{
	BaseClass::BaseClass::OnCursorExited();
	/*if (iState != MOUSE_EXITED)
	{
		SetMouseEnteredState(MOUSE_EXITED);
	}*/
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFScrollButton::OnMousePressed(vgui::MouseCode code)
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
void CTFScrollButton::OnMouseReleased(vgui::MouseCode code)
{
	BaseClass::BaseClass::OnMouseReleased(code);
	if (code == MOUSE_LEFT && (iState == MOUSE_ENTERED || iState == MOUSE_PRESSED))
	{
		//m_pParent->GetParent()->OnCommand(m_pParent->m_szCommand);
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
void CTFScrollButton::SetMouseEnteredState(MouseState flag)
{
	BaseClass::SetMouseEnteredState(flag);
	m_pParent->SendAnimation(flag);
}