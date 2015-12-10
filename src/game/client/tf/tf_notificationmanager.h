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

class CTFNotificationManager;

enum RequestType
{
	REQUEST_IDLE = -1,
	REQUEST_VERSION = 0,
	REQUEST_MESSAGE,

	REQUEST_COUNT
};

struct MessageNotification
{
	char sTitle[64];
	char sMessage[128];
	bool bUnread;
	MessageNotification() { bUnread = true; };
	MessageNotification(char* Title, char* Message)
	{
		Q_snprintf(sTitle, sizeof(sTitle), Title);
		Q_snprintf(sMessage, sizeof(sMessage), Message);
		bUnread = true;
	};
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFNotificationManager : public CAutoGameSystemPerFrame, public CGameEventListener
{
public:
	CTFNotificationManager();
	~CTFNotificationManager();

	// Methods of IGameSystem
	virtual bool Init();
	virtual char const *Name() { return "CTFNotificationManager"; }
	// Gets called each frame
	virtual void Update(float frametime);

	// Methods of CGameEventListener
	virtual void FireGameEvent(IGameEvent *event);

	virtual void AddRequest(RequestType type);
	virtual void SendNotification(MessageNotification pMessage);
	virtual MessageNotification *GetNotification(int iIndex) { return &pNotifications[iIndex]; };
	virtual int GetNotificationsCount() { return pNotifications.Count(); };
	virtual int GetUnreadNotificationsCount();
	virtual void RemoveNotification(int iIndex);
	virtual bool IsOutdated() { return bOutdated; };
	virtual char*GetVersionString();

private:
	bool		m_bInited;
	CUtlVector<MessageNotification>	pNotifications;

	ISteamHTTP*			m_SteamHTTP;
	HTTPRequestHandle	m_httpRequest;

	bool				bOutdated;
	bool				bCompleted;

	CUtlMap<HTTPRequestHandle, RequestType>m_Requests;
	RequestType			iCurrentRequest;
	float				fLastCheck;
	char				m_pzLastMessage[128];
	void				OnMessageCheckCompleted(const char* pMessage);
	void				OnVersionCheckCompleted(const char* pMessage);
	CCallResult<CTFNotificationManager, HTTPRequestCompleted_t> m_CallResultVersion;
	CCallResult<CTFNotificationManager, HTTPRequestCompleted_t> m_CallResultMessage;
	void				OnHTTPRequestCompleted(HTTPRequestCompleted_t *CallResult, bool iofailure);
};

CTFNotificationManager *GetNotificationManager();
#endif // TF_NOTIFICATIONMANAGER_H
