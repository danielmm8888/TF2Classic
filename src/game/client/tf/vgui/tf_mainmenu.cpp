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
	SetProportional(false);
	SetVisible(true);

	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);

	MainMenuPanel = new CTFMainMenuPanel(this);
	TestMenuPanel = new CTFTestMenuPanel(this);
	TestMenuPanel->SetVisible(false);

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenu::~CTFMainMenu()
{
	gameui = NULL;
	g_GameUIDLL.Unload();
}

void CTFMainMenu::ShowPanel(MenuPanel iPanel)
{
	switch (iPanel)
	{
	case MAIN_MENU:
		MainMenuPanel->SetVisible(true);
		break;
	case TEST_MENU:
		TestMenuPanel->SetVisible(true);
		break;
	default:
		break;
	}
}

void CTFMainMenu::HidePanel(MenuPanel iPanel)
{
	switch (iPanel)
	{
	case MAIN_MENU:
		MainMenuPanel->SetVisible(false);
		break;
	case TEST_MENU:
		TestMenuPanel->SetVisible(false);
		break;
	default:
		break;
	}
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

void CTFMainMenu::OnTick()
{
	BaseClass::OnTick();
	if (!engine->IsDrawingLoadingImage() && !IsVisible())
	{
		SetVisible(true);
	}
};

void CTFMainMenu::OnThink()
{
	BaseClass::OnThink();

	if (engine->IsDrawingLoadingImage() && IsVisible())
	{
		SetVisible(false);
	}
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
	else {
		return false;
	}
}