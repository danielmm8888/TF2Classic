#ifndef TF_ECON_ITEMVIEW_H
#define TF_ECON_ITEMVIEW_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"
#include "igamesystem.h"
#include "GameEventListener.h"
#include "networkvar.h"

#include "econ_entity.h"

//#if defined( CLIENT_DLL )
//class C_EconEntity;
//#define CEconEntity C_EconEntity
//#else
//class CEconEntity;
//#endif

class CEconItemHandle
{
public:
	// CEconItem *m_pItem; TODO: not implemented yet, get a GC going

	uint64 m_SteamID; // The owner of the item?
	int m_ItemID;
};


class CEconItemView
{
	DECLARE_CLASS_NOBASE(CEconItemView);
	DECLARE_EMBEDDED_NETWORKVAR();
public:
	CEconItemView() {}
	CEconItemView( int m_ItemID ){ SetItemDefIndex(m_ItemID); }
	static const char* GetWorldDisplayModel(CEconEntity *pEntity, int iClass = 0);
	static const char* GetWorldDisplayModel(int ID, int iClass = 0);
	static const char* GetPlayerDisplayModel(CEconEntity *pEntity);
	static const char* GetPlayerDisplayModel(int ID);
	static const char* GetEntityName( int ID );
	static bool IsCosmetic(CEconEntity *pEntity);
	static bool IsCosmetic(int ID);
	static int GetAnimationSlot(CEconEntity *pEntity);
	static int GetAnimationSlot(int ID);
	static const char* GetAnimationReplacement(CEconEntity *pEntity, const char* name);
	static const char* GetAnimationReplacement(int ID, const char* name);
	static const char* GetSoundOverride(CEconEntity *pEntity, const char* name);
	static const char* GetSoundOverride(int ID, const char* name);
	static bool HasCapability(CEconEntity *pEntity, const char* name);
	static bool HasCapability(int ID, const char* name);
	static bool HasTag(CEconEntity *pEntity, const char* name);
	static bool HasTag(int ID, const char* name);
	static bool HasBodygroupOverride(CEconEntity *pEntity, const char* name);
	static bool HasBodygroupOverride(int ID, const char* name);

	void SetItemDefIndex(int iItemID) { m_iItemDefinitionIndex = iItemID; }
	int GetItemDefIndex(void) const { return m_iItemDefinitionIndex; }

protected:

	CNetworkVar(short, m_iItemDefinitionIndex);

	CNetworkVar(int, m_iEntityQuality); // maybe an enum?
	CNetworkVar(int, m_iEntityLevel);

	CNetworkVar(uint64, m_iItemID);
	CNetworkVar(uint64, m_iAccountID);
	CNetworkVar(int, m_iInventoryPosition);

	CEconItemHandle m_ItemHandle; // The handle to the CEconItem on the GC

	CNetworkVar(int, m_iTeamNumber);
	//bool m_bInitialized; // ?

	// CAttributeList m_AttributeList; TODO: Add dynamic attribs
	CNetworkVar(bool, m_bOnlyIterateItemViewAttributes);
};

#endif // TF_ECON_ITEMVIEW_H