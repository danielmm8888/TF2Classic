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

const char* CEconItemView::GetWorldDisplayModel(CEconEntity *pEntity)
{
	return GetWorldDisplayModel(pEntity->GetItemID());
}

const char* CEconItemView::GetWorldDisplayModel(int ID)
{
	char modelname[128];
	Q_strncpy(modelname, GetItemSchema()->GetItemDefinition(ID)->model_world, sizeof(modelname));
	if (!Q_stricmp(modelname, ""))
	{
		Q_strncpy(modelname, GetItemSchema()->GetItemDefinition(ID)->model_player, sizeof(modelname));
	}
	char *result = (char*)malloc(sizeof(modelname));
	Q_strncpy(result, modelname, sizeof(modelname));
	return result;

	return GetItemSchema()->GetItemDefinition(ID)->model_player;
}

const char* CEconItemView::GetPlayerDisplayModel(CEconEntity *pEntity)
{
	return GetPlayerDisplayModel(pEntity->GetItemID());
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
	return IsCosmetic(pEntity->GetItemID());
}

bool CEconItemView::IsCosmetic(int ID)
{
	bool result = false;

	FIND_ELEMENT(GetItemSchema()->GetItemDefinition(ID)->tags, "is_cosmetic", result);
	return result;
}