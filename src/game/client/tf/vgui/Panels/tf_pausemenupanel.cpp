#include "cbase.h"
#include "tf_pausemenupanel.h"
#include "tf_mainmenu.h"
#include "tf_rgbpanel.h"
#include "tf_gamerules.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFPauseMenuPanel::CTFPauseMenuPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	SetMainMenu(GetParent());	
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFPauseMenuPanel::~CTFPauseMenuPanel()
{

}

bool CTFPauseMenuPanel::Init()
{
	BaseClass::Init();

	bInGame = true;
	return true;
};


void CTFPauseMenuPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	m_pRGBPanel = new CTFRGBPanel(this, "CTFRGBPanel");
	LoadControlSettings("resource/UI/main_menu/PauseMenuPanel.res");
}

void CTFPauseMenuPanel::PerformLayout()
{
	BaseClass::PerformLayout();
	DefaultLayout();
};


void CTFPauseMenuPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "newquit"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(QUIT_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsdialog"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSDIALOG_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsadv"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSADV_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsmouse"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSMOUSE_MENU);
	}
	else if (!Q_strcmp(command, "newoptionskeyboard"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSKEYBOARD_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsaudio"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSAUDIO_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsvideo"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSVIDEO_MENU);
	}
	else if (!Q_strcmp(command, "newloadout"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(LOADOUT_MENU);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}


void CTFPauseMenuPanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFPauseMenuPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFPauseMenuPanel::Show()
{
	BaseClass::Show();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};

void CTFPauseMenuPanel::Hide()
{
	BaseClass::Hide();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};


void CTFPauseMenuPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
};

void CTFPauseMenuPanel::GameLayout()
{
	BaseClass::GameLayout();
	
	if (m_pRGBPanel && TFGameRules())
	{
		if (TFGameRules()->IsDeathmatch())
		{
			m_pRGBPanel->SetVisible(true);
		}
		else
		{
			m_pRGBPanel->SetVisible(false);
		}
	}

};