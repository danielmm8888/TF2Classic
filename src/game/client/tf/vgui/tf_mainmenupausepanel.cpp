#include "cbase.h"
#include "tf_mainmenupausepanel.h"
#include "tf_mainmenu.h"
#include "engine/IEngineSound.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuPausePanel::CTFMainMenuPausePanel(vgui::Panel* parent) : CTFMainMenuPanelBase(parent)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);
	SetVisible(true);
	SetMainMenu(GetParent());

	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);
	LoadControlSettings("resource/UI/main_menu/PauseMenu.res");
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	m_pDisconnectButton = dynamic_cast<CTFMainMenuButton *>(FindChildByName("DisconnectButton"));

	m_pRGBPanel = new CTFMainMenuRGBPanel(this);
	m_pRGBPanel->SetVisible(false);
	m_pRGBPanel->SetZPos(4);
	
	bInGame = true;
	DefaultLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuPausePanel::~CTFMainMenuPausePanel()
{

}

void CTFMainMenuPausePanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMainMenuPausePanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFMainMenuPausePanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "newquit"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(QUIT_MENU);
	}
	else if (!Q_strcmp(command, "newoptions"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONS_MENU);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}


void CTFMainMenuPausePanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFMainMenuPausePanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFMainMenuPausePanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
};

void CTFMainMenuPausePanel::GameLayout()
{
	BaseClass::GameLayout();

	if (m_pRGBPanel && TFGameRules())
	{
		if (TFGameRules()->IsDeathmatch())
		{
			m_pRGBPanel->SetVisible(true);
		}
	}
};