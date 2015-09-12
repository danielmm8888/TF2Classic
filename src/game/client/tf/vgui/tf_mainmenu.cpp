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
#include "panels/tf_tooltippanel.h"
#include "engine/IEngineSound.h"
#include "tier0/icommandline.h"
#include "tf_hud_notification_panel.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// See interface.h/.cpp for specifics:  basically this ensures that we actually Sys_UnloadModule the dll and that we don't call Sys_LoadModule 
//  over and over again.
static CDllDemandLoader g_GameUIDLL("GameUI");

CTFMainMenu *guiroot = NULL;

#define VERSION_URL			"http://services.0x13.io/tf2c/version/?latest=1"
#define MESSAGE_URL			"http://services.0x13.io/tf2c/motd/"

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

CON_COMMAND(tf2c_mainmenu_reload, "Reload Main Menu")
{
	MAINMENU_ROOT->InvalidatePanelsLayout(true, true);
}

CON_COMMAND(showloadout, "Show loadout screen (new)")
{
	if (!guiroot)
		return;

	engine->ClientCmd("gameui_activate");
	MAINMENU_ROOT->ShowPanel(LOADOUT_MENU, true);
}

CON_COMMAND_F(tf2c_checkmessages, "Check for the messages", FCVAR_DEVELOPMENTONLY)
{
	MAINMENU_ROOT->CheckMessage();
}

ConVar tf2c_checkfrequency("tf2c_checkfrequency", "900", FCVAR_DEVELOPMENTONLY, "Messages check frequency (seconds)");

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
	AddMenuPanel(new CTFToolTipPanel(this, "CTFToolTipPanel"), TOOLTIP_MENU);

	ShowPanel(MAIN_MENU);
	ShowPanel(PAUSE_MENU);
	ShowPanel(BACKGROUND_MENU);
	HidePanel(SHADEBACKGROUND_MENU);
	HidePanel(LOADOUT_MENU);
	HidePanel(NOTIFICATION_MENU);
	HidePanel(QUIT_MENU);
	HidePanel(OPTIONSDIALOG_MENU);
	HidePanel(STATSUMMARY_MENU);
	HidePanel(TOOLTIP_MENU);
	
	bInGameLayout = false;
	m_iStopGameStartupSound = 2;
	m_iUpdateLayout = 1;

	bOutdated = false;
	fLastCheck = tf2c_checkfrequency.GetFloat() * -1;
	bCompleted = false;
	m_SteamHTTP = steamapicontext->SteamHTTP();

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

void CTFMainMenu::ShowPanel(MenuPanel iPanel, bool bShowSingle /*= false*/)
{
	GetMenuPanel(iPanel)->SetShowSingle(bShowSingle);
	GetMenuPanel(iPanel)->Show();
	if (bShowSingle)
	{
		GetMenuPanel(CURRENT_MENU)->Hide();
	}
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
	AutoLayout();
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

	if (!bCompleted)
	{
		SteamAPI_RunCallbacks();
		//m_SteamHTTP->GetHTTPDownloadProgressPct(m_httpRequest, &fPercent);
	}

	if (gpGlobals->curtime - fLastCheck > tf2c_checkfrequency.GetFloat())
	{
		fLastCheck = gpGlobals->curtime;
		CheckMessage();
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

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	if (pLocalPlayer && !pVersionCheck)
	{
		CHudNotificationPanel *pNotifyPanel = GET_HUDELEMENT(CHudNotificationPanel);
		if (pNotifyPanel)
		{
			pNotifyPanel->SetupNotifyCustom(pMessage.sMessage, "ico_notify_flag_moving", C_TFPlayer::GetLocalTFPlayer()->GetTeamNumber());
		}
	}
}

void CTFMainMenu::RemoveNotification(int iIndex) 
{
	pNotifications.Remove(iIndex);
	dynamic_cast<CTFMainMenuPanel*>(GetMenuPanel(MAIN_MENU))->OnNotificationUpdate();
	dynamic_cast<CTFPauseMenuPanel*>(GetMenuPanel(PAUSE_MENU))->OnNotificationUpdate();
};

int CTFMainMenu::GetUnreadNotificationsCount() 
{ 
	int iCount = 0;
	for (int i = 0; i < pNotifications.Count(); i++)
	{
		if (pNotifications[i].bUnread)
			iCount++;
	}
	return iCount;
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

void CTFMainMenu::SetStats(CUtlVector<ClassStats_t> &vecClassStats)
{
	if (!guiroot)
		return;
	dynamic_cast<CTFStatsSummaryDialog*>(GetMenuPanel(STATSUMMARY_MENU))->SetStats(vecClassStats);
}


void CTFMainMenu::ShowToolTip(char* sText)
{
	dynamic_cast<CTFToolTipPanel*>(GetMenuPanel(TOOLTIP_MENU))->ShowToolTip(sText);
}

void CTFMainMenu::HideToolTip()
{
	dynamic_cast<CTFToolTipPanel*>(GetMenuPanel(TOOLTIP_MENU))->HideToolTip();
}

void CTFMainMenu::CheckMessage(bool Version/* = false*/)
{
	if (!m_SteamHTTP)
		return;

	char httpString[64];
	Q_snprintf(httpString, sizeof(httpString), (!Version ? MESSAGE_URL : VERSION_URL));
		
	m_httpRequest = m_SteamHTTP->CreateHTTPRequest(k_EHTTPMethodGET, httpString);
	m_SteamHTTP->SetHTTPRequestNetworkActivityTimeout(m_httpRequest, 5);

	SteamAPICall_t hSteamAPICall;
	m_SteamHTTP->SendHTTPRequest(m_httpRequest, &hSteamAPICall);
	m_CallResult.Set(hSteamAPICall, this, (&CTFMainMenu::OnHTTPRequestCompleted));

	pVersionCheck = Version;
	bCompleted = false;
}

void CTFMainMenu::OnHTTPRequestCompleted(HTTPRequestCompleted_t *m_CallResult, bool iofailure)
{
	DevMsg("HTTP Request completed: %i\n", m_CallResult->m_eStatusCode);
	bCompleted = true;

	if (m_CallResult->m_eStatusCode == 200)
	{
		bCompleted = true;
		uint32 iBodysize;
		m_SteamHTTP->GetHTTPResponseBodySize(m_httpRequest, &iBodysize);
		uint8 iBodybuffer[128];
		m_SteamHTTP->GetHTTPResponseBodyData(m_httpRequest, iBodybuffer, iBodysize);
		char result[128];
		Q_strncpy(result, (char*)iBodybuffer, iBodysize + 1);

		if (!pVersionCheck)
		{
			OnMessageCheckCompleted(result);
		}
		else
		{
			OnVersionCheckCompleted(result);
		}
	}
	else
	{
		bCompleted = false;
	}

	m_SteamHTTP->ReleaseHTTPRequest(m_httpRequest);
	
	if (!pVersionCheck)
	{
		CheckMessage(true);
	}
}


char* CTFMainMenu::GetVersionString()
{
	char verString[30];
	if (g_pFullFileSystem->FileExists("version.txt"))
	{
		FileHandle_t fh = filesystem->Open("version.txt", "r", "MOD");
		int file_len = filesystem->Size(fh);
		char* GameInfo = new char[file_len + 1];

		filesystem->Read((void*)GameInfo, file_len, fh);
		GameInfo[file_len] = 0; // null terminator

		filesystem->Close(fh);

		Q_snprintf(verString, sizeof(verString), GameInfo + 8);

		delete[] GameInfo;
	}

	char *szResult = (char*)malloc(sizeof(verString));
	Q_strncpy(szResult, verString, sizeof(verString));
	return szResult;
}



void CTFMainMenu::OnMessageCheckCompleted(const char* pMessage)
{
	pVersionCheck = false;

	if (pMessage[0] == '0')
		return;

	if (m_pzLastMessage[0] != '\0' && !Q_strcmp(pMessage, m_pzLastMessage))
		return;

	char pzResultString[128];
	char pzMessageString[128];

	char * pch;
	int id = 0;
	pch = strchr((char*)pMessage, '\n');
	if (pch != NULL)
	{
		id = pch - pMessage + 1;
	}
	Q_snprintf(pzResultString, id, "%s", pMessage);
	Q_snprintf(pzMessageString, sizeof(pzMessageString), pMessage + id);
	Q_snprintf(m_pzLastMessage, sizeof(m_pzLastMessage), pMessage);

	MainMenuNotification Notification(pzResultString, pzMessageString);
	SendNotification(Notification);
}


void CTFMainMenu::OnVersionCheckCompleted(const char* pMessage)
{
	if (Q_strcmp(GetVersionString(), pMessage) < 0)
	{
		char resultString[128];
		bOutdated = true;
		Q_snprintf(resultString, sizeof(resultString), "Your game is out of date.\nThe newest version of TF2C is %s.\nDownload the update at\nwww.tf2classic.com", pMessage);
		MainMenuNotification Notification("Update!", resultString);
		SendNotification(Notification);
	}
	else
	{
		bOutdated = false;
	}
}

float toProportionalWide(float iWide)
{
	int x, y, x0, y0;
	surface()->GetProportionalBase(x, y);
	surface()->GetScreenSize(x0, y0);
	return ((float)x0 / (float)x) * (float)iWide;
}

float toProportionalTall(float iTall)
{
	int x, y, x0, y0;
	surface()->GetProportionalBase(x, y);
	surface()->GetScreenSize(x0, y0);
	return ((float)y0 / (float)y) * (float)iTall;
}