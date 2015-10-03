#include "cbase.h"
#include "econ_itemview.h"
#include "activitylist.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


#ifdef CLIENT_DLL
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
#ifdef CLIENT_DLL
	RecvPropDataTable( RECVINFO_DT( m_Item ), 0, &REFERENCE_RECV_TABLE( DT_ScriptCreatedItem ) ),
#else
	SendPropDataTable( SENDINFO_DT( m_Item ), &REFERENCE_SEND_TABLE( DT_ScriptCreatedItem ) ),
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
	return GetWorldDisplayModel( pEntity->GetItemDefIndex() );
}

const char* CEconItemView::GetWorldDisplayModel(int ID, int iClass/* = 0*/)
{
	return GetItemSchema()->GetItemDefinition(ID)->model_world;
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

Activity CEconItemView::GetActivityOverride( CEconEntity *pEntity, int iTeamNumber, Activity actOriginalActivity )
{
	return GetActivityOverride( pEntity->GetItemDefIndex(), iTeamNumber, actOriginalActivity );
}

Activity CEconItemView::GetActivityOverride( int ID, int iTeamNumber, Activity actOriginalActivity )
{
	Activity actOverridenActivity = ACT_INVALID;
	for ( unsigned int i = 0; i < GetItemSchema()->GetItemDefinition( ID )->visual.animation_replacement.Count(); i++ )
	{
		const char *szActivityString = GetItemSchema()->GetItemDefinition( ID )->visual.animation_replacement.GetElementName( i );
		actOverridenActivity = (Activity)ActivityList_IndexForName( szActivityString );

		if ( actOverridenActivity == actOriginalActivity )
		{
			szActivityString = GetItemSchema()->GetItemDefinition( ID )->visual.animation_replacement.Element( i );
			actOverridenActivity = (Activity)ActivityList_IndexForName( szActivityString );
		}
	}
	return actOverridenActivity;
}

const char* CEconItemView::GetActivityOverride( CEconEntity *pEntity, int iTeamNumber, const char *name )
{
	return GetActivityOverride( pEntity->GetItemDefIndex(), iTeamNumber, name );
}

const char* CEconItemView::GetActivityOverride( int ID, int iTeamNumber, const char *name )
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