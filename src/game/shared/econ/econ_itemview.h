#ifndef TF_ECON_ITEMVIEW_H
#define TF_ECON_ITEMVIEW_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"
#include "igamesystem.h"
#include "GameEventListener.h"
#include "networkvar.h"

#include "econ_itemschema.h"

class CEconItemHandle
{
public:
	// CEconItem *m_pItem; TODO: not implemented yet, get a GC going

	uint64 m_SteamID; // The owner of the item?
	int m_ItemID;
};


class CEconItemView
{
public:
	DECLARE_CLASS_NOBASE( CEconItemView );
	DECLARE_EMBEDDED_NETWORKVAR();
	CEconItemView() {}
	CEconItemView( int m_ItemID ){ SetItemDefIndex(m_ItemID); }

	EconItemDefinition *GetStaticData( void ) const;

	const char* GetWorldDisplayModel( int iClass = 0 ) const;
	const char* GetPlayerDisplayModel( void ) const;
	const char* GetEntityName( void );
	bool IsCosmetic( void );
	int GetAnimationSlot( void );
	Activity GetActivityOverride( int iTeamNumber, Activity actOriginalActivity );
	const char* GetActivityOverride( int iTeamNumber, const char *name );
	const char* GetSoundOverride( const char* name );
	bool HasCapability( const char* name );
	bool HasTag( const char* name );

	CEconItemAttribute *IterateAttributes( string_t strClass );

	void SetItemDefIndex( int iItemID ) { m_iItemDefinitionIndex = iItemID; }
	int GetItemDefIndex( void ) const { return m_iItemDefinitionIndex; }

protected:
	CNetworkVar( short, m_iItemDefinitionIndex );

	CNetworkVar( int, m_iEntityQuality ); // maybe an enum?
	CNetworkVar( int, m_iEntityLevel );

	CNetworkVar( int, m_iItemID );
	CNetworkVar( uint64, m_iAccountID );
	CNetworkVar( int, m_iInventoryPosition );

	CEconItemHandle m_ItemHandle; // The handle to the CEconItem on the GC

	CNetworkVar( int, m_iTeamNumber );
	//bool m_bInitialized; // ?

	//CUtlDict< EconItemAttribute, unsigned short > m_AttributeList;
	CNetworkVar( bool, m_bOnlyIterateItemViewAttributes );

	CUtlVector<CEconItemAttribute> m_AttributeList;
};

#endif // TF_ECON_ITEMVIEW_H
