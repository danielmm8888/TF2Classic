#include "cbase.h"
#include "econ_item_view.h"
#include "econ_item_system.h"
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
BEGIN_SEND_TABLE_NOBASE( CEconItemView, DT_ScriptCreatedItem )
	SendPropInt( SENDINFO( m_iItemDefinitionIndex ) ),
	SendPropInt( SENDINFO( m_iEntityQuality ) ),
	SendPropInt( SENDINFO( m_iEntityLevel ) ),
	SendPropInt( SENDINFO( m_iItemID ) ),
	SendPropInt( SENDINFO( m_iInventoryPosition ) ),
	SendPropInt( SENDINFO( m_iTeamNumber ) ),
	SendPropBool( SENDINFO( m_bOnlyIterateItemViewAttributes ) ),
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


CEconItemView::CEconItemView()
{
	Init( -1 );
}

CEconItemView::CEconItemView( int iItemID )
{
	Init( iItemID );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconItemView::Init( int iItemID )
{
	SetItemDefIndex( iItemID );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconItemView::SetItemDefIndex( int iItemID )
{
	m_iItemDefinitionIndex = iItemID;
	//m_pItemDef = GetItemSchema()->GetItemDefinition( m_iItemDefinitionIndex );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CEconItemView::GetItemDefIndex( void ) const
{
	return m_iItemDefinitionIndex;
}

//-----------------------------------------------------------------------------
// Purpose: Get static item definition from schema.
//-----------------------------------------------------------------------------
CEconItemDefinition *CEconItemView::GetStaticData( void ) const
{
	return GetItemSchema()->GetItemDefinition( m_iItemDefinitionIndex );
}

//-----------------------------------------------------------------------------
// Purpose: Get world model.
//-----------------------------------------------------------------------------
const char *CEconItemView::GetWorldDisplayModel( int iClass/* = 0*/ ) const
{
	CEconItemDefinition *pStatic = GetStaticData();
	const char *pszModelName = NULL;

	if ( pStatic )
	{
		pszModelName = pStatic->model_world;

		// Assuming we're using same model for both 1st person and 3rd person view.
		if ( !pszModelName[0] && pStatic->attach_to_hands == 1 )
		{
			pszModelName = pStatic->model_player;
		}
	}

	return pszModelName;
}

//-----------------------------------------------------------------------------
// Purpose: Get view model.
//-----------------------------------------------------------------------------
const char *CEconItemView::GetPlayerDisplayModel( int iClass/* = 0*/ ) const
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		if ( pStatic->model_player_per_class[iClass][0] != '\0' )
			return pStatic->model_player_per_class[iClass];

		return pStatic->model_player;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char* CEconItemView::GetEntityName()
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->item_class;
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CEconItemView::IsCosmetic()
{
	bool result = false;
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->tags, "is_cosmetic", result );
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CEconItemView::GetAnimationSlot( void )
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		return pStatic->anim_slot;
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Activity CEconItemView::GetActivityOverride( int iTeamNumber, Activity actOriginalActivity )
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		int iOverridenActivity = ACT_INVALID;

		EconItemVisuals *pVisuals = pStatic->GetVisuals( iTeamNumber );
		FIND_ELEMENT( pVisuals->animation_replacement, actOriginalActivity, iOverridenActivity );

		if ( iOverridenActivity != ACT_INVALID )
			return (Activity)iOverridenActivity;
	}

	return actOriginalActivity;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CEconItemView::GetActivityOverride( int iTeamNumber, const char *name )
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		int iOriginalAct = ActivityList_IndexForName( name );
		int iOverridenAct = ACT_INVALID;
		EconItemVisuals *pVisuals = pStatic->GetVisuals( iTeamNumber );

		FIND_ELEMENT( pVisuals->animation_replacement, iOriginalAct, iOverridenAct );

		if ( iOverridenAct != ACT_INVALID )
			return ActivityList_NameForIndex( iOverridenAct );
	}

	return name;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
const char *CEconItemView::GetSoundOverride( int iIndex, int iTeamNum /*= 0*/ ) const
{
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		EconItemVisuals *pVisuals = pStatic->GetVisuals( iTeamNum );
		return pVisuals->aWeaponSounds[iIndex];
	}

	return NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CEconItemView::HasCapability( const char* name )
{
	bool result = false;
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->capabilities, name, result );
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CEconItemView::HasTag( const char* name )
{
	bool result = false;
	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic )
	{
		FIND_ELEMENT( pStatic->tags, name, result );
	}

	return result;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CEconItemView::AddAttribute( CEconItemAttribute *pAttribute )
{
	// Make sure this attribute exists.
	EconAttributeDefinition *pAttribDef = pAttribute->GetStaticData();

	if ( pAttribDef )
	{
		m_AttributeList.AddToTail( *pAttribute );
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconItemView::SkipBaseAttributes( bool bSkip )
{
	m_bOnlyIterateItemViewAttributes = bSkip;
}

//-----------------------------------------------------------------------------
// Purpose: Find an attribute with the specified class.
//-----------------------------------------------------------------------------
CEconItemAttribute *CEconItemView::IterateAttributes( string_t strClass )
{
	// Returning the first attribute found.
	// This is not how live TF2 does this but this will do for now.
	for ( int i = 0; i < m_AttributeList.Count(); i++ )
	{
		CEconItemAttribute *pAttribute = &m_AttributeList[i];

#ifdef GAME_DLL
		string_t strMyClass = pAttribute->m_strAttributeClass;
#else
		// FIXME: Need to find a proper solution here.
		string_t strMyClass = AllocPooledString( pAttribute->attribute_class );
#endif

		if ( strMyClass == strClass )
		{
			return pAttribute;
		}
	}

	CEconItemDefinition *pStatic = GetStaticData();

	if ( pStatic && !m_bOnlyIterateItemViewAttributes )
	{
		return pStatic->IterateAttributes( strClass );
	}

	return NULL;
}
