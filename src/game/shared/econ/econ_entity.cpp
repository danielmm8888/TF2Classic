//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//===========================================================================//

#include "cbase.h"
#include "econ_entity.h"
#include "econ_itemschema.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


IMPLEMENT_NETWORKCLASS_ALIASED( EconEntity, DT_EconEntity )

BEGIN_NETWORK_TABLE( CEconEntity, DT_EconEntity )
#ifdef TF_CLASSIC_CLIENT
RecvPropInt(RECVINFO(m_Item)),
#else
SendPropInt(SENDINFO(m_Item)),
#endif
END_NETWORK_TABLE()


CEconEntity::CEconEntity()
{
	m_Item.Set(-1);
}

void CEconEntity::SetItemID(int id)
{
	m_Item.Set(id); 
}

int CEconEntity::GetItemID()
{ 
	return m_Item.Get(); 
}

bool CEconEntity::HasItemDefinition() const
{
	return (m_Item.Get() >= 0); 
}

CEconEntity::~CEconEntity()
{

}
