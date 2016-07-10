#include "cbase.h"
#include "tf_notificationpanel.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFNotificationPanel::CTFNotificationPanel( vgui::Panel* parent, const char *panelName ) : CTFMenuPanelBase( parent, panelName )
{
	Init();

	m_pPrevButton = NULL;
	m_pNextButton = NULL;
	m_pMessageLabel = NULL;
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

	m_iCurrent = 0;
	m_iCount = 0;

	return true;
}

void CTFNotificationPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "resource/UI/main_menu/NotificationPanel.res" );

	m_pPrevButton = dynamic_cast<CTFAdvButton *>( FindChildByName( "PrevButton" ) );
	m_pNextButton = dynamic_cast<CTFAdvButton *>( FindChildByName( "NextButton" ) );
	m_pMessageLabel = dynamic_cast<CExLabel *>( FindChildByName( "MessageLabel" ) );

	m_iMinHeight = GetTall();

	UpdateLabels();
}

void CTFNotificationPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFNotificationPanel::OnNotificationUpdate()
{
	m_iCount = GetNotificationManager()->GetNotificationsCount();
	if ( !IsVisible() )
	{
		for ( int i = 0; i < m_iCount; i++ )
		{
			MessageNotification *pNotification = GetNotificationManager()->GetNotification( i );
			if ( pNotification->bUnread )
			{
				m_iCurrent = i;
				break;
			}
		}
	}

	UpdateLabels();
};

void CTFNotificationPanel::UpdateLabels()
{
	m_iCount = GetNotificationManager()->GetNotificationsCount();
	if ( m_iCount <= 0 )
	{
		Hide();
		return;
	}
	if ( m_iCurrent >= m_iCount )
		m_iCurrent = m_iCount - 1;

	m_pNextButton->SetVisible( ( m_iCurrent < m_iCount - 1 ) );
	m_pPrevButton->SetVisible( ( m_iCurrent > 0 ) );

	char sCount[32];
	Q_snprintf( sCount, sizeof( sCount ), "(%d/%d)", m_iCurrent + 1, m_iCount );
	SetDialogVariable( "count", sCount );

	MessageNotification *pNotification = GetNotificationManager()->GetNotification( m_iCurrent );

	SetDialogVariable( "title", pNotification->wszTitle );
	SetDialogVariable( "message", pNotification->wszMessage );
	SetDialogVariable( "timestamp", pNotification->wszDate );
	
	if ( IsVisible() )
	{
		pNotification->bUnread = false;
	}
}

void CTFNotificationPanel::RemoveCurrent()
{
	GetNotificationManager()->RemoveNotification( m_iCurrent );
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

void CTFNotificationPanel::OnCommand( const char* command )
{
	if ( !Q_strcmp( command, "vguicancel" ) )
	{
		Hide();
	}
	else if ( !stricmp( command, "Ok" ) )
	{
		Hide();
	}
	else if ( !stricmp( command, "Next" ) )
	{
		if ( m_iCurrent < m_iCount - 1 )
			m_iCurrent++;
		UpdateLabels();
	}
	else if ( !stricmp( command, "Prev" ) )
	{
		if ( m_iCurrent > 0 )
			m_iCurrent--;
		UpdateLabels();
	}
	else if ( !stricmp( command, "Remove" ) )
	{
		RemoveCurrent();
	}
	else
	{
		BaseClass::OnCommand( command );
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