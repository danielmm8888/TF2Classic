#ifndef TF_VOTEISSUES_H
#define TF_VOTEISSUES_H
#ifdef _WIN32
#pragma once
#endif

#include "vote_controller.h"

class CKickIssue : public CBaseIssue
{
public:
	CKickIssue(const char *typeString);
	~CKickIssue();
	const char *GetDisplayString();
	const char * GetVotePassedString();
	void ListIssueDetails(CBasePlayer *a2);
	bool IsEnabled();
	const char * GetDetailsString();
	void OnVoteStarted();
	void Init();
	void NotifyGC(bool a2);
	int PrintLogData();
	void OnVoteFailed(int a2);
	bool CreateVoteDataFromDetails(const char *s);
	int CanCallVote(int a1, char *s, int a2, int a3); // idb
	void ExecuteCommand();
	bool IsTeamRestrictedVote();

private:
	char		 m_pzPlayerName[64];
	char		 m_pzReason[64];
	int			 m_iPlayerID;
};


#endif // TF_INVENTORY_H
