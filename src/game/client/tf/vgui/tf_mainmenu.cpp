#include "cbase.h"
#include "tf_mainmenu.h"
#include "tf_mainmenubutton.h"
#include "vgui_controls/Frame.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>

#include "vgui_controls/Button.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_controls.h"
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>


using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CMainMenuPanel : public vgui::EditablePanel
{
	typedef vgui::EditablePanel BaseClass;

public:
	CMainMenuPanel(vgui::VPANEL parent);
	~CMainMenuPanel();

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme)
	{

		BaseClass::ApplySchemeSettings(pScheme);
	}

	// We want the panel background image to be square, not rounded.
	virtual void PaintBackground()
	{
		SetPaintBackgroundType(0);
		BaseClass::PaintBackground();
	}
	bool InGame()
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

	void OnCommand(const char* command);
	void OnThink();
	void DefaultLayout();
	void GameLayout();

private:
	CExLabel			*m_pVersionLabel;
	CTFMainMenuButton	*m_pDisconnectButton;
	CTFImagePanel		*m_pBackground;
	CTFImagePanel		*m_pLogo;
	bool			InGameLayout;	
	//CExButton		*m_pQuitButton;

};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CMainMenuPanel::CMainMenuPanel(vgui::VPANEL parent) : BaseClass(NULL, "CMainMenuPanel")
{
	SetParent(parent);
	SetProportional(false);
	SetVisible(true);
	SetScheme("ClientScheme");

	// Size of the panel, since your logo is a VTF, you should set this (width,height) to the proper dimensions
	SetSize(1920, 1080);
	SetPos(0, 0);

	// Loading the .res file.
	LoadControlSettings("resource/UI/MainMenu.res");
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	InGameLayout = false;
	m_pVersionLabel = dynamic_cast<CExLabel *>(FindChildByName("VersionLabel")); 
	m_pBackground = dynamic_cast<CTFImagePanel *>(FindChildByName("Background"));
	m_pDisconnectButton = dynamic_cast<CTFMainMenuButton *>(FindChildByName("DisconnectButton"));
	m_pLogo = dynamic_cast<CTFImagePanel *>(FindChildByName("Logo"));

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
	DefaultLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CMainMenuPanel::~CMainMenuPanel()
{
}

void CMainMenuPanel::OnCommand(const char* command)
{
	engine->ExecuteClientCmd(command);
}

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
};

void CMainMenuPanel::DefaultLayout()
{
	if (m_pDisconnectButton->OnlyInGame())
	{
		m_pDisconnectButton->SetVisible(false);
	};
}

void CMainMenuPanel::GameLayout()
{
	if (m_pDisconnectButton->OnlyInGame())
	{
		m_pDisconnectButton->SetVisible(true);
	};
}

// Class
class CMainMenu : public IMainMenu
{
private:
	CMainMenuPanel *MainMenuPanel;
	vgui::VPANEL m_hParent;

public:
	CMainMenu(void)
	{
		MainMenuPanel = NULL;
	}

	void Create(vgui::VPANEL parent)
	{
		// Create immediately
		MainMenuPanel = new CMainMenuPanel(parent);
	}

	void Destroy(void)
	{
		if (MainMenuPanel)
		{
			MainMenuPanel->SetParent((vgui::Panel *)NULL);	
			delete MainMenuPanel;
		}
	}

};

static CMainMenu g_MainMenu;
IMainMenu *MainMenu = (IMainMenu *)&g_MainMenu;