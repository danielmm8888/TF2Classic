//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: Client side CTFTeam class
//
// $NoKeywords: $
//=============================================================================

#ifndef C_TF_TEAM_H
#define C_TF_TEAM_H
#ifdef _WIN32
#pragma once
#endif

#include "c_team.h"
#include "shareddefs.h"
#include "c_baseobject.h"
#include "c_ai_basenpc.h"

class C_BaseEntity;
class C_BaseObject;
class CBaseTechnology;
class C_AI_BaseNPC;

//-----------------------------------------------------------------------------
// Purpose: TF's Team manager
//-----------------------------------------------------------------------------
class C_TFTeam : public C_Team
{
	DECLARE_CLASS( C_TFTeam, C_Team );
	DECLARE_CLIENTCLASS();

public:

					C_TFTeam();
	virtual			~C_TFTeam();

	int				GetFlagCaptures( void ) { return m_nFlagCaptures; }
	int				GetRole( void ) { return m_iRole; }
	char			*Get_Name( void );

	int				GetNumObjects( int iObjectType = -1 );
	CBaseObject		*GetObject( int num );

	int				GetNumNPCs( void );
	C_AI_BaseNPC	*GetNPC( int num );

	CUtlVector< CHandle<C_BaseObject> > m_aObjects;
	CUtlVector< CHandle<C_AI_BaseNPC> > m_aNPCs;

private:

	int		m_nFlagCaptures;
	int		m_iRole;
};

C_TFTeam *GetGlobalTFTeam( int iTeamNumber );

#endif // C_TF_TEAM_H
