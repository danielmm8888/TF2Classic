#include "cbase.h"
#include "tf_mainmenu.h"
#include "tf_mainmenu_interface.h"

#include "panels/tf_mainmenupanel.h"
#include "panels/tf_pausemenupanel.h"
#include "panels/tf_backgroundpanel.h"
#include "panels/tf_loadoutpanel.h"
#include "panels/tf_notificationpanel.h"
#include "panels/tf_shadebackgroundpanel.h"
#include "panels/tf_optionsdialog.h"
#include "panels/tf_quitdialogpanel.h"
#include "panels/tf_statsummarydialog.h"
#include "engine/IEngineSound.h"
#include "tier0/icommandline.h"

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

	pNotifications.RemoveAll();

	m_pPanels.SetSize(COUNT_MENU);
	AddMenuPanel(new CTFMainMenuPanel(this, "CTFMainMenuPanel"), MAIN_MENU);
	AddMenuPanel(new CTFPauseMenuPanel(this, "CTFPauseMenuPanel"), PAUSE_MENU);
	AddMenuPanel(new CTFBackgroundPanel(this, "CTFBackgroundPanel"), BACKGROUND_MENU);
	AddMenuPanel(new CTFLoadoutPanel(this, "CTFLoadoutPanel"), LOADOUT_MENU);
	AddMenuPanel(new CTFNotificationPanel(this, "CTFNotificationPanel"), NOTIFICATION_MENU);
	AddMenuPanel(new CTFShadeBackgroundPanel(this, "CTFShadeBackgroundPanel"), SHADEBACKGROUND_MENU);
	AddMenuPanel(new CTFQuitDialogPanel(this, "CTFQuitDialogPanel"), QUIT_MENU);
	AddMenuPanel(new CTFOptionsDialog(this, "CTFOptionsDialog"), OPTIONSDIALOG_MENU);
	AddMenuPanel(new CTFStatsSummaryDialog(this, "CTFStatsSummaryDialog"), STATSUMMARY_MENU);

	ShowPanel(MAIN_MENU);
	ShowPanel(PAUSE_MENU);
	ShowPanel(BACKGROUND_MENU);
	HidePanel(SHADEBACKGROUND_MENU);
	HidePanel(LOADOUT_MENU);
	HidePanel(NOTIFICATION_MENU);
	HidePanel(QUIT_MENU);
	HidePanel(OPTIONSDIALOG_MENU);
	HidePanel(STATSUMMARY_MENU);
	
	bInGameLayout = false;
	m_iStopGameStartupSound = 2;
	m_iUpdateLayout = 1;
	vgui::ivgui()->AddTickSignal(GetVPanel());
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
	AutoLayout();
};

void CTFMainMenu::OnCommand(const char* command)
{
	engine->ExecuteClientCmd(command);
}

void InvalidatePanelsLayout(const CCommand &args)
{
	MAINMENU_ROOT->InvalidatePanelsLayout(true, true);
}
ConCommand tf2c_mainmenu_reload("tf2c_mainmenu_reload", InvalidatePanelsLayout);

void CTFMainMenu::InvalidatePanelsLayout(bool layoutNow, bool reloadScheme)
{	
	for (int i = FIRST_MENU; i < COUNT_MENU; i++)
	{
		if (GetMenuPanel(i))
		{
			bool bVisible = GetMenuPanel(i)->IsVisible();
			GetMenuPanel(i)->InvalidateLayout(layoutNow, reloadScheme);
			GetMenuPanel(i)->SetVisible(bVisible);
		}
	}	
}

void CTFMainMenu::LaunchInvalidatePanelsLayout()
{
	m_iUpdateLayout = 4;
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
	if (m_iStopGameStartupSound > 0)
	{
		m_iStopGameStartupSound--;
		if (!m_iStopGameStartupSound)
		{
			enginesound->NotifyBeginMoviePlayback();
		}
	}
	if (m_iUpdateLayout > 0)
	{
		m_iUpdateLayout--;
		if (!m_iUpdateLayout)
		{
			InvalidatePanelsLayout(true, true);
		}
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
		if (GetMenuPanel(i))
			GetMenuPanel(i)->DefaultLayout();
	}		
};

void CTFMainMenu::GameLayout()
{
	//set all panels to game layout
	for (int i = FIRST_MENU; i < COUNT_MENU; i++)
	{
		if (GetMenuPanel(i))
			GetMenuPanel(i)->GameLayout();
	}
};


void CTFMainMenu::SendNotification(MainMenuNotification pMessage)
{
	pNotifications.AddToTail(pMessage);
	dynamic_cast<CTFNotificationPanel*>(GetMenuPanel(NOTIFICATION_MENU))->OnNotificationUpdate();
	dynamic_cast<CTFMainMenuPanel*>(GetMenuPanel(MAIN_MENU))->OnNotificationUpdate();
	dynamic_cast<CTFPauseMenuPanel*>(GetMenuPanel(PAUSE_MENU))->OnNotificationUpdate();
}

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

void CTFMainMenu::SetStats(CUtlVector<ClassStats_t> &vecClassStats)
{
	if (!guiroot)
		return;
	dynamic_cast<CTFStatsSummaryDialog*>(GetMenuPanel(STATSUMMARY_MENU))->SetStats(vecClassStats);
}