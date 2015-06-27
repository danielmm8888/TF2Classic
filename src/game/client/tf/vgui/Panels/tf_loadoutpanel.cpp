#include "cbase.h"
#include "tf_loadoutpanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFLoadoutPanel::CTFLoadoutPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	SetMainMenu(GetParent());
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFLoadoutPanel::~CTFLoadoutPanel()
{

}

void CTFLoadoutPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/LoadoutPanel.res");
	DefaultLayout();
}

void CTFLoadoutPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFLoadoutPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "back") || (!Q_strcmp(command, "vguicancel")))
	{
		Hide();
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}


void CTFLoadoutPanel::Show()
{
	BaseClass::Show();
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(SHADEBACKGROUND_MENU);
};

void CTFLoadoutPanel::Hide()
{
	BaseClass::Hide();
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->HidePanel(SHADEBACKGROUND_MENU);
};


void CTFLoadoutPanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFLoadoutPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFLoadoutPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();

};

void CTFLoadoutPanel::GameLayout()
{
	BaseClass::GameLayout();

};
