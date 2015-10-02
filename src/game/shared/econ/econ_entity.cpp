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

#ifdef TF_CLASSIC_CLIENT
EXTERN_RECV_TABLE(DT_ScriptCreatedItem)
#else
EXTERN_SEND_TABLE(DT_ScriptCreatedItem)
#endif

CEconEntity::CEconEntity()
{
	m_Item.SetItemDefIndex(-1);
}

void CEconEntity::SetItemDefIndex(int id)
{
	m_Item.SetItemDefIndex(id);
}

int CEconEntity::GetItemDefIndex()
{ 
	return m_Item.GetItemDefIndex();
}

bool CEconEntity::HasItemDefinition() const
{
	return (m_Item.GetItemDefIndex() >= 0);
}

CEconEntity::~CEconEntity()
{

}
