#ifndef TF_NOTIFICATIONMANAGER_H
#define TF_NOTIFICATIONMANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"
#include "igamesystem.h"
#include "GameEventListener.h"
#include "steam/steam_api.h"
#include "steam/isteamhttp.h"
#include "time.h"

#define TF_NOTIFICATION_TITLE_SIZE 64
#define TF_NOTIFICATION_MESSAGE_SIZE 256

class CTFNotificationManager;

enum RequestType
{
	REQUEST_IDLE = -1,
	REQUEST_VERSION = 0,
	REQUEST_MESSAGE,
	REQUEST_SERVERLIST,

	REQUEST_COUNT
};

struct MessageNotification
{
	MessageNotification();
	MessageNotification( const char *Title, const char *Message, time_t timeVal );
	MessageNotification( const wchar_t *Title, const wchar_t *Message, time_t timeVal );

	void SetTimeStamp( time_t timeVal );

	time_t timeStamp;
	wchar_t wszTitle[TF_NOTIFICATION_TITLE_SIZE];
	wchar_t wszDate[32];
	wchar_t wszMessage[TF_NOTIFICATION_MESSAGE_SIZE];
	bool bUnread;
	bool bLocal;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFNotificationManager : public CAutoGameSystemPerFrame, public CGameEventListener, public ISteamMatchmakingServerListResponse
{
public:
	CTFNotificationManager();
	~CTFNotificationManager();

	// Methods of IGameSystem
	virtual bool Init();
	virtual char const *Name() { return "CTFNotificationManager"; }
	// Gets called each frame
	virtual void Update( float frametime );

	void CheckVersionAndMessages( void );

	// Methods of CGameEventListener
	virtual void FireGameEvent( IGameEvent *event );

	virtual void AddRequest( RequestType type );
	virtual void SendNotification( MessageNotification &pMessage );
	virtual MessageNotification *GetNotification( int iIndex ) { return &m_Notifications[iIndex]; };
	virtual int GetNotificationsCount() { return m_Notifications.Count(); };
	virtual int GetUnreadNotificationsCount();
	virtual void RemoveNotification( int iIndex );
	virtual bool IsOutdated() { return m_bOutdated; };
	const char *GetVersionName( void );
	time_t GetVersionTimeStamp( void );

	uint32 GetServerFilters( MatchMakingKeyValuePair_t **pFilters );
	// Server has responded ok with updated data
	virtual void ServerResponded( HServerListRequest hRequest, int iServer );
	// Server has failed to respond
	virtual void ServerFailedToRespond( HServerListRequest hRequest, int iServer ) {}
	// A list refresh you had initiated is now 100% completed
	virtual void RefreshComplete( HServerListRequest hRequest, EMatchMakingServerResponse response );

	void UpdateServerlistInfo();
	gameserveritem_t GetServerInfo( int index );
	bool IsOfficialServer( int index );

private:
	bool		m_bInited;
	CUtlVector<MessageNotification>	m_Notifications;
	CUtlMap<time_t, MessageNotification> m_NotificationsMap;

	ISteamHTTP*			m_SteamHTTP;
	HTTPRequestHandle	m_httpRequest;

	bool				m_bOutdated;
	bool				m_bCompleted;
	bool				m_bPlayedSound;

	CUtlMap<HTTPRequestHandle, RequestType> m_Requests;
	RequestType			m_iCurrentRequest;
	float				m_flLastCheck;
	float				m_flUpdateLastCheck;
	char				m_pzLastMessage[128];

	void				OnMessageCheckCompleted( const char* pMessage );
	void				OnVersionCheckCompleted( const char* pMessage );
	void				OnServerlistCheckCompleted( const char* pMessage );

	CCallResult<CTFNotificationManager, HTTPRequestCompleted_t> m_CallResultVersion;
	CCallResult<CTFNotificationManager, HTTPRequestCompleted_t> m_CallResultMessage;
	CCallResult<CTFNotificationManager, HTTPRequestCompleted_t> m_CallResultServerlist;

	void				OnHTTPRequestCompleted( HTTPRequestCompleted_t *CallResult, bool iofailure );

	HServerListRequest m_hRequest;
	CUtlVector<gameserveritem_t> m_ServerList;
	CUtlMap<int, gameserveritem_t> m_Servers;
	CUtlVector<MatchMakingKeyValuePair_t> m_ServerFilters;
};

CTFNotificationManager *GetNotificationManager();
#endif // TF_NOTIFICATIONMANAGER_H
