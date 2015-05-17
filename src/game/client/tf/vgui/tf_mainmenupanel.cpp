#include "cbase.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include "engine/IEngineSound.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuPanel::CTFMainMenuPanel(vgui::Panel* parent) : CTFMainMenuPanelBase(parent)
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
	LoadControlSettings("resource/UI/main_menu/MainMenu.res");
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	InGameLayout = false;
	m_bMusicPlay = true;
	m_flActionThink = -1;
	m_flAnimationThink = -1;
	m_flMusicThink = -1;
	m_bAnimationIn = true;
	m_pVersionLabel = dynamic_cast<CExLabel *>(FindChildByName("VersionLabel"));
	m_pBackground = dynamic_cast<CTFImagePanel *>(FindChildByName("Background"));
	m_pDisconnectButton = dynamic_cast<CTFMainMenuButton *>(FindChildByName("DisconnectButton"));
	m_pVideo = dynamic_cast<CTFVideoPanel *>(FindChildByName("BackgroundVideo"));
	m_pLogo = dynamic_cast<CTFImagePanel *>(FindChildByName("Logo"));
	m_pCardsPanel = new CTFMainMenuCardsPanel(this);
	m_pCardsPanel->SetVisible(false);
	m_pCardsPanel->SetZPos(4);

	Q_strncpy(m_pzVideoLink, GetRandomVideo(), sizeof(m_pzVideoLink));
	Q_strncpy(m_pzMusicLink, GetRandomMusic(), sizeof(m_pzMusicLink));
	SetVersionLabel();	
	
	DefaultLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuPanel::~CTFMainMenuPanel()
{

}

void CTFMainMenuPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMainMenuPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFMainMenuPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "newquit"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(QUIT_MENU);
	}
	else if (!Q_strcmp(command, "randommusic"))
	{
		m_bMusicPlay = false;
		Q_strncpy(m_pzMusicLink, GetRandomMusic(), sizeof(m_pzMusicLink));
		m_flMusicThink = gpGlobals->curtime + enginesound->GetSoundDuration(m_pzMusicLink);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFMainMenuPanel::VideoReplay()
{
	if (m_pzVideoLink[0] != '\0')
	{
		m_pVideo->Activate();
		m_pVideo->BeginPlaybackNoAudio(m_pzVideoLink);
		m_pVideo->MoveToFront();
	}
}

void CTFMainMenuPanel::OnTick()
{
	BaseClass::OnTick();
	if (!m_bMusicPlay && m_pzMusicLink[0] != '\0')
	{
		m_bMusicPlay = true;
		enginesound->NotifyBeginMoviePlayback();
		surface()->PlaySound(m_pzMusicLink);
	}
	if (m_flMusicThink < gpGlobals->curtime && m_pzMusicLink[0] != '\0')
	{
		m_bMusicPlay = false;
		Q_strncpy(m_pzMusicLink, GetRandomMusic(), sizeof(m_pzMusicLink));
		m_flMusicThink = gpGlobals->curtime + enginesound->GetSoundDuration(m_pzMusicLink);
		//Msg("LENGTH %f\n", enginesound->GetSoundDuration(m_pzMusicLink));
	}
	if (m_pVersionLabel && m_flAnimationThink < gpGlobals->curtime)
	{
		//AnimationController::PublicValue_t newPos = { 20, 30, 0, 0 };
		float m_fAlpha = (m_bAnimationIn ? 50.0 : 100.0);
		vgui::GetAnimationController()->RunAnimationCommand(m_pVersionLabel, "Alpha", m_fAlpha, 0.0f, 0.25f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		m_bAnimationIn = !m_bAnimationIn;
		m_flAnimationThink = gpGlobals->curtime + 0.25f;
	}
};

void CTFMainMenuPanel::PlayMusic()
{
	///
}

void CTFMainMenuPanel::OnThink()
{
	BaseClass::OnThink();

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

void CTFMainMenuPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();

	VideoReplay();

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

void CTFMainMenuPanel::GameLayout()
{
	BaseClass::GameLayout();

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

void CTFMainMenuPanel::SetVersionLabel()
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

char* CTFMainMenuPanel::GetRandomVideo()
{
	char* pszBasePath = "media/bg_0";
	int iCount = 0;

	for (int i = 0; i < 9; i++)
	{
		char szPath[MAX_PATH];
		char szNumber[5];
		Q_snprintf(szNumber, sizeof(szNumber), "%d", iCount + 1);
		Q_strncpy(szPath, pszBasePath, sizeof(szPath));
		Q_strncat(szPath, szNumber, sizeof(szPath));
		Q_strncat(szPath, ".bik", sizeof(szPath));
		if (!g_pFullFileSystem->FileExists(szPath))
		{
			if (iCount)
				break; 
			else
				return "";
		}
		iCount++;
	}

	int iRand = rand() % iCount;
	char szPath[MAX_PATH];
	char szNumber[5];
	Q_snprintf(szNumber, sizeof(szNumber), "%d", iRand + 1);
	Q_strncpy(szPath, pszBasePath, sizeof(szPath));
	Q_strncat(szPath, szNumber, sizeof(szPath));
	Q_strncat(szPath, ".bik", sizeof(szPath));
	char *szResult = szPath;
	return szResult;
}

char* CTFMainMenuPanel::GetRandomMusic()
{
	char* pszBasePath = "sound/ui/gamestartup";
	int iCount = 0;

	for (int i = 0; i < 27; i++)
	{
		char szPath[MAX_PATH];
		char szNumber[5];
		Q_snprintf(szNumber, sizeof(szNumber), "%d", iCount + 1);
		Q_strncpy(szPath, pszBasePath, sizeof(szPath));
		Q_strncat(szPath, szNumber, sizeof(szPath));
		Q_strncat(szPath, ".mp3", sizeof(szPath));
		if (!g_pFullFileSystem->FileExists(szPath))
		{
			if (iCount)
				break;
			else
				return "";
		}
		iCount++;
	}

	char* pszSoundPath = "ui/gamestartup";
	int iRand = rand() % iCount;
	char szPath[MAX_PATH];
	char szNumber[5];
	Q_snprintf(szNumber, sizeof(szNumber), "%d", iRand + 1);
	Q_strncpy(szPath, pszSoundPath, sizeof(szPath));
	Q_strncat(szPath, szNumber, sizeof(szPath));
	Q_strncat(szPath, ".mp3", sizeof(szPath));
	char *szResult = szPath;
	return szResult;
}