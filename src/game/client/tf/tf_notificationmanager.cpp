#include "cbase.h"
#include "tf_notificationmanager.h"
#include "tf_mainmenu.h"
#include "filesystem.h"
#include "script_parser.h"
#include "tf_gamerules.h"
#include "tf_hud_notification_panel.h"
//#include "public\steam\matchmakingtypes.h"

const char *g_aRequestURLs[REQUEST_COUNT] =
{
	"http://services.0x13.io/tf2c/version/?latest=1",
	"http://services.0x13.io/tf2c/motd/",
	"http://services.0x13.io/tf2c/servers/official/"
};

static CTFNotificationManager g_TFNotificationManager;
CTFNotificationManager *GetNotificationManager()
{
	return &g_TFNotificationManager;
}

CON_COMMAND_F(tf2c_checkmessages, "Check for the messages", FCVAR_DEVELOPMENTONLY)
{
	GetNotificationManager()->AddRequest(REQUEST_VERSION);
	GetNotificationManager()->AddRequest(REQUEST_MESSAGE);
}

CON_COMMAND_F(tf2c_updateserverlist, "Check for the messages", FCVAR_DEVELOPMENTONLY)
{
	GetNotificationManager()->UpdateServerlistInfo();
}

ConVar tf2c_checkfrequency("tf2c_checkfrequency", "900", FCVAR_DEVELOPMENTONLY, "Messages check frequency (seconds)");
ConVar tf2c_updatefrequency("tf2c_updatefrequency", "15", FCVAR_DEVELOPMENTONLY, "Updatelist update frequency (seconds)");

bool RequestHandleLessFunc(const HTTPRequestHandle &lhs, const HTTPRequestHandle &rhs)
{
	return lhs < rhs;
}

bool ServerLessFunc(const int &lhs, const int &rhs)
{
	return lhs < rhs;
}

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CTFNotificationManager::CTFNotificationManager() : CAutoGameSystemPerFrame("CTFNotificationManager")
{
	if (!filesystem)
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
	if (!m_bInited)
	{
		pNotifications.RemoveAll();

		m_SteamHTTP = steamapicontext->SteamHTTP();
		m_Requests.SetLessFunc(RequestHandleLessFunc);
		m_mapServers.SetLessFunc(ServerLessFunc);
		fLastCheck = tf2c_checkfrequency.GetFloat() * -1;
		fUpdateLastCheck = tf2c_updatefrequency.GetFloat() * -1;
		iCurrentRequest = REQUEST_IDLE;
		bCompleted = false;
		bOutdated = false;
		m_bInited = true;

		hRequest = 0;
		MatchMakingKeyValuePair_t filter;
		Q_strncpy(filter.m_szKey, "gamedir", sizeof(filter.m_szKey));
		Q_strncpy(filter.m_szValue, "tf2classic", sizeof(filter.m_szKey)); // change "tf2classic" to engine->GetGameDirectory() before the release
		m_vecServerFilters.AddToTail(filter);
		//Do it only once
		AddRequest(REQUEST_SERVERLIST);
	}
	return true;
}

void CTFNotificationManager::Update(float frametime)
{
	if (gpGlobals->curtime - fLastCheck > tf2c_checkfrequency.GetFloat())
	{
		fLastCheck = gpGlobals->curtime;
		AddRequest(REQUEST_VERSION);
		AddRequest(REQUEST_MESSAGE);
	}

	if (!MAINMENU_ROOT->InGame() && gpGlobals->curtime - fUpdateLastCheck > tf2c_updatefrequency.GetFloat())
	{
		fUpdateLastCheck = gpGlobals->curtime;
		UpdateServerlistInfo();
	}	
}

//-----------------------------------------------------------------------------
// Purpose: Event handler
//-----------------------------------------------------------------------------
void CTFNotificationManager::FireGameEvent(IGameEvent *event)
{
}

void CTFNotificationManager::AddRequest(RequestType type)
{
	if (!m_SteamHTTP)
		return;

	m_httpRequest = m_SteamHTTP->CreateHTTPRequest(k_EHTTPMethodGET, g_aRequestURLs[type]);
	m_SteamHTTP->SetHTTPRequestNetworkActivityTimeout(m_httpRequest, 5);

	int iReqIndex = m_Requests.Find(m_httpRequest);
	if (iReqIndex == m_Requests.InvalidIndex())
	{
		m_Requests.Insert(m_httpRequest, type);
	}
	SteamAPICall_t hSteamAPICall;
	m_SteamHTTP->SendHTTPRequest(m_httpRequest, &hSteamAPICall);

	switch (type)
	{
	case REQUEST_VERSION:
		m_CallResultVersion.Set(hSteamAPICall, this, (&CTFNotificationManager::OnHTTPRequestCompleted));
		break;
	case REQUEST_MESSAGE:
		m_CallResultMessage.Set(hSteamAPICall, this, (&CTFNotificationManager::OnHTTPRequestCompleted));
		break;
	case REQUEST_SERVERLIST:
		m_CallResultServerlist.Set(hSteamAPICall, this, (&CTFNotificationManager::OnHTTPRequestCompleted));
		break;
	}
	bCompleted = false;
}

void CTFNotificationManager::OnHTTPRequestCompleted(HTTPRequestCompleted_t *CallResult, bool iofailure)
{
	DevMsg("CTFNotificationManager: HTTP Request %i completed: %i\n", CallResult->m_hRequest, CallResult->m_eStatusCode);

	int iReqIndex = m_Requests.Find(CallResult->m_hRequest);
	if (iReqIndex == m_Requests.InvalidIndex())
		return;
	RequestType iRequestType = m_Requests[iReqIndex];

	if (CallResult->m_eStatusCode == 200)
	{
		char result[256];
		uint32 iBodysize;
		m_SteamHTTP->GetHTTPResponseBodySize(CallResult->m_hRequest, &iBodysize);
		uint8 iBodybuffer[256];
		int size = (iBodysize > sizeof(result) ? sizeof(result) : iBodysize);	//cap the buffer size
		m_SteamHTTP->GetHTTPResponseBodyData(CallResult->m_hRequest, iBodybuffer, size);
		Q_strncpy(result, (char*)iBodybuffer, size);

		switch (iRequestType)
		{
		case REQUEST_IDLE:
			break;
		case REQUEST_VERSION:
			OnVersionCheckCompleted(result);
			break;
		case REQUEST_MESSAGE:
			OnMessageCheckCompleted(result);
			break;
		case REQUEST_SERVERLIST:
			OnServerlistCheckCompleted(result);
			break;
		}
	}

	m_Requests.Remove(CallResult->m_hRequest);
	m_SteamHTTP->ReleaseHTTPRequest(CallResult->m_hRequest);
	bCompleted = true;
}

void CTFNotificationManager::OnVersionCheckCompleted(const char* pMessage)
{
	if (pMessage[0] == '\0')
		return;

	if (Q_strcmp(GetVersionString(), pMessage) < 0)
	{
		char resultString[128];
		bOutdated = true;
		Q_snprintf(resultString, sizeof(resultString), "Your game is out of date.\nThe newest version of TF2C is %s.\nDownload the update at\nwww.tf2classic.com", pMessage);
		MessageNotification Notification("Update!", resultString);
		SendNotification(Notification);
	}
	else
	{
		bOutdated = false;
	}
}

void CTFNotificationManager::OnMessageCheckCompleted(const char* pMessage)
{		
	if (pMessage[0] == '\0')
		return;

	if (m_pzLastMessage[0] != '\0' && !Q_strcmp(pMessage, m_pzLastMessage))
		return;

	char pzResultString[128];
	char pzMessageString[128];

	char * pch;
	int id = 0;
	pch = strchr((char*)pMessage, '\n');
	if (pch != NULL)
	{
		id = pch - pMessage + 1;
	}
	Q_snprintf(pzResultString, id, "%s", pMessage);
	Q_snprintf(pzMessageString, sizeof(pzMessageString), pMessage + id);
	Q_snprintf(m_pzLastMessage, sizeof(m_pzLastMessage), pMessage);

	MessageNotification Notification(pzResultString, pzMessageString);
	SendNotification(Notification);

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pLocalPlayer)
	{
		CHudNotificationPanel *pNotifyPanel = GET_HUDELEMENT(CHudNotificationPanel);
		if (pNotifyPanel)
		{
			pNotifyPanel->SetupNotifyCustom(Notification.sMessage, "ico_notify_flag_moving", C_TFPlayer::GetLocalTFPlayer()->GetTeamNumber());
		}
	}
}

void CTFNotificationManager::OnServerlistCheckCompleted(const char* pMessage)
{
	if (pMessage[0] == '\0')
		return;

	if (m_pzLastMessage[0] != '\0' && !Q_strcmp(pMessage, m_pzLastMessage))
		return;

	char pzResultString[128];
	char pzMessageString[128];

	char * pch = (char*)pMessage;
	char* pMes = (char*)pMessage;
	int id = 0;
	int offset = 0;
	
	while (pch != NULL)
	{
		pMes = pch;
		pch = strchr((char*)pMes, ' ');
		if (!pch) break;
		id = pch - pMes + 1 - offset;
		Q_snprintf(pzResultString, id, pMes + offset);

		pMes = pch;
		pch = strchr((char*)pMes, '\n');
		if (!pch) break;
		id = pch - pMes + 1;
		Q_snprintf(pzMessageString, id, pMes);

		offset = 1;

		gameserveritem_t m_Server;
		m_Server.m_NetAdr.Init(atoi(pzResultString), atoi(pzMessageString), atoi(pzMessageString));
		m_ServerList.AddToTail(m_Server);
	}

	UpdateServerlistInfo();
}

void CTFNotificationManager::UpdateServerlistInfo()
{	
	ISteamMatchmakingServers *pMatchmaking = steamapicontext->SteamMatchmakingServers();

	if ( !pMatchmaking || pMatchmaking->IsRefreshing( hRequest ) )
		return;

	MatchMakingKeyValuePair_t *pFilters;
	int nFilters = GetServerFilters(&pFilters);
	hRequest = pMatchmaking->RequestInternetServerList( engine->GetAppID(), &pFilters, nFilters, this );
}

gameserveritem_t CTFNotificationManager::GetServerInfo(int index) 
{ 
	return m_mapServers[index];
};

bool CTFNotificationManager::IsOfficialServer(int index)
{
	for (int i = 0; i < m_ServerList.Count(); i++)
	{
		if (m_ServerList[i].m_NetAdr.GetIP() == m_mapServers[index].m_NetAdr.GetIP() &&
			m_ServerList[i].m_NetAdr.GetConnectionPort() == m_mapServers[index].m_NetAdr.GetConnectionPort())
		{
			return true;
		}
	}
	return false;
};

void CTFNotificationManager::ServerResponded(HServerListRequest hRequest, int iServer)
{
	gameserveritem_t *pServerItem = steamapicontext->SteamMatchmakingServers()->GetServerDetails(hRequest, iServer);
	int index = m_mapServers.Find(iServer);
	if (index == m_mapServers.InvalidIndex())
	{
		m_mapServers.Insert(iServer, *pServerItem);
		//Msg("%i SERVER %s (%s): PING %i, PLAYERS %i/%i, MAP %s\n", iServer, pServerItem->GetName(), pServerItem->m_NetAdr.GetQueryAddressString(),
		//	pServerItem->m_nPing, pServerItem->m_nPlayers, pServerItem->m_nMaxPlayers, pServerItem->m_szMap);
	}
	else
	{
		m_mapServers[index] = *pServerItem;
	}
}

void CTFNotificationManager::RefreshComplete(HServerListRequest hRequest, EMatchMakingServerResponse response)
{
	MAINMENU_ROOT->SetServerlistSize(m_mapServers.Count());	
	MAINMENU_ROOT->OnServerInfoUpdate();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
uint32 CTFNotificationManager::GetServerFilters(MatchMakingKeyValuePair_t **pFilters)
{
	*pFilters = m_vecServerFilters.Base();
	return m_vecServerFilters.Count();
}

void CTFNotificationManager::SendNotification(MessageNotification pMessage)
{
	pNotifications.AddToTail(pMessage);
	MAINMENU_ROOT->OnNotificationUpdate();
}

void CTFNotificationManager::RemoveNotification(int iIndex)
{
	pNotifications.Remove(iIndex);
	MAINMENU_ROOT->OnNotificationUpdate();
};

int CTFNotificationManager::GetUnreadNotificationsCount()
{
	int iCount = 0;
	for (int i = 0; i < pNotifications.Count(); i++)
	{
		if (pNotifications[i].bUnread)
			iCount++;
	}
	return iCount;
};

char* CTFNotificationManager::GetVersionString()
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

		Q_snprintf(verString, sizeof(verString), GameInfo + 8);

		delete[] GameInfo;
	}

	char *szResult = (char*)malloc(sizeof(verString));
	Q_strncpy(szResult, verString, sizeof(verString));
	return szResult;
}