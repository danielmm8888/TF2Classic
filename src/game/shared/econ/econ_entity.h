//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#ifndef ECON_ENTITY_H
#define ECON_ENTITY_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"

#if defined( CLIENT_DLL )
#define CEconEntity C_EconEntity
#endif

#include "econ_itemview.h"

struct wearableanimplayback_t
{
	int iStub;
};

class EconItemDefinition;

//-----------------------------------------------------------------------------
// Purpose: BaseCombatWeapon is derived from this in live tf2.
//			The only actual use for it I've found so far is for the c_model
//			activity translation. Need to do some more research into this.
//-----------------------------------------------------------------------------
class CEconEntity : public CBaseAnimating
{
	DECLARE_CLASS( CEconEntity, CBaseAnimating );
	DECLARE_NETWORKCLASS();

public:
	CEconEntity();
	~CEconEntity();

	virtual int TranslateViewmodelHandActivity( int iActivity ) { return iActivity; }

	virtual void PlayAnimForPlaybackEvent(wearableanimplayback_t iPlayback) {};

	virtual void SetItemID(int id);
	virtual int GetItemID();
	virtual bool HasItemDefinition() const;

protected:
	CNetworkVarEmbedded(CEconItemView, m_Item);
};

#endif