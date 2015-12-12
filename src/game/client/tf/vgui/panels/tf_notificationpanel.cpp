#include "cbase.h"
#include "tf_notificationpanel.h"
#include "tf_notificationmanager.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbutton.h"

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

bool CTFNotificationPanel::Init(void)
{
	BaseClass::Init();

	iCurrent = 0;
	iCount = 0;

	return true;
}

void CTFNotificationPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/NotificationPanel.res");

	UpdateLabels();
}

void CTFNotificationPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFNotificationPanel::OnNotificationUpdate()
{
	iCount = GetNotificationManager()->GetNotificationsCount();
	if (!IsVisible())
	{
		for (int i = 0; i < iCount; i++)
		{
			MessageNotification *pNotification = GetNotificationManager()->GetNotification(i);
			if (pNotification->bUnread)
			{
				iCurrent = i;
				break;
			}
		}
	}
	surface()->PlaySound("ui/notification_alert.wav");

	UpdateLabels();
};

void CTFNotificationPanel::UpdateLabels()
{
	iCount = GetNotificationManager()->GetNotificationsCount();
	if (iCount <= 0)
	{
		Hide();
		return;
	}
	if (iCurrent >= iCount)
		iCurrent = iCount - 1;

	dynamic_cast<CTFAdvButton *>(FindChildByName("NextButton"))->SetVisible((iCurrent < iCount - 1));
	dynamic_cast<CTFAdvButton *>(FindChildByName("PrevButton"))->SetVisible((iCurrent > 0));

	char sCount[32];
	Q_snprintf(sCount, sizeof(sCount), "(%d/%d)", iCurrent + 1, iCount);
	SetDialogVariable("count", sCount);

	MessageNotification* pNotification = GetNotificationManager()->GetNotification(iCurrent);
	Q_snprintf(sTitle, sizeof(sTitle), pNotification->sTitle);
	Q_snprintf(sMessage, sizeof(sMessage), pNotification->sMessage);

	SetDialogVariable("title", sTitle);
	SetDialogVariable("message", sMessage);
	pNotification->bUnread = !IsVisible();
}

void CTFNotificationPanel::RemoveCurrent()
{
	GetNotificationManager()->RemoveNotification(iCurrent);
	UpdateLabels();
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
	else if (!stricmp(command, "Next"))
	{
		if (iCurrent < iCount - 1)
			iCurrent++;
		UpdateLabels();
	}
	else if (!stricmp(command, "Prev"))
	{
		if (iCurrent > 0)
			iCurrent--;
		UpdateLabels();
	}
	else if (!stricmp(command, "Remove"))
	{
		RemoveCurrent();
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