#include "cbase.h"
#include "tf_quitdialogpanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFQuitDialogPanel::CTFQuitDialogPanel(vgui::Panel* parent, const char *panelName) : CTFDialogPanelBase(parent, panelName)
{
	SetMainMenu(GetParent());
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFQuitDialogPanel::~CTFQuitDialogPanel()
{

}

void CTFQuitDialogPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	LoadControlSettings("resource/UI/main_menu/QuitDialogPanel.res");
}

void CTFQuitDialogPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "quitconfirm"))
	{
		engine->ClientCmd("quit");
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}


void CTFQuitDialogPanel::Show()
{
	BaseClass::Show();
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->HidePanel(CURRENT_MENU);
};

void CTFQuitDialogPanel::Hide()
{
	BaseClass::Hide();
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(CURRENT_MENU);
};
