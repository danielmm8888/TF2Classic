#include "cbase.h"
#include "tf_mainmenuoptionspanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuOptionsPanel::CTFMainMenuOptionsPanel(vgui::Panel* parent) : CTFMainMenuPanelBase(parent)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);	
	SetMainMenu(GetParent());

	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);
	LoadControlSettings("resource/UI/main_menu/OptionsMenu.res");
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuOptionsPanel::~CTFMainMenuOptionsPanel()
{

}

void CTFMainMenuOptionsPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		Hide();
	}
	else if (!Q_strcmp(command, "quitconfirm"))
	{
		engine->ClientCmd("quit");
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFMainMenuOptionsPanel::Show()
{
	BaseClass::Show();
	ImagePanel *m_bShadedBG = dynamic_cast<ImagePanel *>(FindChildByName("ShadedBG"));
	vgui::GetAnimationController()->RunAnimationCommand(m_bShadedBG, "Alpha", 220, 0.0f, 0.3f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	//dynamic_cast<CTFMainMenu*>(GetMainMenu())->HidePanel(CURRENT_MENU);
};

void CTFMainMenuOptionsPanel::Hide()
{
	BaseClass::Hide();
	ImagePanel *m_bShadedBG = dynamic_cast<ImagePanel *>(FindChildByName("ShadedBG"));
	vgui::GetAnimationController()->RunAnimationCommand(m_bShadedBG, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	//dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(CURRENT_MENU);
};

void CTFMainMenuOptionsPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMainMenuOptionsPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};
