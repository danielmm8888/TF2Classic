#include "cbase.h"
#include "attribute_manager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

BEGIN_NETWORK_TABLE_NOBASE( CAttributeManager, DT_AttributeManager )
#ifdef CLIENT_DLL
	RecvPropEHandle( RECVINFO( m_hOuter ) ),
#else
	SendPropEHandle( SENDINFO( m_hOuter ) ),
#endif
END_NETWORK_TABLE()

CAttributeManager::CAttributeManager()
{
	m_bParsingMyself = false;
}

void CAttributeManager::AddProvider( CBaseEntity *pEntity )
{
	m_AttributeProviders.AddToTail( pEntity );
}

void CAttributeManager::RemoveProvider( CBaseEntity *pEntity )
{
	m_AttributeProviders.FindAndRemove( pEntity );
}

void CAttributeManager::InitializeAttributes( CBaseEntity *pEntity )
{
	m_hOuter.Set( pEntity );
	m_bParsingMyself = false;
}

float CAttributeManager::ApplyAttributeFloat( float flValue, const CBaseEntity *pEntity, string_t strAttributeClass )
{
	if ( m_bParsingMyself || m_hOuter.Get() == NULL )
	{
		return flValue;
	}

	// Safeguard to prevent potential infinite loops.
	m_bParsingMyself = true;

	for ( int i = 0; i < m_AttributeProviders.Count(); i++ )
	{
		CBaseEntity *pProvider = m_AttributeProviders[i].Get();

		if ( !pProvider || pProvider == pEntity )
			continue;

		IHasAttributes *pAttribInterface = pProvider->GetHasAttributesInterfacePtr();

		if ( pAttribInterface )
		{
			flValue = pAttribInterface->GetAttributeManager()->ApplyAttributeFloat( flValue, pEntity, strAttributeClass );
		}
	}

	IHasAttributes *pAttribInterface = m_hOuter->GetHasAttributesInterfacePtr();

	if ( pAttribInterface && pAttribInterface->GetAttributeOwner() )
	{
		CBaseEntity *pOwner = pAttribInterface->GetAttributeOwner();
		IHasAttributes *pOwnerAttrib = pOwner->GetHasAttributesInterfacePtr();

		if ( pOwnerAttrib )
		{
			flValue = pOwnerAttrib->GetAttributeManager()->ApplyAttributeFloat( flValue, pEntity, strAttributeClass );
		}
	}

	m_bParsingMyself = false;

	return flValue;
}


BEGIN_NETWORK_TABLE_NOBASE( CAttributeContainer, DT_AttributeContainer )
#ifdef CLIENT_DLL
	RecvPropEHandle( RECVINFO( m_hOuter ) ),
#else
	SendPropEHandle( SENDINFO( m_hOuter ) ),
#endif
END_NETWORK_TABLE()

CAttributeContainer::CAttributeContainer()
{

}

float CAttributeContainer::ApplyAttributeFloat( float flValue, const CBaseEntity *pEntity, string_t strAttributeClass )
{
	if ( m_bParsingMyself || m_hOuter.Get() == NULL )
		return flValue;

	m_bParsingMyself = true;

	// This should only ever be used by econ entities.
	CEconEntity *pEconEnt = assert_cast<CEconEntity *>( m_hOuter.Get() );
	CEconItemView *pItem = pEconEnt->GetItem();

	CEconItemAttribute *pAttribute = pItem->IterateAttributes( strAttributeClass );

	if ( pAttribute )
	{
		EconAttributeDefinition *pStatic = pAttribute->GetStaticData();

		switch ( pStatic->description_format )
		{
		case ATTRIB_FORMAT_ADDITIVE:
		case ATTRIB_FORMAT_ADDITIVE_PERCENTAGE:
			flValue += pAttribute->value;
			break;
		case ATTRIB_FORMAT_PERCENTAGE:
		case ATTRIB_FORMAT_INVERTED_PERCENTAGE:
			flValue *= pAttribute->value;
			break;
		case ATTRIB_FORMAT_OR:
		{
			// Oh, man...
			int iValue = (int)flValue;
			int iAttrib = (int)pAttribute->value;
			iValue |= iAttrib;
			flValue = (float)iValue;
			break;
		}
		}
	}

	m_bParsingMyself = false;

	flValue = BaseClass::ApplyAttributeFloat( flValue, pEntity, strAttributeClass );

	return flValue;
}
