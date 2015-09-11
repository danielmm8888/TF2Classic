#include "cbase.h"
#include "tf_backgroundpanel.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define DEFAULT_RATIO_WIDE 1920.0 / 1080.0
#define DEFAULT_RATIO 1024.0 / 768.0

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFBackgroundPanel::CTFBackgroundPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFBackgroundPanel::~CTFBackgroundPanel()
{

}

bool CTFBackgroundPanel::Init()
{
	BaseClass::Init();

	m_pVideo = NULL;
	bInMenu = true;
	bInGame = false;
	return true;
}

void CTFBackgroundPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/BackgroundPanel.res");
	m_pVideo = dynamic_cast<CTFVideoPanel *>(FindChildByName("BackgroundVideo"));

	if (m_pVideo)
	{
		int width, height;
		surface()->GetScreenSize(width, height);

		float fRatio = (float)width / (float)height;
		bool bWidescreen = (fRatio < 1.5 ? false : true);

		Q_strncpy(m_pzVideoLink, GetRandomVideo(bWidescreen), sizeof(m_pzVideoLink));
		float iRatio = (bWidescreen ? DEFAULT_RATIO_WIDE : DEFAULT_RATIO);
		int iWide = (float)height * iRatio + 4;
		m_pVideo->SetBounds(-1, -1, iWide, iWide);
	}
}

void CTFBackgroundPanel::PerformLayout()
{
	BaseClass::PerformLayout();
	AutoLayout();
};


void CTFBackgroundPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

void CTFBackgroundPanel::VideoReplay()
{
	if (!m_pVideo)
		return;
	
	if (IsVisible() && m_pzVideoLink[0] != '\0' && !bInGameLayout)
	{
		m_pVideo->Activate();
		m_pVideo->BeginPlaybackNoAudio(m_pzVideoLink);
	}
	else
	{
		m_pVideo->Shutdown();
	}
}

void CTFBackgroundPanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFBackgroundPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFBackgroundPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
	VideoReplay();
};

void CTFBackgroundPanel::GameLayout()
{
	BaseClass::GameLayout();
	VideoReplay();
};

char* CTFBackgroundPanel::GetRandomVideo(bool bWidescreen)
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
		if (bWidescreen)
			Q_strncat(szPath, "_widescreen", sizeof(szPath));
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
	if (bWidescreen)
		Q_strncat(szPath, "_widescreen", sizeof(szPath));
	Q_strncat(szPath, ".bik", sizeof(szPath));
	char *szResult = (char*)malloc(sizeof(szPath));
	Q_strncpy(szResult, szPath, sizeof(szPath));
	return szResult;
}