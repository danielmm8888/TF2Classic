#include "cbase.h"
#include "econ_itemview.h"
#include "activitylist.h"

#ifdef CLIENT_DLL
#include "dt_utlvector_recv.h"
#else
#include "dt_utlvector_send.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define MAX_ATTRIBUTES_SENT 20

#ifdef CLIENT_DLL
BEGIN_RECV_TABLE_NOBASE(CEconItemView, DT_ScriptCreatedItem)
	RecvPropInt(RECVINFO(m_iItemDefinitionIndex)),
	RecvPropInt(RECVINFO(m_iEntityQuality)),
	RecvPropInt(RECVINFO(m_iEntityLevel)),
	RecvPropInt(RECVINFO(m_iItemID)),
	RecvPropInt(RECVINFO(m_iInventoryPosition)),
	RecvPropInt(RECVINFO(m_iTeamNumber)),
	RecvPropBool(RECVINFO(m_bOnlyIterateItemViewAttributes)),
	RecvPropUtlVector( 
	RECVINFO_UTLVECTOR( m_AttributeList ),
	MAX_ATTRIBUTES_SENT,
	RecvPropDataTable( NULL, 0, 0, &REFERENCE_RECV_TABLE( DT_EconItemAttribute ) ) )
END_RECV_TABLE()
#else
BEGIN_SEND_TABLE_NOBASE(CEconItemView, DT_ScriptCreatedItem)
	SendPropInt(SENDINFO(m_iItemDefinitionIndex)),
	SendPropInt(SENDINFO(m_iEntityQuality)),
	SendPropInt(SENDINFO(m_iEntityLevel)),
	SendPropInt(SENDINFO(m_iItemID)),
	SendPropInt(SENDINFO(m_iInventoryPosition)),
	SendPropInt(SENDINFO(m_iTeamNumber)),
	SendPropBool(SENDINFO(m_bOnlyIterateItemViewAttributes)),
	SendPropUtlVector( 
	SENDINFO_UTLVECTOR( m_AttributeList ),
	MAX_ATTRIBUTES_SENT,
	SendPropDataTable( NULL, 0, &REFERENCE_SEND_TABLE( DT_EconItemAttribute ) ) )
END_SEND_TABLE()
#endif

#define FIND_ELEMENT(map, key, val)						\
		unsigned int index = map.Find(key);				\
		if (index != map.InvalidIndex())						\
			val = map.Element(index)				

#define FIND_ELEMENT_STRING(map, key, val)						\
		unsigned int index = map.Find(key);						\
		if (index != map.InvalidIndex())								\
			Q_snprintf(val, sizeof(val), map.Element(index))


EconItemDefinition *CEconItemView::GetStaticData( void ) const
{
	return GetItemSchema()->GetItemDefinition( m_iItemDefinitionIndex );
}

const char *CEconItemView::GetWorldDisplayModel( int iClass/* = 0*/ ) const
{
	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->model_world;
	}

	return NULL;
}

const char *CEconItemView::GetPlayerDisplayModel() const
{
	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->model_player;
	}

	return NULL;
}

const char* CEconItemView::GetEntityName()
{
	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->item_class;
	}

	return NULL;
}

bool CEconItemView::IsCosmetic()
{
	bool result = false;
	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->tags, "is_cosmetic", result );
	}

	return result;
}

int CEconItemView::GetAnimationSlot( void )
{
	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->anim_slot;
	}

	return -1;
}

Activity CEconItemView::GetActivityOverride( int iTeamNumber, Activity actOriginalActivity )
{
	EconItemDefinition *pStatic = GetStaticData();

	int iOverridenActivity = ACT_INVALID;

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->visual.animation_replacement, actOriginalActivity, iOverridenActivity );
	}

	return (Activity)iOverridenActivity;
}

const char *CEconItemView::GetActivityOverride( int iTeamNumber, const char *name )
{
	EconItemDefinition *pStatic = GetStaticData();
	int iOriginalAct = ActivityList_IndexForName( name );
	int iOverridenAct = ACT_INVALID;

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->visual.animation_replacement, iOriginalAct, iOverridenAct );
	}

	return ActivityList_NameForIndex( iOverridenAct );
}

const char *CEconItemView::GetSoundOverride( int iIndex ) const
{
	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->visual.aWeaponSounds[iIndex];
	}

	return NULL;
}

bool CEconItemView::HasCapability( const char* name )
{
	bool result = false;
	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->capabilities, name, result );
	}

	return result;
}

bool CEconItemView::HasTag( const char* name )
{
	bool result = false;
	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->tags, name, result );
	}

	return result;
}

CEconItemAttribute *CEconItemView::IterateAttributes( string_t strClass )
{
	// Returning the first attribute found.
	// This is not how live TF2 does this but this will do for now.
	for ( int i = 0; i < m_AttributeList.Count(); i++ )
	{
		CEconItemAttribute *pAttribute = &m_AttributeList[i];

#ifndef CLIENT_DLL
		string_t strMyClass = AllocPooledString( pAttribute->attribute_class );
#else
		EconAttributeDefinition *pStatic = pAttribute->GetStaticData();
		if ( !pStatic )
			continue;

		string_t strMyClass = AllocPooledString( pStatic->attribute_class );
#endif

		if ( strMyClass == strClass )
		{
			return &m_AttributeList[i];
		}
	}

	EconItemDefinition *pStatic = GetStaticData();

	if ( pStatic && !m_bOnlyIterateItemViewAttributes )
	{
		return pStatic->IterateAttributes( strClass );
	}

	return NULL;
}
