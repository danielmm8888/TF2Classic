#include "cbase.h"
#include "tf_shareddefs.h"
#include "tf_voteissues.h"

extern ConVar sv_vote_timer_duration;

CKickIssue::CKickIssue(const char *typeString) : CBaseIssue(typeString)
{
	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), "");
	Q_snprintf(m_pzReason, sizeof(m_pzReason), "");
	m_iPlayerID = 0;
}

CKickIssue::~CKickIssue()
{
}

const char *CKickIssue::GetDisplayString()
{
	char result[64];
	Q_snprintf(result, sizeof(result), "#TF_vote_kick_player_%s", m_pzReason);
	char *szResult = (char*)malloc(sizeof(result));
	Q_strncpy(szResult, result, sizeof(result));
	return szResult;
}

const char *CKickIssue::GetVotePassedString()
{
	return "#TF_vote_passed_kick_player";
	//TODO: add something for "#TF_vote_passed_ban_player";
}

void CKickIssue::ListIssueDetails(CBasePlayer *a2)
{
	char s[64];
	if (true)//There should be check or something
	{
		V_snprintf(s, sizeof(s), "callvote %s <userID>\n", GetTypeString());
		ClientPrint(a2, 2, s);
	}
}

bool CKickIssue::IsEnabled()
{
	//Also some check
	return true;
}

const char * CKickIssue::GetDetailsString()
{
	if (m_iPlayerID > 0 && UTIL_PlayerByIndex(m_iPlayerID - 1))
	{
		return m_pzPlayerName;
	}
	else
	{
		return "Unnamed";
	}
}

void CKickIssue::OnVoteStarted()
{
	const char *pDetails = CBaseIssue::GetDetailsString();
	const char *pch;
	pch = strrchr(pDetails, ' ');
	if (!pch)
		return;

	int i = pch - pDetails + 1;
	Q_snprintf(m_pzReason, sizeof(m_pzReason), pDetails + i);

	char m_PlayerID[64];
	Q_snprintf(m_PlayerID, i, pDetails);

	m_iPlayerID = atoi(m_PlayerID);
	CBasePlayer *pVoteCaller = UTIL_PlayerByIndex(m_iPlayerID - 1);
	if (!pVoteCaller)
		return;

	Q_snprintf(m_pzPlayerName, sizeof(m_pzPlayerName), pVoteCaller->GetPlayerName());

	g_voteController->TryCastVote(pVoteCaller->entindex(), "Option2");
	CSteamID pSteamID;
	pVoteCaller->GetSteamID(&pSteamID);
	g_voteController->AddPlayerToNameLockedList(pSteamID, sv_vote_timer_duration.GetFloat(), m_iPlayerID);
}

void CKickIssue::Init()
{

}

void CKickIssue::NotifyGC(bool a2)
{
	return;
}

int CKickIssue::PrintLogData()
{
	return 0;
}

void CKickIssue::OnVoteFailed(int a2)
{
	CBaseIssue::OnVoteFailed(a2);
	PrintLogData();
}

bool CKickIssue::CreateVoteDataFromDetails(const char *s)
{
	return 0;
}

int CKickIssue::CanCallVote(int a1, char *s, int a2, int a3)
{
	return 0;
}

void CKickIssue::ExecuteCommand()
{
	;
}

bool CKickIssue::IsTeamRestrictedVote()
{
	return 1;
}

