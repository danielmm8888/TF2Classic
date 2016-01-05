#include "cbase.h"
#include "attribute_manager.h"
#include "econ_item_schema.h"

#ifdef CLIENT_DLL
#include "prediction.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define ATTRIB_REAPPLY_PARITY_BITS 3

BEGIN_NETWORK_TABLE_NOBASE( CAttributeManager, DT_AttributeManager )
#ifdef CLIENT_DLL
	RecvPropEHandle( RECVINFO( m_hOuter ) ),
	RecvPropInt( RECVINFO( m_iReapplyProvisionParity ) ),
#else
	SendPropEHandle( SENDINFO( m_hOuter ) ),
	SendPropInt( SENDINFO( m_iReapplyProvisionParity ), ATTRIB_REAPPLY_PARITY_BITS, SPROP_UNSIGNED ),
#endif
END_NETWORK_TABLE()

CAttributeManager::CAttributeManager()
{
	m_bParsingMyself = false;
	m_iReapplyProvisionParity = 0;
}

#ifdef CLIENT_DLL
void CAttributeManager::OnPreDataChanged( DataUpdateType_t updateType )
{
	m_iOldReapplyProvisionParity = m_iReapplyProvisionParity;
}

void CAttributeManager::OnDataChanged( DataUpdateType_t updateType )
{
	// If parity ever falls out of sync we can catch up here.
	if ( m_iReapplyProvisionParity != m_iOldReapplyProvisionParity )
	{
		if ( m_hOuter )
		{
			IHasAttributes *pAttributes = m_hOuter->GetHasAttributesInterfacePtr();
			pAttributes->ReapplyProvision();
			m_iOldReapplyProvisionParity = m_iReapplyProvisionParity;
		}
	}
}

#endif

void CAttributeManager::AddProvider( CBaseEntity *pEntity )
{
	m_AttributeProviders.AddToTail( pEntity );
}

void CAttributeManager::RemoveProvider( CBaseEntity *pEntity )
{
	m_AttributeProviders.FindAndRemove( pEntity );
}

void CAttributeManager::ProviteTo( CBaseEntity *pEntity )
{
	if ( !pEntity || !m_hOuter.Get() )
		return;

	IHasAttributes *pAttributes = pEntity->GetHasAttributesInterfacePtr();

	if ( pAttributes )
	{
		pAttributes->GetAttributeManager()->AddProvider( m_hOuter.Get() );
	}

#ifdef CLIENT_DLL
	if ( prediction->InPrediction() )
#endif
	m_iReapplyProvisionParity = ( m_iReapplyProvisionParity + 1 ) & ( ( 1 << ATTRIB_REAPPLY_PARITY_BITS ) - 1 );
}

void CAttributeManager::StopProvidingTo( CBaseEntity *pEntity )
{
	if ( !pEntity || !m_hOuter.Get() )
		return;

	IHasAttributes *pAttributes = pEntity->GetHasAttributesInterfacePtr();

	if ( pAttributes )
	{
		pAttributes->GetAttributeManager()->RemoveProvider( m_hOuter.Get() );
	}

#ifdef CLIENT_DLL
	if ( prediction->InPrediction() )
#endif
	m_iReapplyProvisionParity = ( m_iReapplyProvisionParity + 1 ) & ( ( 1 << ATTRIB_REAPPLY_PARITY_BITS ) - 1 );
}

void CAttributeManager::InitializeAttributes( CBaseEntity *pEntity )
{
	Assert( pEntity->GetHasAttributesInterfacePtr() != NULL );

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

		IHasAttributes *pAttributes = pProvider->GetHasAttributesInterfacePtr();

		if ( pAttributes )
		{
			flValue = pAttributes->GetAttributeManager()->ApplyAttributeFloat( flValue, pEntity, strAttributeClass );
		}
	}

	IHasAttributes *pAttributes = m_hOuter->GetHasAttributesInterfacePtr();
	CBaseEntity *pOwner = pAttributes->GetAttributeOwner();

	if ( pOwner )
	{
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
	RecvPropInt( RECVINFO( m_iReapplyProvisionParity ) ),
#else
	SendPropEHandle( SENDINFO( m_hOuter ) ),
	SendPropInt( SENDINFO( m_iReapplyProvisionParity ), ATTRIB_REAPPLY_PARITY_BITS, SPROP_UNSIGNED ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA_NO_BASE( CAttributeContainer )
	DEFINE_PRED_FIELD( m_iReapplyProvisionParity, FIELD_INTEGER, FTYPEDESC_INSENDTABLE ),
END_PREDICTION_DATA()
#endif

CAttributeContainer::CAttributeContainer()
{

}

float CAttributeContainer::ApplyAttributeFloat( float flValue, const CBaseEntity *pEntity, string_t strAttributeClass )
{
	if ( m_bParsingMyself || m_hOuter.Get() == NULL )
		return flValue;

	m_bParsingMyself = true;;

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
