//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#include "cbase.h"
#include "econ_entity.h"
#include "eventlist.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED( EconEntity, DT_EconEntity )

BEGIN_NETWORK_TABLE( CEconEntity, DT_EconEntity )
#ifdef CLIENT_DLL
	RecvPropDataTable( RECVINFO_DT( m_Item ), 0, &REFERENCE_RECV_TABLE( DT_ScriptCreatedItem ) ),
	RecvPropDataTable( RECVINFO_DT( m_AttributeManager ), 0, &REFERENCE_RECV_TABLE( DT_AttributeContainer ) ),
#else
	SendPropDataTable( SENDINFO_DT( m_Item ), &REFERENCE_SEND_TABLE( DT_ScriptCreatedItem ) ),
	SendPropDataTable( SENDINFO_DT( m_AttributeManager ), &REFERENCE_SEND_TABLE( DT_AttributeContainer ) ),
#endif
END_NETWORK_TABLE()

#ifdef CLIENT_DLL
BEGIN_PREDICTION_DATA( C_EconEntity )
	DEFINE_PRED_TYPEDESCRIPTION( m_AttributeManager, CAttributeContainer ),
END_PREDICTION_DATA()
#endif

CEconEntity::CEconEntity()
{
	m_pAttributes = this;
}

CEconEntity::~CEconEntity()
{
}

#ifdef CLIENT_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconEntity::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

	m_AttributeManager.OnPreDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconEntity::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	m_AttributeManager.OnDataChanged( updateType );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconEntity::FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options )
{
	if ( event == AE_CL_BODYGROUP_SET_VALUE_CMODEL_WPN )
	{
		// Something?
	}
	else
		BaseClass::FireEvent( origin, angles, event, options );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CEconEntity::OnFireEvent( C_BaseViewModel *pViewModel, const Vector& origin, const QAngle& angles, int event, const char *options )
{
	if ( event == AE_CL_BODYGROUP_SET_VALUE_CMODEL_WPN )
	{
		return true;
	}
	return false;
}

#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CEconEntity::SetItem( CEconItemView &newItem )
{
	m_Item = newItem;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CEconItemView *CEconEntity::GetItem( void )
{
	return &m_Item;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CEconEntity::HasItemDefinition( void ) const
{
	return ( m_Item.GetItemDefIndex() >= 0 );
}

//-----------------------------------------------------------------------------
// Purpose: Shortcut to get item ID.
//-----------------------------------------------------------------------------
int CEconEntity::GetItemID( void )
{
	return m_Item.GetItemDefIndex();
}

//-----------------------------------------------------------------------------
// Purpose: Derived classes need to override this.
//-----------------------------------------------------------------------------
void CEconEntity::GiveTo( CBaseEntity *pEntity )
{
}

//-----------------------------------------------------------------------------
// Purpose: Add or remove this from owner's attribute providers list.
//-----------------------------------------------------------------------------
void CEconEntity::ReapplyProvision( void )
{
	CBaseEntity *pOwner = GetOwnerEntity();
	CBaseEntity *pOldOwner = m_hOldOwner.Get();

	if ( pOwner != pOldOwner )
	{
		if ( pOldOwner )
		{
			m_AttributeManager.StopProvidingTo( pOldOwner );
		}

		if ( pOwner )
		{
			m_AttributeManager.ProviteTo( pOwner );
			m_hOldOwner = pOwner;
		}
		else
		{
			m_hOldOwner = NULL;
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CEconEntity::UpdateOnRemove( void )
{
	SetOwnerEntity( NULL );
	ReapplyProvision();
	BaseClass::UpdateOnRemove();
}
