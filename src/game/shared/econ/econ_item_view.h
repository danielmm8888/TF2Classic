#ifndef TF_ECON_ITEM_VIEW_H
#define TF_ECON_ITEM_VIEW_H

#ifdef _WIN32
#pragma once
#endif

#include "econ_item_schema.h"

class CEconItemHandle
{
public:
	// CEconItem *m_pItem; TODO: not implemented yet, get a GC going

	uint64 m_SteamID; // The owner of the item?
	int m_ItemID;
};


#ifdef CLIENT_DLL
EXTERN_RECV_TABLE( DT_ScriptCreatedItem )
#else
EXTERN_SEND_TABLE( DT_ScriptCreatedItem )
#endif

class CEconItemView
{
public:
	DECLARE_CLASS_NOBASE( CEconItemView );
	DECLARE_EMBEDDED_NETWORKVAR();
	CEconItemView();
	CEconItemView( int iItemID );

	void Init( int iItemID );

	CEconItemDefinition *GetStaticData( void ) const;

	const char* GetWorldDisplayModel( int iClass = 0 ) const;
	const char* GetPlayerDisplayModel( int iClass = 0 ) const;
	const char* GetEntityName( void );
	bool IsCosmetic( void );
	int GetAnimationSlot( void );
	Activity GetActivityOverride( int iTeamNumber, Activity actOriginalActivity );
	const char* GetActivityOverride( int iTeamNumber, const char *name );
	const char* GetSoundOverride( int iIndex, int iTeamNum = TEAM_UNASSIGNED ) const;
	bool HasCapability( const char* name );
	bool HasTag( const char* name );

	bool AddAttribute( CEconItemAttribute *pAttribute );
	void SkipBaseAttributes( bool bSkip );
	CEconItemAttribute *IterateAttributes( string_t strClass );

	void SetItemDefIndex( int iItemID );
	int GetItemDefIndex( void ) const;

private:
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

#endif // TF_ECON_ITEM_VIEW_H
