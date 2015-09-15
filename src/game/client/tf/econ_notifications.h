#ifndef ECON_NOTIFICATIONS_H
#define ECON_NOTIFICATIONS_H

#ifdef _WIN32
#pragma once
#endif

#include "steam/steam_api.h"

//-----------------------------------------------------------------------------
// Purpose: Dull notification class. Added to make CTFVoteNotification work from hud_vote.cpp
//-----------------------------------------------------------------------------
class CEconNotification
{
public:
	CEconNotification();
	~CEconNotification();

	virtual void	SetText(const char *text);
	virtual void	AddStringToken(const char *a2, const wchar_t *a3);
	virtual void	SetKeyValues(KeyValues *a2);
	virtual void	SetLifetime(float a1, float a3 = 0.0f);
	virtual float	GetExpireTime();
	virtual float	GetInGameLifeTime();
	virtual void	SetIsInUse(bool a2);
	virtual void	SetSteamID(const CSteamID *a2);
	virtual void	MarkForDeletion();
	virtual bool	CanBeTriggered();
	virtual void	Trigger();
	virtual bool	CanBeAcceptedOrDeclined();
	virtual void	Accept();
	virtual void	Decline();

private:
	char		sText[64];	//1
							//2
	float		fLifetime;	//3
	KeyValues	*bStrings;	//4
	bool		bInUse;		//4128
	CSteamID	pSteamID;	//4116
};

static int NotificationQueue_Add(CEconNotification *a1)
{
	DevMsg("Adding notification\n");
	return 0;
}

#endif // ECON_NOTIFICATIONS_H
