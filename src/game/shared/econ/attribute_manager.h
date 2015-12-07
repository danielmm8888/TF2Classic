#ifndef ATTRIBUTE_MANAGER_H
#define ATTRIBUTE_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "econ_itemschema.h"
#include "econ_itemview.h"
#include "gamestringpool.h"

// Client specific.
#ifdef CLIENT_DLL
EXTERN_RECV_TABLE( DT_AttributeManager );
EXTERN_RECV_TABLE( DT_AttributeContainer );
// Server specific.
#else
EXTERN_SEND_TABLE( DT_AttributeManager );
EXTERN_SEND_TABLE( DT_AttributeContainer );
#endif

class CAttributeManager
{
public:
	DECLARE_EMBEDDED_NETWORKVAR();
	DECLARE_CLASS_NOBASE( CAttributeManager );

	CAttributeManager();

	template <typename type>
	static type AttribHookValue( type iValue, const char* text, const CBaseEntity *pEntity )
	{
		if ( !pEntity )
			return iValue;

		IHasAttributes *pAttribInteface = pEntity->GetHasAttributesInterfacePtr();

		if ( pAttribInteface )
		{
			string_t strAttributeClass = AllocPooledString_StaticConstantStringPointer( text );
			float flResult = pAttribInteface->GetAttributeManager()->ApplyAttributeFloat( iValue, pEntity, strAttributeClass );
			iValue = (type)flResult;
		}

		return iValue;
	}

	void			AddProvider( CBaseEntity *pEntity );
	void			RemoveProvider( CBaseEntity *pEntity );
	virtual void	InitializeAttributes( CBaseEntity *pEntity );
	virtual float	ApplyAttributeFloat( float flValue, const CBaseEntity *pEntity, string_t strAttributeClass );

protected:
	CNetworkHandle( CBaseEntity, m_hOuter );
	bool m_bParsingMyself;

private:
	CUtlVector<EHANDLE> m_AttributeProviders;
};


class CAttributeContainer : public CAttributeManager
{
public:
	DECLARE_EMBEDDED_NETWORKVAR();
	DECLARE_CLASS( CAttributeContainer, CAttributeManager );

	CAttributeContainer();

	float ApplyAttributeFloat( float flValue, const CBaseEntity *pEntity, string_t strAttributeClass );

public:
	CEconItemView m_Item;
};

#endif // ATTRIBUTE_MANAGER_H
