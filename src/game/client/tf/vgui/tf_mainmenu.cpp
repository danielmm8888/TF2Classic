#include "cbase.h"
#include "tf_mainmenu.h"
#include "tf_mainmenu_interface.h"

#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// See interface.h/.cpp for specifics:  basically this ensures that we actually Sys_UnloadModule the dll and that we don't call Sys_LoadModule 
//  over and over again.
static CDllDemandLoader g_GameUIDLL("GameUI");

CMainMenuPanel *guiroot = NULL;

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
CMainMenuPanel::CMainMenuPanel(VPANEL parent) : Frame(NULL, "MainMenuPanel")
{
	SetParent(parent);
	guiroot = this;
	gameui = NULL;
	LoadGameUI();

	SetScheme("ClientScheme");
	SetProportional(false);
	SetVisible(true);
	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);
	LoadControlSettings("resource/UI/MainMenu.res");

	InGameLayout = false;
	b_ShowVideo = true;
	m_flActionThink = -1;
	m_pVersionLabel = dynamic_cast<CExLabel *>(FindChildByName("VersionLabel")); 
	m_pBackground = dynamic_cast<CTFImagePanel *>(FindChildByName("Background"));
	m_pDisconnectButton = dynamic_cast<CTFMainMenuButton *>(FindChildByName("DisconnectButton"));
	m_pVideo = dynamic_cast<CTFVideoPanel *>(FindChildByName("BackgroundVideo"));
	m_pLogo = dynamic_cast<CTFImagePanel *>(FindChildByName("Logo"));
	SetVersionLabel();

	vgui::ivgui()->AddTickSignal(GetVPanel());
	DefaultLayout();
}

IGameUI *CMainMenuPanel::GetGameUI()
{
	if (!gameui)
	{
		if (!LoadGameUI())
			return NULL;
	}

	return gameui;
}

bool CMainMenuPanel::LoadGameUI()
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


void CMainMenuPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CMainMenuPanel::~CMainMenuPanel()
{
	gameui = NULL;
	g_GameUIDLL.Unload();
}

void CMainMenuPanel::OnCommand(const char* command)
{
	engine->ExecuteClientCmd(command);
}

void CMainMenuPanel::OnTick()
{
	if (m_pVideo && !InGameLayout && m_flActionThink < gpGlobals->curtime)
		{
			m_pVideo->Activate();
			m_pVideo->BeginPlayback("media/bg_01.bik");
			m_pVideo->MoveToFront();
			m_flActionThink = gpGlobals->curtime + m_pVideo->GetEndDelay();
			b_ShowVideo = false;
		}
};

void CMainMenuPanel::OnThink()
{
	if (!InGame() && InGameLayout)
	{
		DefaultLayout();
		InGameLayout = false;
	} 
	else if (InGame() && !InGameLayout)
	{
		GameLayout();
		InGameLayout = true;
	}
	/*
	if (TFGameRules())
	{
		if (TFGameRules()->IsDeathmatch())
		{
			//Draw deathmatch additions :v
		}
	}
	*/
};

void CMainMenuPanel::DefaultLayout()
{
	//something with animations when I'll get them to work
	//vgui::GetAnimationController()->StartAnimationSequence("MainMenuIntro");

	//we need to find better way to show/hide stuff
	if (m_pDisconnectButton)
	{
		if (m_pDisconnectButton->OnlyInGame())
		{
			m_pDisconnectButton->SetVisible(false);
		}
		else if (m_pDisconnectButton->OnlyAtMenu())
		{
			m_pDisconnectButton->SetVisible(true);
		}
	}
	if (m_pVideo)
	{
		m_pVideo->SetVisible(true);
	}	
};

void CMainMenuPanel::GameLayout()
{
	if (m_pDisconnectButton)
	{
		if (m_pDisconnectButton->OnlyInGame())
		{
			m_pDisconnectButton->SetVisible(true);
		}
		else if (m_pDisconnectButton->OnlyAtMenu())
		{
			m_pDisconnectButton->SetVisible(false);
		}
	}
	if (m_pVideo)
	{
		m_pVideo->SetVisible(false);
	}
};

void CMainMenuPanel::SetVersionLabel()
{
	if (m_pVersionLabel)
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

			Q_snprintf(verString, sizeof(verString), "Version: %s", GameInfo + 8);

			delete[] GameInfo;
		}
		m_pVersionLabel->SetText(verString);
	}
};

void CMainMenuPanel::PaintBackground()
{
	SetPaintBackgroundType(0);
	BaseClass::PaintBackground();
}

bool CMainMenuPanel::InGame()
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if (pPlayer && IsVisible())
	{
		return true;
	}
	else {
		return false;
	}
}