#include "cbase.h"
#include "tf_dialogpanelbase.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advpanellistpanel.h"
#include "controls/tf_scriptobject.h"
#include "controls/tf_cvartogglecheckbutton.h"
#include "controls/tf_cvarslider.h"
#include "vgui_controls/ComboBox.h"
//#include "EngineInterface.h"

#include <KeyValues.h>
#include <vgui/IScheme.h>
#include "tier1/convar.h"
#include <stdio.h>
#include <vgui_controls/TextEntry.h>
// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

CTFDialogPanelBase::CTFDialogPanelBase(vgui::Panel *parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFDialogPanelBase::~CTFDialogPanelBase()
{
	DestroyControls();
}

bool CTFDialogPanelBase::Init()
{
	BaseClass::Init();
	bEmbedded = false;
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: sets background color & border
//-----------------------------------------------------------------------------
void CTFDialogPanelBase::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	//m_pOk = dynamic_cast<CTFAdvButton *>(FindChildByName("OK"));
	//m_pCancel = dynamic_cast<CTFAdvButton *>(FindChildByName("Cancel"));
}

void CTFDialogPanelBase::PerformLayout()
{
	BaseClass::PerformLayout();
	if (bEmbedded)
	{
		OnCreateControls();
		//m_pOk->SetVisible(false);		
		//m_pCancel->SetVisible(false);		
	}
	else
	{
		Show();
	}
};

void CTFDialogPanelBase::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		PostActionSignal(new KeyValues("CancelPressed"));
		OnResetData();
		Hide();
	}
	else if (!stricmp(command, "Ok"))
	{
		PostActionSignal(new KeyValues("OkPressed"));
		OnApplyChanges();
		Hide();
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFDialogPanelBase::Show()
{
	BaseClass::Show();
	if (!bEmbedded)
	{
		OnCreateControls();
	}
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.3f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	if (!GetMainMenu())
		return;
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(SHADEBACKGROUND_MENU);
};

void CTFDialogPanelBase::Hide()
{
	BaseClass::Hide();
	if (!bEmbedded)
	{
		DestroyControls();
	}
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	if (!GetMainMenu())
		return;
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->HidePanel(SHADEBACKGROUND_MENU);
};

void CTFDialogPanelBase::AddControl(vgui::Panel* panel, int iType, const char* text)
{
	mpcontrol_t	*pCtrl = new mpcontrol_t(m_pListPanel, "mpcontrol_t");
	switch (iType)
	{
	case O_CATEGORY:
		dynamic_cast<CTFAdvButton*>(panel)->SetDisabled(true);
		dynamic_cast<CTFAdvButton*>(panel)->SetBorder("AdvSettingsTitleBorder");
		dynamic_cast<CTFAdvButton*>(panel)->SetBorderVisible(true);
		dynamic_cast<CTFAdvButton*>(panel)->SetBGVisible(false);
		break;
	case O_BOOL:
		dynamic_cast<CTFAdvCheckButton*>(panel)->SetFont(m_pListPanel->GetFontString());
		break;
	case O_SLIDER:
		dynamic_cast<CTFAdvSlider*>(panel)->SetFont(m_pListPanel->GetFontString());
		break;
	case O_LIST:
		dynamic_cast<ComboBox*>(panel)->SetFont(m_pListPanel->GetFont());
		pCtrl->pPrompt = new vgui::Label(pCtrl, "DescLabel", "");
		pCtrl->pPrompt->SetFont(m_pListPanel->GetFont());
		pCtrl->pPrompt->SetContentAlignment(vgui::Label::a_west);
		pCtrl->pPrompt->SetTextInset(5, 0);
		pCtrl->pPrompt->SetText(text);
		break;
	default:
		break;
	}
	panel->SetParent(pCtrl);
	pCtrl->pControl = panel;
	int h = m_pListPanel->GetTall() / 13.0; //(float)GetParent()->GetTall() / 15.0;
	pCtrl->SetSize(800, h);
	m_pListPanel->AddItem(pCtrl);
}

void CTFDialogPanelBase::CreateControls()
{
	DestroyControls();
}

void CTFDialogPanelBase::DestroyControls()
{
	if (!m_pListPanel)
		return;

	m_pListPanel->DeleteAllItems();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDialogPanelBase::OnResetData()
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDialogPanelBase::OnApplyChanges()
{

}

void CTFDialogPanelBase::OnThink()
{

}
