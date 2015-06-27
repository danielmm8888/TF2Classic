#include "cbase.h"
#include "tf_mainmenu.h"
#include "tf_mainmenu_interface.h"

#include "panels/tf_mainmenupanel.h"
#include "panels/tf_pausemenupanel.h"
#include "panels/tf_backgroundpanel.h"
#include "panels/tf_loadoutpanel.h"
#include "panels/tf_shadebackgroundpanel.h"
#include "panels/tf_optionsdialog.h"
#include "panels/tf_optionsadvancedpanel.h"
#include "panels/tf_optionsmousepanel.h"
#include "panels/tf_optionskeyboardpanel.h"
#include "panels/tf_optionsaudiopanel.h"
#include "panels/tf_optionsvideopanel.h"
#include "panels/tf_quitdialogpanel.h"

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
	SetProportional(true);
	SetVisible(true);

	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);

	m_pPanels.SetSize(COUNT_MENU);
	AddMenuPanel(new CTFMainMenuPanel(this, "CTFMainMenuPanel"), MAIN_MENU);
	AddMenuPanel(new CTFPauseMenuPanel(this, "CTFPauseMenuPanel"), PAUSE_MENU);
	AddMenuPanel(new CTFBackgroundPanel(this, "CTFBackgroundPanel"), BACKGROUND_MENU);
	AddMenuPanel(new CTFLoadoutPanel(this, "CTFLoadoutPanel"), LOADOUT_MENU);
	AddMenuPanel(new CTFShadeBackgroundPanel(this, "CTFShadeBackgroundPanel"), SHADEBACKGROUND_MENU);
	AddMenuPanel(new CTFQuitDialogPanel(this, "CTFQuitDialogPanel"), QUIT_MENU);
	AddMenuPanel(new CTFOptionsDialog(this, "CTFOptionsDialog"), OPTIONSDIALOG_MENU);
	AddMenuPanel(new CTFOptionsAdvancedPanel(this, "CTFOptionsAdvancedPanel"), OPTIONSADV_MENU);
	AddMenuPanel(new CTFOptionsMousePanel(this, "CTFOptionsMousePanel"), OPTIONSMOUSE_MENU);
	AddMenuPanel(new CTFOptionsKeyboardPanel(this, "CTFOptionsKeyboardPanel"), OPTIONSKEYBOARD_MENU);
	AddMenuPanel(new CTFOptionsAudioPanel(this, "CTFOptionsAudioPanel"), OPTIONSAUDIO_MENU);
	AddMenuPanel(new CTFOptionsVideoPanel(this, "CTFOptionsVideoPanel"), OPTIONSVIDEO_MENU);
	//AddMenuPanel(new CGameConsoleDialog(this, "CTFOptionsVideoPanel"), OPTIONSVIDEO_MENU);
	HidePanel(MAIN_MENU);
	ShowPanel(PAUSE_MENU);
	ShowPanel(BACKGROUND_MENU);
	HidePanel(LOADOUT_MENU);
	HidePanel(SHADEBACKGROUND_MENU);
	HidePanel(QUIT_MENU);
	HidePanel(OPTIONSDIALOG_MENU);
	HidePanel(OPTIONSADV_MENU);
	HidePanel(OPTIONSMOUSE_MENU);
	HidePanel(OPTIONSKEYBOARD_MENU);
	HidePanel(OPTIONSAUDIO_MENU);
	HidePanel(OPTIONSVIDEO_MENU);
	
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

void CTFMainMenu::AddMenuPanel(CTFMenuPanelBase *m_pPanel, int iPanel)
{
	m_pPanels[iPanel] = m_pPanel;
	m_pPanel->SetZPos(iPanel);
}

CTFMenuPanelBase* CTFMainMenu::GetMenuPanel(int iPanel)
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
	else if (engine->IsDrawingLoadingImage() && IsVisible())
	{
		SetVisible(false);
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