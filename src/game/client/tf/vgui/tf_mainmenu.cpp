#include "cbase.h"
#include "tf_mainmenu.h"
#include "tf_mainmenu_interface.h"

#include "tf_mainmenupanel.h"
#include "tf_mainmenupausepanel.h"
#include "tf_mainmenubackgroundpanel.h"
#include "tf_mainmenuloadoutpanel.h"
#include "tf_mainmenushadebackgroundpanel.h"
#include "tf_mainmenuoptionspanel.h"
#include "tf_mainmenuquitpanel.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// See interface.h/.cpp for specifics:  basically this ensures that we actually Sys_UnloadModule the dll and that we don't call Sys_LoadModule 
//  over and over again.
static CDllDemandLoader g_GameUIDLL("GameUI");

CTFMainMenu *guiroot = NULL;

void OverrideMainMenu()
{
	if (!MainMenu->GetPanel())
	{
		MainMenu->Create(NULL);
	}
	if (guiroot->GetGameUI())
	{
		guiroot->GetGameUI()->SetMainMenuOverride(guiroot->GetVPanel());
		return;
	}
}

class CLoadingDialog;
vgui::DHANDLE<CLoadingDialog> g_hLoadingDialog;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenu::CTFMainMenu(VPANEL parent) : vgui::EditablePanel(NULL, "MainMenu")
{
	SetParent(parent);

	guiroot = this;
	gameui = NULL;
	LoadGameUI();
	SetScheme("ClientScheme");

	SetDragEnabled(false);
	SetShowDragHelper(false);
	SetProportional(false);
	SetVisible(true);

	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);

	m_pPanels.SetSize(COUNT_MENU);
	AddMenuPanel(new CTFMainMenuPanel(this, "CTFMainMenuPanel"), MAIN_MENU);
	AddMenuPanel(new CTFMainMenuPausePanel(this, "CTFMainMenuPausePanel"), PAUSE_MENU);
	AddMenuPanel(new CTFMainMenuBackgroundPanel(this, "CTFMainMenuBackgroundPanel"), BACKGROUND_MENU);
	AddMenuPanel(new CTFMainMenuLoadoutPanel(this, "CTFMainMenuLoadoutPanel"), LOADOUT_MENU);
	AddMenuPanel(new CTFMainMenuShadeBackgroundPanel(this, "CTFMainMenuShadeBackgroundPanel"), SHADEBACKGROUND_MENU);
	AddMenuPanel(new CTFMainMenuQuitPanel(this, "CTFMainMenuQuitPanel"), QUIT_MENU);
	AddMenuPanel(new CTFMainMenuOptionsPanel(this, "CTFMainMenuOptionsPanel"), OPTIONS_MENU);
	ShowPanel(MAIN_MENU);
	ShowPanel(PAUSE_MENU);
	ShowPanel(BACKGROUND_MENU);
	HidePanel(LOADOUT_MENU);
	HidePanel(SHADEBACKGROUND_MENU);
	HidePanel(QUIT_MENU);
	HidePanel(OPTIONS_MENU);

	bInGameLayout = false;
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenu::~CTFMainMenu()
{
	m_pPanels.RemoveAll();
	gameui = NULL;
	g_GameUIDLL.Unload();
}

void CTFMainMenu::AddMenuPanel(CTFMainMenuPanelBase *m_pPanel, int iPanel)
{
	m_pPanels[iPanel] = m_pPanel;
	m_pPanel->SetZPos(iPanel);
}

CTFMainMenuPanelBase* CTFMainMenu::GetMenuPanel(int iPanel)
{
	return m_pPanels[iPanel];
}

void CTFMainMenu::ShowPanel(MenuPanel iPanel)
{
	GetMenuPanel(iPanel)->Show();
}

void CTFMainMenu::HidePanel(MenuPanel iPanel)
{
	GetMenuPanel(iPanel)->Hide();
}

IGameUI *CTFMainMenu::GetGameUI()
{
	if (!gameui)
	{
		if (!LoadGameUI())
			return NULL;
	}

	return gameui;
}

bool CTFMainMenu::LoadGameUI()
{
	if (!gameui)
	{
		CreateInterfaceFn gameUIFactory = g_GameUIDLL.GetFactory();
		if (gameUIFactory)
		{
			gameui = (IGameUI *)gameUIFactory(GAMEUI_INTERFACE_VERSION, NULL);
			if (!gameui)
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
	return true;
}


void CTFMainMenu::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMainMenu::PerformLayout()
{
	BaseClass::PerformLayout();
};

void CTFMainMenu::OnCommand(const char* command)
{
	engine->ExecuteClientCmd(command);
}

void CTFMainMenu::OnTick()
{
	BaseClass::OnTick();
	if (!engine->IsDrawingLoadingImage() && !IsVisible())
	{
		SetVisible(true);
	}
	if (!InGame() && bInGameLayout)
	{
		DefaultLayout();
		bInGameLayout = false;
	}
	else if (InGame() && !bInGameLayout)
	{
		GameLayout();
		bInGameLayout = true;
	}

};

void CTFMainMenu::OnThink()
{
	BaseClass::OnThink();
};


void CTFMainMenu::DefaultLayout()
{
	//set all panels to default layout
	for (int i = FIRST_MENU; i < COUNT_MENU; i++)
	{
		GetMenuPanel(i)->DefaultLayout();
	}		
};

void CTFMainMenu::GameLayout()
{
	//set all panels to game layout
	for (int i = FIRST_MENU; i < COUNT_MENU; i++)
	{
		GetMenuPanel(i)->GameLayout();
	}
};

void CTFMainMenu::PaintBackground()
{
	SetPaintBackgroundType(0);
	BaseClass::PaintBackground();
}

bool CTFMainMenu::InGame()
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if (pPlayer && IsVisible())
	{
		return true;
	}
	else 
	{
		return false;
	}
}