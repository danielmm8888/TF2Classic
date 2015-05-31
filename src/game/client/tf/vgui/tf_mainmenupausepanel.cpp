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
CTFMainMenuPausePanel::CTFMainMenuPausePanel(vgui::Panel* parent, const char *panelName) : CTFMainMenuPanelBase(parent, panelName)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);
	SetVisible(true);
	SetMainMenu(GetParent());	
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuPausePanel::~CTFMainMenuPausePanel()
{

}

bool CTFMainMenuPausePanel::Init()
{
	BaseClass::Init();

	bInGame = true;
	return true;
};


void CTFMainMenuPausePanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_pRGBPanel = new CTFMainMenuRGBPanel(this, "CTFMainMenuRGBPanel");
	LoadControlSettings("resource/UI/main_menu/PauseMenu.res");
	//m_pRGBPanel->SetVisible(false);
	//m_pRGBPanel->SetZPos(4);
}

void CTFMainMenuPausePanel::PerformLayout()
{
	BaseClass::PerformLayout();
	DefaultLayout();
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
		else
		{
			m_pRGBPanel->SetVisible(false);
		}
	}

};