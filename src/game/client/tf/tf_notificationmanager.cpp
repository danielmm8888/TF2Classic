#include "cbase.h"
#include "tf_notificationmanager.h"
#include "tf_mainmenu.h"
#include "filesystem.h"
#include "script_parser.h"
#include "tf_gamerules.h"
#include "tf_hud_notification_panel.h"
#include "vgui/ISurface.h"
#include "vgui/ILocalize.h"
#include "fmtstr.h"
//#include "public\steam\matchmakingtypes.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define TF_NOTIFICATION_TEST_TIME "1456596000"

const char *g_aRequestURLs[REQUEST_COUNT] =
{
	"http://services.0x13.io/tf2c/version/?latest=1",
	"http://services.0x13.io/tf2c/motd/",
	"http://services.0x13.io/tf2c/servers/official/"
};

MessageNotification::MessageNotification( void )
{
	timeStamp = 0;
	wszTitle[0] = '\0';
	wszMessage[0] = '\0';
	wszDate[0] = '\0';
	bUnread = true;
};

MessageNotification::MessageNotification( const char *Title, const char *Message, time_t timeVal )
{
	g_pVGuiLocalize->ConvertANSIToUnicode( Title, wszTitle, sizeof( wszTitle ) );
	g_pVGuiLocalize->ConvertANSIToUnicode( Message, wszMessage, sizeof( wszMessage ) );
	bUnread = true;
	SetTimeStamp( timeVal );
};

MessageNotification::MessageNotification( const wchar_t *Title, const wchar_t *Message, time_t timeVal )
{
	V_wcsncpy( wszTitle, Title, sizeof( wszTitle ) );
	V_wcsncpy( wszMessage, Message, sizeof( wszMessage ) );
	bUnread = true;
	SetTimeStamp( timeVal );
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void MessageNotification::SetTimeStamp( time_t timeVal )
{
	timeStamp = timeVal;
	
	char szDate[64];
	BGetLocalFormattedDate( timeStamp, szDate, sizeof( szDate ) );

	g_pVGuiLocalize->ConvertANSIToUnicode( szDate, wszDate, sizeof( wszDate ) );
}

static CTFNotificationManager g_TFNotificationManager;
CTFNotificationManager *GetNotificationManager()
{
	return &g_TFNotificationManager;
}

CON_COMMAND_F( tf2c_checkmessages, "Check for the messages", FCVAR_DEVELOPMENTONLY )
{
	GetNotificationManager()->CheckVersionAndMessages();
}

CON_COMMAND_F( tf2c_updateserverlist, "Check for the messages", FCVAR_DEVELOPMENTONLY )
{
	GetNotificationManager()->UpdateServerlistInfo();
}

ConVar tf2c_checkfrequency( "tf2c_checkfrequency", "900", FCVAR_DEVELOPMENTONLY, "Messages check frequency (seconds)" );
ConVar tf2c_updatefrequency( "tf2c_updatefrequency", "15", FCVAR_DEVELOPMENTONLY, "Updatelist update frequency (seconds)" );
ConVar tf2c_latest_notification( "tf2c_latest_notification", "0", FCVAR_ARCHIVE );

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CTFNotificationManager::CTFNotificationManager() : CAutoGameSystemPerFrame( "CTFNotificationManager" )
{
	if ( !filesystem )
		return;

	m_bInited = false;
	Init();
}

CTFNotificationManager::~CTFNotificationManager()
{
}

//-----------------------------------------------------------------------------
// Purpose: Initializer
//-----------------------------------------------------------------------------
bool CTFNotificationManager::Init()
{
	if ( !m_bInited )
	{
		m_Notifications.RemoveAll();

		m_SteamHTTP = steamapicontext->SteamHTTP();
		SetDefLessFunc( m_Requests );
		SetDefLessFunc( m_Servers );
		m_flLastCheck = tf2c_checkfrequency.GetFloat() * -1;
		m_flUpdateLastCheck = tf2c_updatefrequency.GetFloat() * -1;
		m_iCurrentRequest = REQUEST_IDLE;
		m_bCompleted = false;
		m_bOutdated = false;
		m_bPlayedSound = false;
		m_bInited = true;

		m_hRequest = 0;
		MatchMakingKeyValuePair_t filter;
		Q_strncpy( filter.m_szKey, "gamedir", sizeof( filter.m_szKey ) );
		Q_strncpy( filter.m_szValue, "tf2classic", sizeof( filter.m_szKey ) ); // change "tf2classic" to engine->GetGameDirectory() before the release
		m_ServerFilters.AddToTail( filter );
		//Do it only once
		AddRequest( REQUEST_SERVERLIST );
	}
	return true;
}

void CTFNotificationManager::Update( float frametime )
{
	if ( gpGlobals->curtime - m_flLastCheck > tf2c_checkfrequency.GetFloat() )
	{
		m_flLastCheck = gpGlobals->curtime;
		CheckVersionAndMessages();
	}

	if ( !MAINMENU_ROOT->InGame() && gpGlobals->curtime - m_flUpdateLastCheck > tf2c_updatefrequency.GetFloat() )
	{
		m_flUpdateLastCheck = gpGlobals->curtime;
		UpdateServerlistInfo();
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFNotificationManager::CheckVersionAndMessages( void )
{
	AddRequest( REQUEST_VERSION );
	AddRequest( REQUEST_MESSAGE );
	m_bPlayedSound = false;
}

//-----------------------------------------------------------------------------
// Purpose: Event handler
//-----------------------------------------------------------------------------
void CTFNotificationManager::FireGameEvent( IGameEvent *event )
{
}

void CTFNotificationManager::AddRequest( RequestType type )
{
	if ( !m_SteamHTTP )
		return;

	m_httpRequest = m_SteamHTTP->CreateHTTPRequest( k_EHTTPMethodGET, g_aRequestURLs[type] );
	m_SteamHTTP->SetHTTPRequestNetworkActivityTimeout( m_httpRequest, 5 );

	int iReqIndex = m_Requests.Find( m_httpRequest );
	if ( iReqIndex == m_Requests.InvalidIndex() )
	{
		m_Requests.Insert( m_httpRequest, type );
	}
	SteamAPICall_t hSteamAPICall;
	m_SteamHTTP->SendHTTPRequest( m_httpRequest, &hSteamAPICall );

	switch ( type )
	{
	case REQUEST_VERSION:
		m_CallResultVersion.Set( hSteamAPICall, this, ( &CTFNotificationManager::OnHTTPRequestCompleted ) );
		break;
	case REQUEST_MESSAGE:
		m_CallResultMessage.Set( hSteamAPICall, this, ( &CTFNotificationManager::OnHTTPRequestCompleted ) );
		break;
	case REQUEST_SERVERLIST:
		m_CallResultServerlist.Set( hSteamAPICall, this, ( &CTFNotificationManager::OnHTTPRequestCompleted ) );
		break;
	}
	m_bCompleted = false;
}

void CTFNotificationManager::OnHTTPRequestCompleted( HTTPRequestCompleted_t *CallResult, bool iofailure )
{
	DevMsg( "CTFNotificationManager: HTTP Request %i completed: %i\n", CallResult->m_hRequest, CallResult->m_eStatusCode );

	int iReqIndex = m_Requests.Find( CallResult->m_hRequest );
	if ( iReqIndex == m_Requests.InvalidIndex() )
		return;
	RequestType iRequestType = m_Requests[iReqIndex];

	if ( CallResult->m_eStatusCode == 200 )
	{
		uint32 iBodysize;
		m_SteamHTTP->GetHTTPResponseBodySize( CallResult->m_hRequest, &iBodysize );

		uint8 iBodybuffer[256];
		m_SteamHTTP->GetHTTPResponseBodyData( CallResult->m_hRequest, iBodybuffer, iBodysize );

		char result[TF_NOTIFICATION_TITLE_SIZE + TF_NOTIFICATION_MESSAGE_SIZE];
		V_strncpy( result, (char *)iBodybuffer, min( (int)( iBodysize + 1 ), sizeof( result ) ) );

		switch ( iRequestType )
		{
		case REQUEST_IDLE:
			break;
		case REQUEST_VERSION:
			OnVersionCheckCompleted( result );
			break;
		case REQUEST_MESSAGE:
			OnMessageCheckCompleted( result );
			break;
		case REQUEST_SERVERLIST:
			OnServerlistCheckCompleted( result );
			break;
		}
	}

	m_Requests.Remove( CallResult->m_hRequest );
	m_SteamHTTP->ReleaseHTTPRequest( CallResult->m_hRequest );
	m_bCompleted = true;
}

void CTFNotificationManager::OnVersionCheckCompleted( const char* pMessage )
{
	if ( pMessage[0] == '\0' )
		return;

	if ( Q_strcmp( GetVersionString(), pMessage ) < 0 )
	{
		if ( m_bOutdated )
			return;

		m_bOutdated = true;
		wchar_t wszVersion[16];

		g_pVGuiLocalize->ConvertANSIToUnicode( pMessage, wszVersion, sizeof( wszVersion ) );

		MessageNotification Notification;
		V_wcsncpy( Notification.wszTitle, g_pVGuiLocalize->Find( "#TF_GameOutdatedTitle" ), sizeof( Notification.wszTitle ) );
		g_pVGuiLocalize->ConstructString( Notification.wszMessage, sizeof( Notification.wszMessage ), g_pVGuiLocalize->Find( "#TF_GameOutdated" ), 1, wszVersion );

		// Urgent - set time to now.
		Notification.SetTimeStamp( time( NULL ) );

		SendNotification( Notification );
	}
	else
	{
		m_bOutdated = false;
	}
}

void CTFNotificationManager::OnMessageCheckCompleted( const char* pMessage )
{
	if ( pMessage[0] == '\0' )
		return;

	const char *sChar = strchr( pMessage, '\n' );

	if ( !sChar )
	{
		Warning( "Incorrect notification message format.\n" );
		return;
	}

	time_t timePrevious = V_atoi64( tf2c_latest_notification.GetString() );
	time_t timeNew = V_atoi64( TF_NOTIFICATION_TEST_TIME );

	if ( timeNew <= timePrevious ) // Already viewed this one.
		return;

	tf2c_latest_notification.SetValue( TF_NOTIFICATION_TEST_TIME );

	char szTitle[TF_NOTIFICATION_TITLE_SIZE];
	char szMessage[TF_NOTIFICATION_MESSAGE_SIZE];

	V_strncpy( szTitle, pMessage, min( sChar - pMessage + 1, sizeof( szTitle ) ) );
	V_strncpy( szMessage, sChar + 1, sizeof( szMessage ) );
	V_strncpy( m_pzLastMessage, pMessage, sizeof( m_pzLastMessage ) );

	MessageNotification Notification( szTitle, szMessage, timeNew );
	SendNotification( Notification );

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( pLocalPlayer )
	{
		CHudNotificationPanel *pNotifyPanel = GET_HUDELEMENT( CHudNotificationPanel );
		if ( pNotifyPanel )
		{
			pNotifyPanel->SetupNotifyCustom( szMessage, "ico_notify_flag_moving", pLocalPlayer->GetTeamNumber() );
		}
	}
}

void CTFNotificationManager::OnServerlistCheckCompleted( const char* pMessage )
{
	if ( pMessage[0] == '\0' )
		return;

	if ( m_pzLastMessage[0] != '\0' && !Q_strcmp( pMessage, m_pzLastMessage ) )
		return;

	char pzResultString[128];
	char pzMessageString[128];

	char * pch = (char*)pMessage;
	char* pMes = (char*)pMessage;
	int id = 0;
	int offset = 0;

	while ( pch != NULL )
	{
		pMes = pch;
		pch = strchr( (char*)pMes, ' ' );
		if ( !pch ) break;
		id = pch - pMes + 1 - offset;
		Q_snprintf( pzResultString, id, pMes + offset );

		pMes = pch;
		pch = strchr( (char*)pMes, '\n' );
		if ( !pch ) break;
		id = pch - pMes + 1;
		Q_snprintf( pzMessageString, id, pMes );

		offset = 1;

		gameserveritem_t m_Server;
		m_Server.m_NetAdr.Init( atoi( pzResultString ), atoi( pzMessageString ), atoi( pzMessageString ) );
		m_ServerList.AddToTail( m_Server );
	}

	UpdateServerlistInfo();
}

void CTFNotificationManager::UpdateServerlistInfo()
{
	ISteamMatchmakingServers *pMatchmaking = steamapicontext->SteamMatchmakingServers();

	if ( !pMatchmaking || pMatchmaking->IsRefreshing( m_hRequest ) )
		return;

	MatchMakingKeyValuePair_t *pFilters;
	int nFilters = GetServerFilters( &pFilters );
	m_hRequest = pMatchmaking->RequestInternetServerList( engine->GetAppID(), &pFilters, nFilters, this );
}

gameserveritem_t CTFNotificationManager::GetServerInfo( int index )
{
	return m_Servers[index];
};

bool CTFNotificationManager::IsOfficialServer( int index )
{
	for ( int i = 0; i < m_ServerList.Count(); i++ )
	{
		if ( m_ServerList[i].m_NetAdr.GetIP() == m_Servers[index].m_NetAdr.GetIP() &&
			m_ServerList[i].m_NetAdr.GetConnectionPort() == m_Servers[index].m_NetAdr.GetConnectionPort() )
		{
			return true;
		}
	}
	return false;
};

void CTFNotificationManager::ServerResponded( HServerListRequest hRequest, int iServer )
{
	gameserveritem_t *pServerItem = steamapicontext->SteamMatchmakingServers()->GetServerDetails( hRequest, iServer );
	int index = m_Servers.Find( iServer );
	if ( index == m_Servers.InvalidIndex() )
	{
		m_Servers.Insert( iServer, *pServerItem );
		//Msg("%i SERVER %s (%s): PING %i, PLAYERS %i/%i, MAP %s\n", iServer, pServerItem->GetName(), pServerItem->m_NetAdr.GetQueryAddressString(),
		//	pServerItem->m_nPing, pServerItem->m_nPlayers, pServerItem->m_nMaxPlayers, pServerItem->m_szMap);
	}
	else
	{
		m_Servers[index] = *pServerItem;
	}
}

void CTFNotificationManager::RefreshComplete( HServerListRequest hRequest, EMatchMakingServerResponse response )
{
	MAINMENU_ROOT->SetServerlistSize( m_Servers.Count() );
	MAINMENU_ROOT->OnServerInfoUpdate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
uint32 CTFNotificationManager::GetServerFilters( MatchMakingKeyValuePair_t **pFilters )
{
	*pFilters = m_ServerFilters.Base();
	return m_ServerFilters.Count();
}

//-----------------------------------------------------------------------------
// Purpose: Sort notifications by timestamp (latest first)
//-----------------------------------------------------------------------------
int NotificationsSort( const MessageNotification *pItem1, const MessageNotification *pItem2 )
{
	return ( pItem2->timeStamp - pItem1->timeStamp );
}

void CTFNotificationManager::SendNotification( MessageNotification &pMessage )
{
	m_Notifications.AddToTail( pMessage );
	m_Notifications.Sort( NotificationsSort );

	MAINMENU_ROOT->OnNotificationUpdate();

	// Only play sound once per notification.
	if ( !m_bPlayedSound )
	{
		surface()->PlaySound( "ui/notification_alert.wav" );
		m_bPlayedSound = true;
	}
}

void CTFNotificationManager::RemoveNotification( int iIndex )
{
	m_Notifications.Remove( iIndex );
	m_Notifications.Sort( NotificationsSort );

	MAINMENU_ROOT->OnNotificationUpdate();
};

int CTFNotificationManager::GetUnreadNotificationsCount()
{
	int iCount = 0;
	for ( int i = 0; i < m_Notifications.Count(); i++ )
	{
		if ( m_Notifications[i].bUnread )
			iCount++;
	}
	return iCount;
};

char* CTFNotificationManager::GetVersionString()
{
	char verString[30];
	if ( g_pFullFileSystem->FileExists( "version.txt" ) )
	{
		FileHandle_t fh = filesystem->Open( "version.txt", "r", "MOD" );
		int file_len = filesystem->Size( fh );
		char* GameInfo = new char[file_len + 1];

		filesystem->Read( (void*)GameInfo, file_len, fh );
		GameInfo[file_len] = 0; // null terminator

		filesystem->Close( fh );

		Q_snprintf( verString, sizeof( verString ), GameInfo + 8 );

		delete[] GameInfo;
	}

	char *szResult = (char*)malloc( sizeof( verString ) );
	Q_strncpy( szResult, verString, sizeof( verString ) );
	return szResult;
}