#include "cbase.h"
#include "econ_notifications.h"

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CEconNotification::CEconNotification()
{
	bInUse = false;
	fLifetime = -1.0f;
	bStrings = NULL;
}

CEconNotification::~CEconNotification()
{
}

void CEconNotification::SetText(const char *a2)
{
	Q_snprintf(sText, sizeof(sText), a2);
}

void CEconNotification::AddStringToken(const char *a2, const wchar_t *a3)
{
	if (!bStrings)
	{
		bStrings = new KeyValues("CEconNotification");
	}
	bStrings->SetWString(a2, a3);
}

void CEconNotification::SetKeyValues(KeyValues *a2)
{
	if (bStrings)
	{
		bStrings->deleteThis();
	}
	bStrings->MakeCopy(a2);
}

void CEconNotification::SetLifetime(float a1, float a3/* = 0.0f*/)
{
	fLifetime = a1 + a3;
}

float CEconNotification::GetExpireTime()
{
	return fLifetime;
}

float CEconNotification::GetInGameLifeTime()
{
	return fLifetime;
}

void CEconNotification::SetIsInUse(bool a2)
{
	bInUse = a2;
}

void CEconNotification::SetSteamID(const CSteamID *a2)
{
	pSteamID = *a2;
}

void CEconNotification::MarkForDeletion()
{
	fLifetime = 0;
}

bool CEconNotification::CanBeTriggered()
{
	return false;
}

void CEconNotification::Trigger()
{
	;
}

bool CEconNotification::CanBeAcceptedOrDeclined()
{
	return false;
}

void CEconNotification::Accept()
{
	;
}

void CEconNotification::Decline()
{
	;
}
