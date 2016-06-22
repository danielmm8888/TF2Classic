#include "cbase.h"
#include "econ_item_schema.h"
#include "econ_item_system.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================
// CEconItemAttribute
//=============================================================================

BEGIN_NETWORK_TABLE_NOBASE( CEconItemAttribute, DT_EconItemAttribute )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iAttributeDefinitionIndex ) ),
	RecvPropFloat( RECVINFO( value ) ),
	RecvPropString( RECVINFO( attribute_class ) ),
#else
	SendPropInt( SENDINFO( m_iAttributeDefinitionIndex ) ),
	SendPropFloat( SENDINFO( value ) ),
	SendPropString( SENDINFO( attribute_class ) ),
#endif
END_NETWORK_TABLE()


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconItemAttribute::Init( int iIndex, float flValue, const char *pszAttributeClass /*= NULL*/ )
{
	m_iAttributeDefinitionIndex = iIndex;
	value = flValue;

	if ( pszAttributeClass )
	{
		V_strncpy( attribute_class.GetForModify(), pszAttributeClass, sizeof( attribute_class ) );
	}
	else
	{
		EconAttributeDefinition *pAttribDef = GetStaticData();
		if ( pAttribDef )
		{
			V_strncpy( attribute_class.GetForModify(), pAttribDef->attribute_class, sizeof( attribute_class ) );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
EconAttributeDefinition *CEconItemAttribute::GetStaticData( void )
{
	return GetItemSchema()->GetAttributeDefinition( m_iAttributeDefinitionIndex );
}


//=============================================================================
// CEconItemDefinition
//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
EconItemVisuals *CEconItemDefinition::GetVisuals( int iTeamNum /*= TEAM_UNASSIGNED*/ )
{
	if ( iTeamNum > LAST_SHARED_TEAM && iTeamNum < TF_TEAM_COUNT )
	{
		return &visual[iTeamNum];
	}

	return &visual[TEAM_UNASSIGNED];
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CEconItemDefinition::GetLoadoutSlot( int iClass /*= TF_CLASS_UNDEFINED*/ )
{
	if ( iClass && item_slot_per_class[iClass] != -1 )
	{
		return item_slot_per_class[iClass];
	}

	return item_slot;
}

//-----------------------------------------------------------------------------
// Purpose: Find an attribute with the specified class.
//-----------------------------------------------------------------------------
CEconItemAttribute *CEconItemDefinition::IterateAttributes( string_t strClass )
{
	// Returning the first attribute found.
	for ( int i = 0; i < attributes.Count(); i++ )
	{
		CEconItemAttribute *pAttribute = &attributes[i];
		string_t strMyClass = AllocPooledString( pAttribute->attribute_class.Get() );

		if ( strMyClass == strClass )
		{
			return pAttribute;
		}
	}

	return NULL;
}
