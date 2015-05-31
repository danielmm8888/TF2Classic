#include "cbase.h"
#include "tf_mainmenuquitpanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuQuitPanel::CTFMainMenuQuitPanel(vgui::Panel* parent, const char *panelName) : CTFMainMenuPanelBase(parent, panelName)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);
	SetMainMenu(GetParent());
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuQuitPanel::~CTFMainMenuQuitPanel()
{

}

void CTFMainMenuQuitPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	LoadControlSettings("resource/UI/main_menu/QuitMenu.res");
}

void CTFMainMenuQuitPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFMainMenuQuitPanel::OnCommand(const char* command)
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


void CTFMainMenuQuitPanel::Show()
{
	BaseClass::Show();
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(SHADEBACKGROUND_MENU);
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->HidePanel(CURRENT_MENU);
};

void CTFMainMenuQuitPanel::Hide()
{
	BaseClass::Hide();
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->HidePanel(SHADEBACKGROUND_MENU);
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(CURRENT_MENU);
};
