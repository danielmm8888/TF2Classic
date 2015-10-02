#include "cbase.h"
#include "econ_itemview.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


#ifdef TF_CLASSIC_CLIENT
BEGIN_RECV_TABLE_NOBASE(CEconItemView, DT_ScriptCreatedItem)
	RecvPropInt(RECVINFO(m_iItemDefinitionIndex)),
	RecvPropInt(RECVINFO(m_iEntityQuality)),
	RecvPropInt(RECVINFO(m_iEntityLevel)),
	//RecvPropInt(RECVINFO(m_iItemID)),
	RecvPropInt(RECVINFO(m_iInventoryPosition)),
	RecvPropInt(RECVINFO(m_iTeamNumber)),
	RecvPropInt(RECVINFO(m_bOnlyIterateItemViewAttributes)),
END_RECV_TABLE()
#else
BEGIN_SEND_TABLE_NOBASE(CEconItemView, DT_ScriptCreatedItem)
	SendPropInt(SENDINFO(m_iItemDefinitionIndex)),
	SendPropInt(SENDINFO(m_iEntityQuality)),
	SendPropInt(SENDINFO(m_iEntityLevel)),
	//SendPropInt(SENDINFO(m_iItemID)),
	SendPropInt(SENDINFO(m_iInventoryPosition)),
	SendPropInt(SENDINFO(m_iTeamNumber)),
	SendPropBool(SENDINFO(m_bOnlyIterateItemViewAttributes)),
END_SEND_TABLE()
#endif

BEGIN_NETWORK_TABLE(CEconEntity, DT_EconEntity)
#ifdef TF_CLASSIC_CLIENT
RecvPropDataTable(RECVINFO_DT(m_Item), 0, &REFERENCE_RECV_TABLE(DT_ScriptCreatedItem)),
#else
SendPropDataTable(SENDINFO_DT(m_Item),
&REFERENCE_SEND_TABLE(DT_ScriptCreatedItem)),
#endif
END_NETWORK_TABLE()

#define FIND_ELEMENT(dict, str, val)					\
		unsigned int index = dict.Find(str);			\
		if (index < dict.Count())						\
			val = dict.Element(index)	

#define FIND_ELEMENT_STRING(dict, str, val)				\
		unsigned int index = dict.Find(str);			\
		if (index < dict.Count())						\
			Q_snprintf(val, sizeof(val), dict.Element(index))

const char* CEconItemView::GetWorldDisplayModel(CEconEntity *pEntity, int iClass/* = 0*/)
{
	return GetWorldDisplayModel(pEntity->GetItemDefIndex());
}

const char* CEconItemView::GetWorldDisplayModel(int ID, int iClass/* = 0*/)
{
	char modelname[128];
	if (iClass > 0)
	{
		FIND_ELEMENT_STRING(GetItemSchema()->GetItemDefinition(ID)->model_player_per_class, g_aPlayerClassNames_NonLocalized[iClass], modelname);
	}
	if (!Q_stricmp(modelname, ""))
	{
		Q_strncpy(modelname, GetItemSchema()->GetItemDefinition(ID)->model_world, sizeof(modelname));
		if (!Q_stricmp(modelname, ""))
		{
			Q_strncpy(modelname, GetItemSchema()->GetItemDefinition(ID)->model_player, sizeof(modelname));
		}
	}
	char *result = (char*)malloc(sizeof(modelname));
	Q_strncpy(result, modelname, sizeof(modelname));
	return result;
}

const char* CEconItemView::GetPlayerDisplayModel(CEconEntity *pEntity)
{
	return GetPlayerDisplayModel(pEntity->GetItemDefIndex());
}

const char* CEconItemView::GetPlayerDisplayModel(int ID)
{
	return GetItemSchema()->GetItemDefinition(ID)->model_player;
}

const char* CEconItemView::GetEntityName( int ID )
{
	char name[64];
	Q_strncpy( name, GetItemSchema()->GetItemDefinition( ID )->item_class, sizeof( name ) );
	char *result = (char*)malloc(sizeof(name));
	Q_strncpy(result, name, sizeof(name));
	return result;
}

bool CEconItemView::IsCosmetic(CEconEntity *pEntity)
{
	return IsCosmetic(pEntity->GetItemDefIndex());
}

bool CEconItemView::IsCosmetic(int ID)
{
	bool result = false;
	FIND_ELEMENT(GetItemSchema()->GetItemDefinition(ID)->tags, "is_cosmetic", result);
	return result;
}

const char* CEconItemView::GetAnimationReplacement(CEconEntity *pEntity, const char* name)
{
	return GetAnimationReplacement(pEntity->GetItemDefIndex(), name);
}

const char* CEconItemView::GetAnimationReplacement(int ID, const char* name)
{
	char str[64];
	FIND_ELEMENT_STRING(GetItemSchema()->GetItemDefinition(ID)->visual.animation_replacement, name, str);
	char *result = (char*)malloc(sizeof(str));
	Q_strncpy(result, str, sizeof(str));
	return result;
}

const char* CEconItemView::GetSoundOverride(CEconEntity *pEntity, const char* name)
{
	return GetSoundOverride(pEntity->GetItemDefIndex(), name);
}

const char* CEconItemView::GetSoundOverride(int ID, const char* name)
{
	char str[64];
	FIND_ELEMENT_STRING(GetItemSchema()->GetItemDefinition(ID)->visual.misc_info, name, str);
	char *result = (char*)malloc(sizeof(str));
	Q_strncpy(result, str, sizeof(str));
	return result;
}

bool CEconItemView::HasCapability(CEconEntity *pEntity, const char* name)
{
	return HasCapability(pEntity->GetItemDefIndex(), name);
}

bool CEconItemView::HasCapability(int ID, const char* name)
{
	bool result = false;
	FIND_ELEMENT(GetItemSchema()->GetItemDefinition(ID)->capabilities, name, result);
	return result;
}

bool CEconItemView::HasTag(CEconEntity *pEntity, const char* name)
{
	return HasTag(pEntity->GetItemDefIndex(), name);
}

bool CEconItemView::HasTag(int ID, const char* name)
{
	bool result = false;
	FIND_ELEMENT(GetItemSchema()->GetItemDefinition(ID)->tags, name, result);
	return result;
}

bool CEconItemView::HasBodygroupOverride(CEconEntity *pEntity, const char* name)
{
	return HasBodygroupOverride(pEntity->GetItemDefIndex(), name);
}

bool CEconItemView::HasBodygroupOverride(int ID, const char* name)
{
	bool result = false;
	FIND_ELEMENT(GetItemSchema()->GetItemDefinition(ID)->visual.player_bodygroups, name, result);
	return result;
}