//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Simple Inventory
// by MrModez
// $NoKeywords: $
//=============================================================================//


#include "cbase.h"
#include "tf_shareddefs.h"
#include "tf_inventory.h"
#include "econ_item_system.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define TF_INVENTORY_FILE "tf_inventory.txt"

static CTFInventory g_TFInventory;

CTFInventory *GetTFInventory()
{
	return &g_TFInventory;
}

CTFInventory::CTFInventory() : CAutoGameSystemPerFrame( "CTFInventory" )
{
#ifdef CLIENT_DLL
	m_pInventory = NULL;
#endif

	// Generate dummy base items.
	for ( int iClass = 0; iClass < TF_CLASS_COUNT_ALL; iClass++ )
	{
		for ( int iSlot = 0; iSlot < TF_LOADOUT_SLOT_COUNT; iSlot++ )
		{
			m_Items[iClass][iSlot].AddToTail( NULL );
		}
	}
};

CTFInventory::~CTFInventory()
{
#if defined( CLIENT_DLL )
	m_pInventory->deleteThis();
#endif
}

//-----------------------------------------------------------------------------
// Purpose: Fill the item arrays with data from item schema.
//-----------------------------------------------------------------------------
bool CTFInventory::Init( void )
{
	GetItemSchema()->Init();

	// Generate item list.
	FOR_EACH_MAP( GetItemSchema()->m_Items, i )
	{
		int iItemID = GetItemSchema()->m_Items.Key( i );
		CEconItemDefinition *pItemDef = GetItemSchema()->m_Items.Element( i );

		if ( pItemDef->item_slot == -1 )
			continue;

		// Add it to each class that uses it.
		for ( int iClass = 0; iClass < TF_CLASS_COUNT_ALL; iClass++ )
		{
			if ( pItemDef->used_by_classes & ( 1 << iClass ) )
			{
				// Show it if it's either base item or has show_in_armory flag.
				int iSlot = pItemDef->GetLoadoutSlot( iClass );

				if ( pItemDef->baseitem )
				{
					CEconItemView *pBaseItem = m_Items[iClass][iSlot][0];
					if ( pBaseItem != NULL )
					{
						Warning( "Duplicate base item %d for class %s in slot %s!\n", iItemID, g_aPlayerClassNames_NonLocalized[iClass], g_LoadoutSlots[iSlot] );
						delete pBaseItem;
					}

					m_Items[iClass][iSlot][0] = new CEconItemView( iItemID );
				}
				else if ( pItemDef->show_in_armory )
				{
					CEconItemView *pNewItem = new CEconItemView( iItemID );
					m_Items[iClass][iSlot].AddToTail( pNewItem );
				}
			}
		}
	}


#if defined( CLIENT_DLL )
	LoadInventory();
#endif

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFInventory::LevelInitPreEntity( void )
{
	GetItemSchema()->Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFInventory::GetWeapon( int iClass, int iSlot )
{
	return Weapons[iClass][iSlot];
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CEconItemView *CTFInventory::GetItem( int iClass, int iSlot, int iNum )
{
	if ( CheckValidWeapon( iClass, iSlot, iNum ) == false )
		return NULL;

	return m_Items[iClass][iSlot][iNum];
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFInventory::CheckValidSlot( int iClass, int iSlot, bool bHudCheck /*= false*/ )
{
	if ( iClass < TF_CLASS_UNDEFINED || iClass > TF_CLASS_COUNT )
		return false;

	int iCount = ( bHudCheck ? INVENTORY_ROWNUM : TF_LOADOUT_SLOT_COUNT );

	// Array bounds check.
	if ( iSlot >= iCount || iSlot < 0 )
		return false;

	// Slot must contain a base item.
	if ( m_Items[iClass][iSlot][0] == NULL )
		return false;

	return true;
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFInventory::CheckValidWeapon( int iClass, int iSlot, int iWeapon, bool bHudCheck /*= false*/ )
{
	if ( iClass < TF_CLASS_UNDEFINED || iClass > TF_CLASS_COUNT )
		return false;

	int iCount = ( bHudCheck ? INVENTORY_COLNUM : m_Items[iClass][iSlot].Count() );

	// Array bounds check.
	if ( iWeapon >= iCount || iWeapon < 0 )
		return false;

	// Don't allow switching if this class has no weapon at this position.
	if ( m_Items[iClass][iSlot][iWeapon] == NULL )
		return false;

	return true;
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFInventory::NumWeapons( int iClass, int iSlot )
{
	// Slot must contain a base item.
	if ( m_Items[iClass][iSlot][0] == NULL )
		return 0;

	return m_Items[iClass][iSlot].Count();
}

#if defined( CLIENT_DLL )
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFInventory::LoadInventory()
{
	bool bExist = filesystem->FileExists( TF_INVENTORY_FILE, "MOD" );
	if ( bExist )
	{
		if ( !m_pInventory )
		{
			m_pInventory = new KeyValues( "Inventory" );
		}
		m_pInventory->LoadFromFile( filesystem, TF_INVENTORY_FILE );
	}
	else
	{
		ResetInventory();
	}
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFInventory::SaveInventory()
{
	m_pInventory->SaveToFile( filesystem, TF_INVENTORY_FILE );
};

//-----------------------------------------------------------------------------
// Purpose: Create a default inventory file.
//-----------------------------------------------------------------------------
void CTFInventory::ResetInventory()
{
	if ( m_pInventory )
	{
		m_pInventory->deleteThis();
	}

	m_pInventory = new KeyValues( "Inventory" );

	for ( int i = TF_CLASS_UNDEFINED; i < TF_CLASS_COUNT_ALL; i++ )
	{
		KeyValues *pClassInv = new KeyValues( g_aPlayerClassNames_NonLocalized[i] );
		for ( int j = 0; j < TF_LOADOUT_SLOT_COUNT; j++ )
		{
			pClassInv->SetInt( g_LoadoutSlots[j], 0 );
		}
		m_pInventory->AddSubKey( pClassInv );
	}

	SaveInventory();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFInventory::GetWeaponPreset( int iClass, int iSlot )
{
	KeyValues *pClass = m_pInventory->FindKey( g_aPlayerClassNames_NonLocalized[iClass] );
	if ( !pClass )	//cannot find class node
	{
		ResetInventory();
		return 0;
	}
	int iPreset = pClass->GetInt( g_LoadoutSlots[iSlot], -1 );
	if ( iPreset == -1 )	//cannot find slot node
	{
		ResetInventory();
		return 0;
	}

	if ( CheckValidWeapon( iClass, iSlot, iPreset ) == false )
		return 0;

	return iPreset;
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFInventory::SetWeaponPreset( int iClass, int iSlot, int iPreset )
{
	KeyValues* pClass = m_pInventory->FindKey( g_aPlayerClassNames_NonLocalized[iClass] );
	if ( !pClass )	//cannot find class node
	{
		ResetInventory();
		pClass = m_pInventory->FindKey( g_aPlayerClassNames_NonLocalized[iClass] );
	}
	pClass->SetInt( GetSlotName( iSlot ), iPreset );
	SaveInventory();
}

const char* CTFInventory::GetSlotName( int iSlot )
{
	return g_LoadoutSlots[iSlot];
};

#endif

//-----------------------------------------------------------------------------
// Purpose: Legacy array, used when we're forced to use old method of giving out weapons.
//-----------------------------------------------------------------------------
const int CTFInventory::Weapons[TF_CLASS_COUNT_ALL][TF_PLAYER_WEAPON_COUNT] =
{
	{

	},
	{
		TF_WEAPON_SCATTERGUN,
		TF_WEAPON_PISTOL_SCOUT,
		TF_WEAPON_BAT
	},
	{
		TF_WEAPON_SNIPERRIFLE,
		TF_WEAPON_SMG,
		TF_WEAPON_CLUB
	},
	{
		TF_WEAPON_ROCKETLAUNCHER,
		TF_WEAPON_SHOTGUN_SOLDIER,
		TF_WEAPON_SHOVEL
	},
	{
		TF_WEAPON_GRENADELAUNCHER,
		TF_WEAPON_PIPEBOMBLAUNCHER,
		TF_WEAPON_BOTTLE
	},
	{
		TF_WEAPON_SYRINGEGUN_MEDIC,
		TF_WEAPON_MEDIGUN,
		TF_WEAPON_BONESAW
	},
	{
		TF_WEAPON_MINIGUN,
		TF_WEAPON_SHOTGUN_HWG,
		TF_WEAPON_FISTS
	},
	{
		TF_WEAPON_FLAMETHROWER,
		TF_WEAPON_SHOTGUN_PYRO,
		TF_WEAPON_FIREAXE
	},
	{
		TF_WEAPON_REVOLVER,
		TF_WEAPON_NONE,
		TF_WEAPON_KNIFE,
		TF_WEAPON_PDA_SPY,
		TF_WEAPON_INVIS
	},
	{
		TF_WEAPON_SHOTGUN_PRIMARY,
		TF_WEAPON_PISTOL,
		TF_WEAPON_WRENCH,
		TF_WEAPON_PDA_ENGINEER_BUILD,
		TF_WEAPON_PDA_ENGINEER_DESTROY
	},
	{
		TF_WEAPON_NONE,
		TF_WEAPON_NONE,
		TF_WEAPON_UMBRELLA
	},
	{
		TF_WEAPON_NONE,
		TF_WEAPON_PISTOL,
		TF_WEAPON_CROWBAR
	}
};
