#include "cbase.h"
#include "tf_mainmenupanel.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advslider.h"
#include "vgui_controls/SectionedListPanel.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_notificationmanager.h"
#include "engine/IEngineSound.h"
#include "vgui_avatarimage.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define BLOG_URL "http://tf2classic.com/?nolinks=1&noheader=1&nofooter=1&fillwrapper=1"

static void OnBlogToggle(IConVar *var, const char *pOldValue, float flOldValue)
{
	GET_MAINMENUPANEL(CTFMainMenuPanel)->ShowBlogPanel(((ConVar*)var)->GetBool());
}
ConVar tf2c_mainmenu_music("tf2c_mainmenu_music", "1", FCVAR_ARCHIVE, "Toggle music in the main menu");
ConVar tf2c_mainmenu_showblog("tf2c_mainmenu_showblog", "0", FCVAR_ARCHIVE, "Toggle blog in the main menu", OnBlogToggle);

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

	if (steamapicontext->SteamUser())
	{
		m_SteamID = steamapicontext->SteamUser()->GetSteamID();
	}

	m_iShowFakeIntro = 4;
	m_pVersionLabel = NULL;
	m_pNotificationButton = NULL;
	m_pProfileAvatar = NULL;
	m_pFakeBGImage = NULL;
	m_pBlogPanel = new CTFBlogPanel(this, "BlogPanel");
	m_pServerlistPanel = new CTFServerlistPanel(this, "ServerlistPanel");

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
	m_pFakeBGImage = dynamic_cast<vgui::ImagePanel *>(FindChildByName("FakeBGImage"));

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

	char szNickName[64];
	Q_snprintf(szNickName, sizeof(szNickName),
		(steamapicontext->SteamFriends() ? steamapicontext->SteamFriends()->GetPersonaName() : "Unknown"));
	SetDialogVariable("nickname", szNickName);

	ShowBlogPanel(tf2c_mainmenu_showblog.GetBool() || GetNotificationManager()->IsOutdated());
	OnNotificationUpdate();
	AutoLayout();

	if (m_iShowFakeIntro > 0)
	{
		char szBGName[128];
		engine->GetMainMenuBackgroundName(szBGName, sizeof(szBGName));
		char szImage[128];
		Q_snprintf(szImage, sizeof(szImage), "../console/%s", szBGName);
		int width, height;
		surface()->GetScreenSize(width, height);
		float fRatio = (float)width / (float)height;
		bool bWidescreen = (fRatio < 1.5 ? false : true);
		if (bWidescreen)
			Q_strcat(szImage, "_widescreen", sizeof(szImage));
		m_pFakeBGImage->SetImage(szImage);
		m_pFakeBGImage->SetVisible(true);
		m_pFakeBGImage->SetAlpha(255);
	}
};

void CTFMainMenuPanel::ShowBlogPanel(bool show)
{
	if (m_pBlogPanel)
	{
		m_pBlogPanel->SetVisible(show);
		if (show)
		{
			m_pBlogPanel->LoadBlogPost(BLOG_URL);
		}
	}
}

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
		//MAINMENU_ROOT->CheckVersion();
	}
	else if (!Q_strcmp(command, "shownotification"))
	{
		if (m_pNotificationButton)
		{
			m_pNotificationButton->SetGlowing(false);
		}
		MAINMENU_ROOT->ShowPanel(NOTIFICATION_MENU);
	}
	else if (!Q_strcmp(command, "testnotification"))
	{
		wchar_t resultString[128];
		V_snwprintf(resultString, sizeof(resultString), L"test %d", GetNotificationManager()->GetNotificationsCount());
		MessageNotification Notification(L"Yoyo", resultString, time( NULL ) );
		GetNotificationManager()->SendNotification(Notification);
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

void CTFMainMenuPanel::OnTick()
{
	BaseClass::OnTick();

	if (tf2c_mainmenu_music.GetBool() && !bInGameLayout)
	{
		if ((m_psMusicStatus == MUSIC_FIND || m_psMusicStatus == MUSIC_STOP_FIND) && !enginesound->IsSoundStillPlaying(m_nSongGuid))
		{
			GetRandomMusic(m_pzMusicLink, sizeof(m_pzMusicLink));
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

	if (m_iShowFakeIntro > 0)
	{
		m_iShowFakeIntro--;
		if (m_iShowFakeIntro == 0)
		{
			vgui::GetAnimationController()->RunAnimationCommand(m_pFakeBGImage, "Alpha", 0, 1.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
		}
	}	
	if (m_pFakeBGImage->IsVisible() && m_pFakeBGImage->GetAlpha() == 0)
	{
		m_pFakeBGImage->SetVisible(false);
	}
};

void CTFMainMenuPanel::Show()
{
	BaseClass::Show();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
};

void CTFMainMenuPanel::Hide()
{
	BaseClass::Hide();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};


void CTFMainMenuPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
	ShowBlogPanel(tf2c_mainmenu_showblog.GetBool());
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
		if (GetNotificationManager()->GetNotificationsCount() > 0)
		{
			m_pNotificationButton->SetVisible(true);
		}
		else
		{
			m_pNotificationButton->SetVisible(false);
		}

		if (GetNotificationManager()->GetUnreadNotificationsCount() > 0)
		{
			m_pNotificationButton->SetGlowing(true);
		}
		else
		{
			m_pNotificationButton->SetGlowing(false);
		}
	}
	if (GetNotificationManager()->IsOutdated())
	{
		if (m_pVersionLabel)
		{
			m_pVersionLabel->SetFgColor(Color(255, 20, 50, 100));
		}
	}
};

void CTFMainMenuPanel::SetVersionLabel()  //GetVersionString
{
	if (m_pVersionLabel)
	{
		char verString[64];
		Q_snprintf(verString, sizeof(verString), "Version: %s", GetNotificationManager()->GetVersionName());
		m_pVersionLabel->SetText(verString);
	}
};

void CTFMainMenuPanel::GetRandomMusic(char *pszBuf, int iBufLength)
{
	Assert(iBufLength);

	char szPath[MAX_PATH];

	// Check that there's music available
	if (!g_pFullFileSystem->FileExists("sound/ui/gamestartup1.mp3"))
	{
		Assert(false);
		*pszBuf = '\0';
	}

	// Discover tracks, 1 through n
	int iLastTrack = 0;
	do
	{
		Q_snprintf(szPath, sizeof(szPath), "sound/ui/gamestartup%d.mp3", ++iLastTrack);
	} while (g_pFullFileSystem->FileExists(szPath));

	// Pick a random one
	Q_snprintf(szPath, sizeof(szPath), "ui/gamestartup%d.mp3", RandomInt(1, iLastTrack - 1));
	Q_strncpy(pszBuf, szPath, iBufLength);
}

void CTFMainMenuPanel::SetServerlistSize(int size)
{
	m_pServerlistPanel->SetServerlistSize(size);
}

void CTFMainMenuPanel::UpdateServerInfo()
{
	m_pServerlistPanel->UpdateServerInfo();
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFBlogPanel::CTFBlogPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	m_pHTMLPanel = new vgui::HTML(this, "HTMLPanel");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFBlogPanel::~CTFBlogPanel()
{
}

void CTFBlogPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/BlogPanel.res");
}

void CTFBlogPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	LoadBlogPost(BLOG_URL);
}

void CTFBlogPanel::LoadBlogPost(const char* URL)
{
	if (m_pHTMLPanel)
	{
		m_pHTMLPanel->SetVisible(true);
		m_pHTMLPanel->OpenURL(URL, NULL);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFServerlistPanel::CTFServerlistPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	m_iSize = 0;
	m_pServerList = new vgui::SectionedListPanel(this, "ServerList");
	m_pConnectButton = new CTFAdvButton(this, "ConnectButton", "Connect");
	m_pListSlider = new CTFAdvSlider(this, "ListSlider", "");
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFServerlistPanel::~CTFServerlistPanel()
{
}

void CTFServerlistPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/ServerlistPanel.res");

	m_pServerList->RemoveAll();
	m_pServerList->RemoveAllSections();
	m_pServerList->SetSectionFgColor(0, Color(255, 255, 255, 255));
	m_pServerList->SetBgColor(Color(0, 0, 0, 0));
	m_pServerList->SetBorder(NULL);
	m_pServerList->AddSection(0, "Servers", ServerSortFunc);
	m_pServerList->AddColumnToSection(0, "Name", "Servers", SectionedListPanel::COLUMN_BRIGHT, m_iServerWidth);
	m_pServerList->AddColumnToSection(0, "Players", "Players", SectionedListPanel::COLUMN_BRIGHT, m_iPlayersWidth);
	m_pServerList->AddColumnToSection(0, "Ping", "Ping", SectionedListPanel::COLUMN_BRIGHT, m_iPingWidth);
	m_pServerList->AddColumnToSection(0, "Map", "Map", SectionedListPanel::COLUMN_BRIGHT, m_iMapWidth);
	m_pServerList->SetSectionAlwaysVisible(0, true);
	m_pServerList->GetScrollBar()->UseImages("", "", "", ""); //hack to hide the scrollbar

	m_pConnectButton->SetVisible(false);
	UpdateServerInfo();
}

void CTFServerlistPanel::PerformLayout()
{
	BaseClass::PerformLayout();
}

void CTFServerlistPanel::OnThink()
{
	m_pServerList->ClearSelection();
	m_pListSlider->SetVisible(false);
	m_pConnectButton->SetVisible(false);

	if (!IsCursorOver())
		return;

	m_pListSlider->SetValue(m_pServerList->GetScrollBar()->GetValue());

	for (int i = 0; i < m_pServerList->GetItemCount(); i++)
	{
		int _x, _y;
		m_pServerList->GetPos(_x, _y);
		int x, y, wide, tall;
		m_pServerList->GetItemBounds(i, x, y, wide, tall);
		int cx, cy;
		surface()->SurfaceGetCursorPos(cx, cy);
		m_pServerList->ScreenToLocal(cx, cy);

		if (cx > x && cx < x + wide && cy > y && cy < y + tall)
		{
			m_pServerList->SetSelectedItem(i);
			int by = y + _y;
			m_pConnectButton->SetPos(m_iServerWidth + m_iPlayersWidth + m_iPingWidth, by);
			m_pConnectButton->SetVisible(true);
			m_pListSlider->SetVisible(true);

			char szCommand[128];
			Q_snprintf(szCommand, sizeof(szCommand), "connect %s", m_pServerList->GetItemData(i)->GetString("ServerIP", ""));
			m_pConnectButton->SetCommandString(szCommand);
		}
	}
}

void CTFServerlistPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "scrolled"))
	{
		m_pServerList->GetScrollBar()->SetValue(m_pListSlider->GetValue());
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Used for sorting servers
//-----------------------------------------------------------------------------
bool CTFServerlistPanel::ServerSortFunc(vgui::SectionedListPanel *list, int itemID1, int itemID2)
{
	KeyValues *it1 = list->GetItemData(itemID1);
	KeyValues *it2 = list->GetItemData(itemID2);
	Assert(it1 && it2);

	int v1 = it1->GetInt("CurPlayers");
	int v2 = it2->GetInt("CurPlayers");
	if (v1 > v2)
		return true;
	else if (v1 < v2)
		return false;

	/*
	int iOff1 = it1->GetBool("Official");
	int iOff2 = it2->GetBool("Official");
	if (iOff1 && !iOff2)
		return true;
	else if (!iOff1 && iOff2)
		return false;
	*/

	int iPing1 = it1->GetInt("Ping");
	if (iPing1 == 0)
		return false;
	int iPing2 = it2->GetInt("Ping");
	return (iPing1 < iPing2);
}

void CTFServerlistPanel::SetServerlistSize(int size) 
{
	m_iSize = size;
};

void CTFServerlistPanel::UpdateServerInfo()
{
	m_pServerList->RemoveAll();
	HFont Font = GETSCHEME()->GetFont("FontStoreOriginalPrice", true);

	for (int i = 0; i < m_iSize; i++)
	{
		gameserveritem_t m_Server = GetNotificationManager()->GetServerInfo(i);		
		if (m_Server.m_steamID.GetAccountID() == 0)
			continue;

		bool bOfficial = GetNotificationManager()->IsOfficialServer(i);
		if (!bOfficial)
			continue;

		char szServerName[128];
		char szServerIP[32];
		char szServerPlayers[16];
		int szServerCurPlayers;
		int szServerPing;
		char szServerMap[32];

		Q_snprintf(szServerName, sizeof(szServerName), "%s", m_Server.GetName());
		Q_snprintf(szServerIP, sizeof(szServerIP), "%s", m_Server.m_NetAdr.GetQueryAddressString());
		Q_snprintf(szServerPlayers, sizeof(szServerPlayers), "%i/%i", m_Server.m_nPlayers, m_Server.m_nMaxPlayers);
		szServerCurPlayers = m_Server.m_nPlayers;
		szServerPing = m_Server.m_nPing;
		Q_snprintf(szServerMap, sizeof(szServerMap), "%s", m_Server.m_szMap);

		KeyValues *curitem = new KeyValues("data");

		curitem->SetString("Name", szServerName);
		curitem->SetString("ServerIP", szServerIP);
		curitem->SetString("Players", szServerPlayers);
		curitem->SetInt("Ping", szServerPing);
		curitem->SetInt("CurPlayers", szServerCurPlayers);
		curitem->SetString("Map", szServerMap);
		//curitem->SetBool("Official", bOfficial);		

		int itemID = m_pServerList->AddItem(0, curitem);
		/*
		if (bOfficial)
			m_pServerList->SetItemFgColor(itemID, GETSCHEME()->GetColor("TeamYellow", Color(255, 255, 255, 255)));
		else
			m_pServerList->SetItemFgColor(itemID, GETSCHEME()->GetColor("AdvTextDefault", Color(255, 255, 255, 255)));
		*/
		m_pServerList->SetItemFont(itemID, Font);
		curitem->deleteThis();
	}

	if (m_pServerList->GetItemCount() > 0)
	{
		SetVisible(true);
	}
	else
	{
		SetVisible(false);
	}

	int min, max;
	m_pServerList->InvalidateLayout(1, 0);
	m_pServerList->GetScrollBar()->GetRange(min, max);
	m_pListSlider->SetRange(min, max - m_pServerList->GetScrollBar()->GetButton(0)->GetTall() * 4);
}