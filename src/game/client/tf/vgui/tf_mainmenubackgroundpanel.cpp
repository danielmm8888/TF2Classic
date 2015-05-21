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
CTFMainMenuBackgroundPanel::CTFMainMenuBackgroundPanel(vgui::Panel* parent) : CTFMainMenuPanelBase(parent)
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
	LoadControlSettings("resource/UI/main_menu/BackgroundMenu.res");
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	m_pBackground = dynamic_cast<CTFImagePanel *>(FindChildByName("Background"));
	m_pVideo = dynamic_cast<CTFVideoPanel *>(FindChildByName("BackgroundVideo"));

	Q_strncpy(m_pzVideoLink, GetRandomVideo(), sizeof(m_pzVideoLink));

	DefaultLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuBackgroundPanel::~CTFMainMenuBackgroundPanel()
{

}

void CTFMainMenuBackgroundPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMainMenuBackgroundPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFMainMenuBackgroundPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

void CTFMainMenuBackgroundPanel::VideoReplay()
{
	if (m_pzVideoLink[0] != '\0' && !bInGameLayout)
	{
		m_pVideo->SetVisible(true);
		m_pVideo->SetEnabled(true);
		m_pVideo->BeginPlaybackNoAudio(m_pzVideoLink);
	}
}

void CTFMainMenuBackgroundPanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFMainMenuBackgroundPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFMainMenuBackgroundPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();

	VideoReplay();
	if (m_pVideo)
	{
		m_pVideo->SetVisible(true);
	}
};

void CTFMainMenuBackgroundPanel::GameLayout()
{
	BaseClass::GameLayout();

	if (m_pVideo)
	{
		m_pVideo->SetVisible(false);
	}
};

char* CTFMainMenuBackgroundPanel::GetRandomVideo()
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
	char *szResult = (char*)malloc(sizeof(szPath));
	Q_strncpy(szResult, szPath, sizeof(szPath));
	return szResult;
}