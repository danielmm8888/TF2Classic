#include "cbase.h"
#include "tf_mainmenuloadoutpanel.h"
#include "tf_mainmenu.h"
#include "engine/IEngineSound.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuLoadoutPanel::CTFMainMenuLoadoutPanel(vgui::Panel* parent, const char *panelName) : CTFMainMenuPanelBase(parent, panelName)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(true);
	SetVisible(true);
	SetMainMenu(GetParent());
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuLoadoutPanel::~CTFMainMenuLoadoutPanel()
{

}

void CTFMainMenuLoadoutPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/fullloadoutpanel.res");
	DefaultLayout();
}

void CTFMainMenuLoadoutPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFMainMenuLoadoutPanel::OnCommand(const char* command)
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


void CTFMainMenuLoadoutPanel::Show()
{
	BaseClass::Show();
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(SHADEBACKGROUND_MENU);
};

void CTFMainMenuLoadoutPanel::Hide()
{
	BaseClass::Hide();
	dynamic_cast<CTFMainMenu*>(GetMainMenu())->HidePanel(SHADEBACKGROUND_MENU);
};


void CTFMainMenuLoadoutPanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFMainMenuLoadoutPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFMainMenuLoadoutPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();

};

void CTFMainMenuLoadoutPanel::GameLayout()
{
	BaseClass::GameLayout();

};
