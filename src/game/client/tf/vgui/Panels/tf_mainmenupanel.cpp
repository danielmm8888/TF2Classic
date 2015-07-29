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

	m_psMusicStatus = MUSIC_FIND;
	m_pzMusicLink[0] = '\0';
	m_nSongGuid = 0;

	m_SteamID = steamapicontext->SteamUser()->GetSteamID();
	m_SteamHTTP = steamapicontext->SteamHTTP();
	m_pVersionLabel = NULL;
	m_pNotificationButton = NULL;
	m_pProfileAvatar = NULL;
	m_pNicknameButton = NULL;

	fPercent = -1.0f;
	bOutdated = false;
	bChecking = false;
	bCompleted = false;
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

	if (m_pProfileAvatar)
	{
		m_pProfileAvatar->SetPlayer(m_SteamID, k_EAvatarSize64x64);
		m_pProfileAvatar->SetShouldDrawFriendIcon(false);
	}
	if (m_pNicknameButton)
	{
		m_pNicknameButton->SetText(steamapicontext->SteamFriends()->GetPersonaName());
		m_pNicknameButton->SetDisabled(true);
	}
	DefaultLayout();
	CheckVersion();
};

void CTFMainMenuPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "newquit"))
	{
		MAINMENU_ROOT->ShowPanel(QUIT_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsdialog"))
	{
		MAINMENU_ROOT->ShowPanel(OPTIONSDIALOG_MENU);
	}
	else if (!Q_strcmp(command, "newloadout"))
	{
		MAINMENU_ROOT->ShowPanel(LOADOUT_MENU);
	}
	else if (!Q_strcmp(command, "newstats"))
	{
		MAINMENU_ROOT->ShowPanel(STATSUMMARY_MENU);
	}
	else if (!Q_strcmp(command, "checkversion"))
	{
		CheckVersion();
	}
	else if (!Q_strcmp(command, "shownotification"))
	{
		if (m_pNotificationButton)
		{
			m_pNotificationButton->SetVisible(false);
		}
		MAINMENU_ROOT->ShowPanel(NOTIFICATION_MENU);
	}
	else if (!Q_strcmp(command, "testnotification"))
	{
		MainMenuNotification Notification("Yoyo", "Testing");
		MAINMENU_ROOT->SendNotification(Notification);
	}
	else if (!Q_strcmp(command, "randommusic"))
	{
		enginesound->StopSoundByGuid(m_nSongGuid);
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

		char result[128];
		Q_strncpy(result, (char*)iBodybuffer, iBodysize + 1);

		char resultString[128];
		if (Q_strcmp(GetVersionString(), result) < 0)
		{
			bOutdated = true;
			if (m_pVersionLabel)
			{
				m_pVersionLabel->SetFgColor(Color(255, 20, 50, 100));
			}
			
			Q_snprintf(resultString, sizeof(resultString), "Your game is out of date.\nThe newest version of TF2C is %s.\nDownload the update at\nwww.tf2classic.com", result);
			MainMenuNotification Notification("Update!", resultString);
			MAINMENU_ROOT->SendNotification(Notification);
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

	if (tf2c_mainmenu_music.GetBool() && !bInGameLayout)
	{
		if ((m_psMusicStatus == MUSIC_FIND || m_psMusicStatus == MUSIC_STOP_FIND) && !enginesound->IsSoundStillPlaying(m_nSongGuid))
		{
			Q_strncpy(m_pzMusicLink, GetRandomMusic(), sizeof(m_pzMusicLink));
			m_psMusicStatus = MUSIC_PLAY;
		}
		else if ((m_psMusicStatus == MUSIC_PLAY || m_psMusicStatus == MUSIC_STOP_PLAY)&& m_pzMusicLink[0] != '\0')
		{
			enginesound->StopSoundByGuid(m_nSongGuid);
			ConVar *snd_musicvolume = cvar->FindVar("snd_musicvolume");
			float fVolume = (snd_musicvolume ? snd_musicvolume->GetFloat() : 1.0f);
			enginesound->EmitAmbientSound(m_pzMusicLink, fVolume, PITCH_NORM, 0);			
			m_nSongGuid = enginesound->GetGuidForLastSoundEmitted();
			m_psMusicStatus = MUSIC_FIND;
		}
	}
	else if (m_psMusicStatus == MUSIC_FIND)
	{
		enginesound->StopSoundByGuid(m_nSongGuid);
		m_psMusicStatus = (m_nSongGuid == 0 ? MUSIC_STOP_FIND : MUSIC_STOP_PLAY);
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
	if (m_pNotificationButton)
	{
		m_pNotificationButton->SetVisible(true);
		m_pNotificationButton->SetGlowing(true);
	}
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