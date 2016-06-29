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
#include "panels/tf_dialogpanelbase.h"
#include "inputsystem/iinputsystem.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFAdvButton, CTFAdvButtonBase);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvButton::CTFAdvButton(vgui::Panel *parent, const char *panelName, const char *text) : CTFAdvButtonBase(parent, panelName, text)
{
	pButton = new CTFButton(this, "SubButton", text);
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
	bGlowing = false;
	m_flActionThink = -1;
	m_flAnimationThink = -1;
	m_bAnimationIn = true;
}

void CTFAdvButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_bScaleImage = inResourceData->GetBool("scaleImage", true);

	for (KeyValues *pData = inResourceData->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey())
	{
		if (!Q_stricmp(pData->GetName(), "SubButton"))
		{
			pButton->ApplySettings(pData);
		}
	}

	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	pButtonImage->SetDrawColor(pScheme->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::PerformLayout()
{
	BaseClass::PerformLayout();

	if (m_fImageWidth)
	{
		float fWidth = YRES(m_fImageWidth);
		float fHeight = fWidth;
		int iShift = (GetTall() - fWidth) / 2.0;
		float fXOrigin = iShift * 2 + fWidth;
		pButton->SetTextInset(fXOrigin, 0);
		pButtonImage->SetPos(iShift, iShift);
		pButtonImage->SetWide(fWidth);
		pButtonImage->SetTall(fHeight);
	}

	pButtonImage->SetImage(pDefaultButtonImage);
	pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorDefault, Color(255, 255, 255, 255)));
	pButtonImage->SetZPos(2);
	pButtonImage->SetShouldScaleImage(m_bScaleImage);


	CTFDialogPanelBase *pParent = dynamic_cast<CTFDialogPanelBase*>(GetParent());
	if (pParent)
	{
		char sText[64];
		pButton->GetText(sText, sizeof(sText));
		if (Q_strcmp(sText, ""))
		{
			char * pch;
			pch = strchr(sText, '&');
			if (pch != NULL)
			{
				int id = pch - sText + 1;
				//pch = strchr(pch + 1, '&');
				char* cTest = &sText[id];
				cTest[1] = '\0';
				pParent->AddShortcut(cTest, GetCommandString());
			}
		}
	}
}

void CTFAdvButton::SetText(const char *tokenName)
{
	pButton->SetText(tokenName);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButton::OnThink()
{
	BaseClass::OnThink();

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
	bGlowing = Glowing; 

	if (!bGlowing)
	{
		float m_fAlpha = 255.0f;
		float m_fDelay = 0.0f;
		float m_fDuration = 0.0f;
		vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", m_fAlpha, m_fDelay, m_fDuration, vgui::AnimationController::INTERPOLATOR_LINEAR);
	}
};

void CTFAdvButton::SetSelected(bool bState)
{
	m_bSelected = bState;
	SendAnimation(MOUSE_DEFAULT);
	pButton->SetSelected(bState);
};

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

	if (pButton->IsSelected() && m_bSelected)
		return;

	bool bAnimation = ((pButton->m_fXShift == 0 && pButton->m_fYShift == 0) ? false : true);
	AnimationController::PublicValue_t p_AnimLeave(0, 0);
	AnimationController::PublicValue_t p_AnimHover(pButton->m_fXShift, pButton->m_fYShift);
	switch (flag)
	{
	//We can add additional stuff like animation here
	case MOUSE_DEFAULT:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorSelected, Color(255, 255, 255, 255)));
		break;
	case MOUSE_ENTERED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorArmed, Color(255, 255, 255, 255)));
		if (bAnimation)
			vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimHover, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR, NULL);
		break;
	case MOUSE_EXITED:
		pButtonImage->SetDrawColor(GETSCHEME()->GetColor(pImageColorSelected, Color(255, 255, 255, 255)));
		if (bAnimation)
			vgui::GetAnimationController()->RunAnimationCommand(pButton, "Position", p_AnimLeave, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR, NULL);
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
	m_pParent = NULL;
	iState = MOUSE_DEFAULT;
	m_fXShift = 0.0;
	m_fYShift = 0.0;
}

void CTFButton::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	m_fXShift = inResourceData->GetFloat("xshift", 0.0);
	m_fYShift = inResourceData->GetFloat("yshift", 0.0);

	SetArmedSound( "ui/buttonrollover.wav" );
	SetDepressedSound( "ui/buttonclick.wav" );
	SetReleasedSound( "ui/buttonclickrelease.wav" );

	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	
	SetDefaultColor(pScheme->GetColor(m_pParent->pDefaultColor, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	SetArmedColor(pScheme->GetColor(m_pParent->pArmedColor, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	SetDepressedColor(pScheme->GetColor(m_pParent->pDepressedColor, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));
	SetSelectedColor(pScheme->GetColor(m_pParent->pDepressedColor, Color(255, 255, 255, 255)), Color(0, 0, 0, 0));

	SetDefaultBorder(pScheme->GetBorder(EMPTY_STRING));
	SetArmedBorder(pScheme->GetBorder(EMPTY_STRING));
	SetDepressedBorder(pScheme->GetBorder(EMPTY_STRING));
	SetSelectedBorder(pScheme->GetBorder(EMPTY_STRING));
}

void CTFButton::PerformLayout()
{
	BaseClass::PerformLayout();

	if (!m_pParent)
		return;

	SetZPos(3);
	SetWide(m_pParent->GetWide());
	SetTall(m_pParent->GetTall());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButton::OnCursorEntered()
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
		m_pParent->PostActionSignal(new KeyValues("ButtonPressed"));
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
	if (m_pParent->IsEnabled())
		m_pParent->SendAnimation(flag);
}