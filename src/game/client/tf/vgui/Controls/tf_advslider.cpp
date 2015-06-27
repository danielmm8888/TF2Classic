#include "cbase.h"
#include "tf_advslider.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFAdvSlider, CTFAdvButtonBase);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvSlider::CTFAdvSlider(vgui::Panel *parent, const char *panelName, const char *text) : CTFAdvButtonBase(parent, panelName, text)
{
	pButton = new CTFScrollButton(this, "ScrollBar", text);
	pTitleLabel = new CExLabel(this, "TitleLabel", text);
	pValueLabel = new CExLabel(this, "ValueLabel", "0");
	pButton->SetParent(this);
	pValueLabel->SetParent(this);
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvSlider::~CTFAdvSlider()
{
	delete pButton;
	delete pTitleLabel;
	delete pValueLabel;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::Init()
{
	BaseClass::Init();
	Q_strncpy(pDefaultBG, DEFAULT_BG, sizeof(pDefaultBG));
	Q_strncpy(pArmedBG, ARMED_BG, sizeof(pArmedBG));
	Q_strncpy(pDepressedBG, DEPRESSED_BG, sizeof(pDepressedBG));
	fMinValue = 0.0;
	fMaxValue = 100.0;
	fLabelWidth = 0.0;
	fValue = 0.0;
	m_bBGVisible = true;
	m_bBorderVisible = true;
	m_bShowInt = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	fMinValue = inResourceData->GetFloat("minvalue", 0.0);
	fMaxValue = inResourceData->GetFloat("maxvalue", 100.0);
	fLabelWidth = inResourceData->GetFloat("labelWidth", 0.0);
	fValue = fMinValue;
	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	pButton->SetDefaultColor(pScheme->GetColor(DEFAULT_TEXT, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetArmedColor(pScheme->GetColor(ARMED_TEXT, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetDepressedColor(pScheme->GetColor(DEPRESSED_TEXT, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetSelectedColor(pScheme->GetColor(DEPRESSED_TEXT, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	pButton->SetFont(pScheme->GetFont(m_szFont, true));
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
	pTitleLabel->SetFgColor(pScheme->GetColor(pDefaultText, Color(255, 255, 255, 255)));
	pTitleLabel->SetFont(pScheme->GetFont(m_szFont, true));
	pValueLabel->SetFgColor(pScheme->GetColor(pDefaultText, Color(255, 255, 255, 255)));
	pValueLabel->SetFont(pScheme->GetFont(m_szFont, true));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::PerformLayout()
{
	BaseClass::PerformLayout();

	float fBorder = (fLabelWidth > 0.0 ? fLabelWidth : GetWide() / 2.0 + 20);
	float fShift = 50.0;
	GetText(m_szText, sizeof(m_szText));
	pButton->SetText(EMPTY_STRING);
	pButton->SetCommand(GetCommandString());
	pButton->SetVisible(IsEnabled());
	pButton->SetEnabled(true);
	pButton->SetPos(fBorder, 0);
	pButton->SetZPos(2);
	pButton->SetWide(20);  //scroll wide
	pButton->SetTall(GetTall());
	pButton->SetContentAlignment(GetAlignment(m_szTextAlignment));
	//pButton->SetFont(GetFont());
	pButton->SetArmedSound("ui/buttonrollover.wav");
	pButton->SetDepressedSound("ui/buttonclick.wav");
	pButton->SetReleasedSound("ui/buttonclickrelease.wav");


	pBGBorder->SetPos(fBorder, 0);
	pBGBorder->SetWide(GetWide() - fBorder - fShift);

	pTitleLabel->SetVisible(true);
	pTitleLabel->SetEnabled(true);
	pTitleLabel->SetText(m_szText);
	pTitleLabel->SetPos(0, 0);
	pTitleLabel->SetTextInset(5, 0);
	pTitleLabel->SetZPos(3);
	pTitleLabel->SetWide(fBorder);
	pTitleLabel->SetTall(GetTall());
	//pTitleLabel->SetFont(GetFont());
	pTitleLabel->SetContentAlignment(GetAlignment(m_szTextAlignment));
	
	pValueLabel->SetVisible(true);
	pValueLabel->SetEnabled(true);
	pValueLabel->SetPos(GetWide() - fShift, 0);
	pValueLabel->SetZPos(3);
	pValueLabel->SetWide(fShift);
	pValueLabel->SetTall(GetTall());
	//pValueLabel->SetFont(GetFont());
	pValueLabel->SetContentAlignment(GetAlignment("center"));

	Msg("LAYOUT slide %s\n", m_szText);
	UpdateValue();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::OnThink()
{
	BaseClass::OnThink();
	if (pButton->GetState() == MOUSE_PRESSED && IsEnabled())
	{
		//Msg("Setting shit\n");
		SetPercentage();
		RunCommand();
	}
	else if (Q_strcmp(GetCommandString(), "") && m_bAutoChange)
	{
		UpdateValue();
	}
}

void CTFAdvSlider::UpdateValue()
{
	//Msg("Updating\n");
	ConVarRef CheckButtonCommand(GetCommandString());
	float fValue = CheckButtonCommand.GetFloat();
	SetValue(fValue);
}


void CTFAdvSlider::RunCommand()
{
	//Msg("SCROLLBAR VALUE: %f\n", GetValue());
	//Q_snprintf(szCommand, sizeof(szCommand), "%s %f", m_szCommand, GetValue
	if (!Q_strcmp(GetCommandString(), ""))
	{
		char szCommand[MAX_PATH];
		Q_snprintf(szCommand, sizeof(szCommand), "scrolled");
		GetParent()->OnCommand(szCommand);
	}
	else if (m_bAutoChange)
	{
		char szCommand[MAX_PATH];
		Q_snprintf(szCommand, sizeof(szCommand), "%s %f", GetCommandString(), GetValue());
		engine->ExecuteClientCmd(szCommand);
		//GetParent()->OnCommand(szCommand);
	}
}


float CTFAdvSlider::GetValue()
{
	return fValue;
}

int CTFAdvSlider::GetScrollValue()
{
	return GetPercentage() * (fMaxValue - fMinValue) + fMinValue;
}

float CTFAdvSlider::GetPercentage()
{
	int _x = 0, _y = 0;
	int scroll_x = 0, scroll_y = 0;
	int mx = pBGBorder->GetWide() - pButton->GetWide();  //max local xpos
	pBGBorder->GetPos(_x, _y);
	pButton->GetPos(scroll_x, scroll_y); //scroll local pos

	//Msg("Percentage: %f%%\n", pers * 100.0);
	float pers = (float)(scroll_x - _x) / (float)mx;
	return pers;
}

void CTFAdvSlider::SetValue(float fVal)
{
	float fPerc = (fVal - fMinValue) / (fMaxValue - fMinValue);
	SetPercentage(fPerc);
}

void CTFAdvSlider::SetPercentage()
{
	int _x = 0, _y = 0;
	int x = 0, y = 0;
	int ix = 0, iy = 0;
	int mx = pBGBorder->GetWide() - pButton->GetWide();  //max local xpos
	pBGBorder->GetPos(_x, _y);
	surface()->SurfaceGetCursorPos(x, y); //cursor global pos
	GetParent()->ScreenToLocal(x, y);//cursor global to local
	GetPos(ix, iy); //control global pos

	float fPerc = (float)(x - ix - _x - pButton->GetWide() / 2) / (float)mx;
	SetPercentage(fPerc);
}

void CTFAdvSlider::SetPercentage(float fPerc)
{
	int _x = 0, _y = 0;
	int scroll_x = 0, scroll_y = 0;
	int mx = pBGBorder->GetWide() - pButton->GetWide();  //max local xpos
	pBGBorder->GetPos(_x, _y);
	pButton->GetPos(scroll_x, scroll_y); //scroll local pos
	float fPos = min(max(0.0, fPerc), 1.0) * (float)mx + (float)_x;

	fValue = min(max(0.0, fPerc), 1.0) * (fMaxValue - fMinValue) + fMinValue;
	char sValue[30];
	if (m_bShowInt)
	{
		Q_snprintf(sValue, sizeof(sValue), "%i", (int)fValue);
	}
	else
	{
		Q_snprintf(sValue, sizeof(sValue), "%2.1f", fValue);
	}
	pValueLabel->SetText(sValue);
	AnimationController::PublicValue_t p_AnimHover = { fPos, scroll_y, 0, 0 };
	vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimHover, 0.0f, 0.05f, vgui::AnimationController::INTERPOLATOR_LINEAR);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::SetDefaultAnimation()
{
	BaseClass::SetDefaultAnimation();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvSlider::SendAnimation(MouseState flag)
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
	if (!m_pParent->IsDisabled())
		m_pParent->SendAnimation(flag);
}