//=============================================================================
//
// Purpose: Applies attributes.
//
//=============================================================================

#ifndef ATTRIBUTE_MANAGER_H
#define ATTRIBUTE_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

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
	static type AttribHookValue( type value, const char* text, const CBaseEntity *pEntity )
	{
		if ( !pEntity )
			return value;

		IHasAttributes *pAttribInteface = pEntity->GetHasAttributesInterfacePtr();

		if ( pAttribInteface )
		{
			string_t strAttributeClass = AllocPooledString_StaticConstantStringPointer( text );
			float flResult = pAttribInteface->GetAttributeManager()->ApplyAttributeFloat( (float)value, pEntity, strAttributeClass );
			value = (type)flResult;
		}

		return value;
	}

	template <>
	static string_t AttribHookValue<string_t>( string_t strValue, const char *text, const CBaseEntity *pEntity )
	{
		if ( !pEntity )
			return strValue;

		IHasAttributes *pAttribInteface = pEntity->GetHasAttributesInterfacePtr();

		if ( pAttribInteface )
		{
			string_t strAttributeClass = AllocPooledString_StaticConstantStringPointer( text );
			strValue = pAttribInteface->GetAttributeManager()->ApplyAttributeString( strValue, pEntity, strAttributeClass );
		}

		return strValue;
}

#ifdef CLIENT_DLL
	virtual void		OnPreDataChanged( DataUpdateType_t updateType );
	virtual void		OnDataChanged( DataUpdateType_t updatetype );
#endif
	void				AddProvider( CBaseEntity *pEntity );
	void				RemoveProvider( CBaseEntity *pEntity );
	void				ProviteTo( CBaseEntity *pEntity );
	void				StopProvidingTo( CBaseEntity *pEntity );
	virtual void		InitializeAttributes( CBaseEntity *pEntity );
	virtual float		ApplyAttributeFloat( float flValue, const CBaseEntity *pEntity, string_t strAttributeClass );
	virtual string_t	ApplyAttributeString( string_t strValue, const CBaseEntity *pEntity, string_t strAttributeClass );

protected:
	CNetworkHandle( CBaseEntity, m_hOuter );
	bool m_bParsingMyself;

	CNetworkVar( int, m_iReapplyProvisionParity );
#ifdef CLIENT_DLL
	int m_iOldReapplyProvisionParity;
#endif

private:
	CUtlVector<EHANDLE> m_AttributeProviders;
};


class CAttributeContainer : public CAttributeManager
{
public:
	DECLARE_EMBEDDED_NETWORKVAR();
	DECLARE_CLASS( CAttributeContainer, CAttributeManager );
#ifdef CLIENT_DLL
	DECLARE_PREDICTABLE();
#endif

	CAttributeContainer();

	float		ApplyAttributeFloat( float flValue, const CBaseEntity *pEntity, string_t strAttributeClass );
	string_t	ApplyAttributeString( string_t strValue, const CBaseEntity *pEntity, string_t strAttributeClass );
};

#endif // ATTRIBUTE_MANAGER_H
