#include "cbase.h"
#include "tf_notificationpanel.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFNotificationPanel::CTFNotificationPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFNotificationPanel::~CTFNotificationPanel()
{

}

bool CTFNotificationPanel::Init( void )
{
	BaseClass::Init();

	m_pTitle = NULL;
	m_pMessage = NULL;

	return true;
}

void CTFNotificationPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/NotificationPanel.res");

	m_pTitle = dynamic_cast<CExLabel *>(FindChildByName("TitleLabel"));
	m_pMessage = dynamic_cast<CExLabel *>(FindChildByName("MessageLabel"));

	UpdateLabels();
}

void CTFNotificationPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFNotificationPanel::OnNotificationUpdate()
{
	Q_snprintf(sTitle, sizeof(sTitle), MAINMENU_ROOT->GetNotification().sTitle);
	Q_snprintf(sMessage, sizeof(sMessage), MAINMENU_ROOT->GetNotification().sMessage);
	surface()->PlaySound("ui/notification_alert.wav");
};

void CTFNotificationPanel::UpdateLabels()
{
	if (m_pTitle && m_pMessage)
	{
		m_pTitle->SetText(sTitle);
		m_pMessage->SetText(sMessage);
	}
}

void CTFNotificationPanel::Show()
{
	BaseClass::Show();

	UpdateLabels();
}

void CTFNotificationPanel::Hide()
{
	BaseClass::Hide();
}

void CTFNotificationPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		Hide();
	}
	else if (!stricmp(command, "Ok"))
	{
		Hide();
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFNotificationPanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFNotificationPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFNotificationPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
};

void CTFNotificationPanel::GameLayout()
{
	BaseClass::GameLayout();
};