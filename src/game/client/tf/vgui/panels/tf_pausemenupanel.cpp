#include "cbase.h"
#include "tf_pausemenupanel.h"
#include "controls/tf_advbutton.h"
#include "tf_notificationmanager.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFPauseMenuPanel::CTFPauseMenuPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
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

	m_pNotificationButton = NULL;
	bInMenu = false;
	bInGame = true;
	return true;
};


void CTFPauseMenuPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/PauseMenuPanel.res");
	m_pNotificationButton = dynamic_cast<CTFAdvButton*>(FindChildByName("NotificationButton"));
}

void CTFPauseMenuPanel::PerformLayout()
{
	BaseClass::PerformLayout();
	OnNotificationUpdate();
};


void CTFPauseMenuPanel::OnCommand(const char* command)
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
	else if (!Q_strcmp(command, "shownotification"))
	{
		if (m_pNotificationButton)
		{
			m_pNotificationButton->SetGlowing(false);
		}
		MAINMENU_ROOT->ShowPanel(NOTIFICATION_MENU);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFPauseMenuPanel::OnNotificationUpdate()
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
};


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
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_SIMPLESPLINE);
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
};