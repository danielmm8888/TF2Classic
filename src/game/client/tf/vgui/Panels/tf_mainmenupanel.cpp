#include "cbase.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbutton.h"
#include "engine/IEngineSound.h"
#include "steam/steam_api.h"
#include "steam/isteamhttp.h"
#include "vgui_avatarimage.h"
#include "soundenvelope.h"
#include <convar.h>

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define VERSION_URL			"http://services.0x13.io/tf2c/version/?latest=1"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuPanel::CTFMainMenuPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	SetMainMenu(GetParent());
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuPanel::~CTFMainMenuPanel()
{

}

bool CTFMainMenuPanel::Init()
{
	BaseClass::Init();

	m_bMusicPlay = true;
	m_flActionThink = -1;
	m_flAnimationThink = -1;
	m_flMusicThink = -1;
	m_bAnimationIn = true;

	m_SteamID = steamapicontext->SteamUser()->GetSteamID();
	m_SteamHTTP = steamapicontext->SteamHTTP();

	fPercent = -1.0f;
	bOutdated = false;
	bChecking = false;
	bCompleted = false;
	bUnread = false;
	bInMenu = true;
	bInGame = false;
	return true;
}


void CTFMainMenuPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/MainMenuPanel.res");
	m_pVersionLabel = dynamic_cast<CExLabel *>(FindChildByName("VersionLabel"));
	m_pNotificationButton = dynamic_cast<CTFAdvButton *>(FindChildByName("NotificationButton"));
	m_pProfileAvatar = dynamic_cast<CAvatarImagePanel *>(FindChildByName("AvatarImage"));
	m_pNicknameButton = dynamic_cast<CTFAdvButton *>(FindChildByName("NicknameButton"));
	SetVersionLabel();
}	

void CTFMainMenuPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	m_pProfileAvatar->SetPlayer(m_SteamID, k_EAvatarSize64x64);
	m_pProfileAvatar->SetShouldDrawFriendIcon(false);
	m_pNicknameButton->SetText(steamapicontext->SteamFriends()->GetPersonaName());
	m_pNicknameButton->SetDisabled(true);
	DefaultLayout();
	CheckVersion();
};

void CTFMainMenuPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "newquit"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(QUIT_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsdialog"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSDIALOG_MENU);
	}
	else if (!Q_strcmp(command, "newloadout"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(LOADOUT_MENU);
	}
	else if (!Q_strcmp(command, "checkversion"))
	{
		CheckVersion();
	}
	else if (!Q_strcmp(command, "shownotification"))
	{
		bUnread = false;
		m_pNotificationButton->SetVisible(false);
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(NOTIFICATION_MENU);
	}
	else if (!Q_strcmp(command, "testnotification"))
	{
		MainMenuNotification Notification("Yoyo", "TestingShit");
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->SendNotification(Notification);
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

void CTFMainMenuPanel::CheckVersion()
{
	char verString[64];
	Q_snprintf(verString, sizeof(verString), VERSION_URL);

	m_httpRequest = m_SteamHTTP->CreateHTTPRequest(k_EHTTPMethodGET, verString);
	m_SteamHTTP->SetHTTPRequestNetworkActivityTimeout(m_httpRequest, 5);

	SteamAPICall_t hSteamAPICall;
	m_SteamHTTP->SendHTTPRequest(m_httpRequest, &hSteamAPICall);
	m_CallResult.Set(hSteamAPICall, this, (&CTFMainMenuPanel::CHTTPRequestCompleted));

	fPercent = 0.0f;
	bChecking = true;
}

void CTFMainMenuPanel::CHTTPRequestCompleted(HTTPRequestCompleted_t *m_CallResult, bool iofailure)
{
	Msg("HTTP Request completed: %i\n", m_CallResult->m_eStatusCode);
	bCompleted = true;
	bChecking = false;
	fPercent = -1.0f;

	if (m_CallResult->m_eStatusCode == 200)
	{
		uint32 iBodysize;
		m_SteamHTTP->GetHTTPResponseBodySize(m_httpRequest, &iBodysize);
		uint8* iBodybuffer = new uint8();
		m_SteamHTTP->GetHTTPResponseBodyData(m_httpRequest, iBodybuffer, iBodysize);

		char result[64];
		Q_strncpy(result, (char*)iBodybuffer, iBodysize + 1);

		char resultString[64];
		if (Q_strcmp(GetVersionString(), result) < 0)
		{
			bOutdated = true;
			m_pVersionLabel->SetFgColor(Color(255, 20, 50, 100));
			
			Q_snprintf(resultString, sizeof(resultString), "Update your shit NOW!\nBTW, it's version: %s!", result);
			MainMenuNotification Notification("YOU CUNT", resultString);
			dynamic_cast<CTFMainMenu*>(GetMainMenu())->SendNotification(Notification);
		}
		else
		{
			bOutdated = false;
		}
	}
	else
	{
		//Msg("Can't get the info\n");
	}

	m_SteamHTTP->ReleaseHTTPRequest(m_httpRequest);
}

static void OnVariableChange(IConVar *var, const char *pOldValue, float flOldValue)
{
	if (((ConVar*)var)->GetBool() == false)
	{
		enginesound->NotifyBeginMoviePlayback();
	}
}
ConVar tf2c_mainmenu_music("tf2c_mainmenu_music", "1", FCVAR_ARCHIVE, "Plays music in MainMenu", OnVariableChange);

void CTFMainMenuPanel::OnTick()
{
	BaseClass::OnTick();

	if (bChecking && !bCompleted)
	{
		SteamAPI_RunCallbacks();
		m_SteamHTTP->GetHTTPDownloadProgressPct(m_httpRequest, &fPercent);
	}

	if (!bInGameLayout)
	{
		if (tf2c_mainmenu_music.GetBool())
		{
			if (m_bMusicPlay && m_flMusicThink < gpGlobals->curtime)
			{
				m_bMusicPlay = false;
				Q_strncpy(m_pzMusicLink, GetRandomMusic(), sizeof(m_pzMusicLink));
				m_flMusicThink = gpGlobals->curtime + enginesound->GetSoundDuration(m_pzMusicLink);
			}
			else if (!m_bMusicPlay && m_pzMusicLink[0] != '\0')
			{
				m_bMusicPlay = true;
				enginesound->NotifyBeginMoviePlayback();
				surface()->PlaySound(m_pzMusicLink);
			}
		}
		else
		{
			if (m_bMusicPlay)
			{				
				m_bMusicPlay = false;
			}
			else if (m_flMusicThink == -1)
			{
				m_flMusicThink = gpGlobals->curtime;
				enginesound->NotifyBeginMoviePlayback(); 
			}
		}

		if (bUnread && m_pNotificationButton && m_flAnimationThink < gpGlobals->curtime)
		{
			float m_fAlpha = (m_bAnimationIn ? 50.0f : 255.0f);
			float m_fDelay = (m_bAnimationIn ? 0.75f : 0.0f);
			float m_fDuration = (m_bAnimationIn ? 0.15f : 0.25f);
			vgui::GetAnimationController()->RunAnimationCommand(m_pNotificationButton, "Alpha", m_fAlpha, m_fDelay, m_fDuration, vgui::AnimationController::INTERPOLATOR_LINEAR);
			m_bAnimationIn = !m_bAnimationIn;
			m_flAnimationThink = gpGlobals->curtime + 1.0f;
		}

		/*
		if (bOutdated && m_pVersionLabel && m_flAnimationThink < gpGlobals->curtime)
		{
		float m_fAlpha = (m_bAnimationIn ? 50.0 : 100.0);
		vgui::GetAnimationController()->RunAnimationCommand(m_pVersionLabel, "Alpha", m_fAlpha, 0.0f, 0.25f, vgui::AnimationController::INTERPOLATOR_LINEAR);
		m_bAnimationIn = !m_bAnimationIn;
		m_flAnimationThink = gpGlobals->curtime + 0.25f;
		}
		*/
	}
};

void CTFMainMenuPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFMainMenuPanel::Show()
{
	BaseClass::Show();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};

void CTFMainMenuPanel::Hide()
{
	BaseClass::Hide();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};


void CTFMainMenuPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
};

void CTFMainMenuPanel::GameLayout()
{
	BaseClass::GameLayout();
};

void CTFMainMenuPanel::PlayMusic()
{

}

void CTFMainMenuPanel::OnNotificationUpdate()
{
	bUnread = true;
	m_pNotificationButton->SetVisible(true);
};

void CTFMainMenuPanel::SetVersionLabel()  //GetVersionString
{
	if (m_pVersionLabel)
	{
		char verString[30];
		Q_snprintf(verString, sizeof(verString), "Version: %s", GetVersionString());
		m_pVersionLabel->SetText(verString);
	}
};

char* CTFMainMenuPanel::GetVersionString()
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
	char *szResult = (char*)malloc(sizeof(szPath));
	Q_strncpy(szResult, szPath, sizeof(szPath));
	return szResult;	
}