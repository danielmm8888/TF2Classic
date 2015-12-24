//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Simple Inventory
// by MrModez
// $NoKeywords: $
//=============================================================================//


#include "cbase.h"
#include "tf_shareddefs.h"
#include "tf_inventory.h"

CTFInventory *pTFInventory = NULL;

CTFInventory *GetTFInventory()
{
	if (NULL == pTFInventory)
	{
		pTFInventory = new CTFInventory();
	}
	return pTFInventory;
}

CTFInventory::CTFInventory()
{
#if defined( CLIENT_DLL )
	m_pInventory = NULL;
	LoadInventory();
#endif
};

CTFInventory::~CTFInventory()
{
#if defined( CLIENT_DLL )
	m_pInventory->deleteThis();
#endif
}

int CTFInventory::GetWeapon(int iClass, int iSlot, int iNum)
{
	return Weapons[iClass][iSlot][iNum];
};

int CTFInventory::GetItem(int iClass, int iSlot, int iNum)
{
	return Items[iClass][iSlot][iNum];
};

bool CTFInventory::CheckValidSlot(int iClass, int iSlot, bool bEcon, bool HudCheck)
{
	if (iClass < TF_CLASS_UNDEFINED || iClass >= TF_CLASS_COUNT_ALL)
		return false;
	int iCount = (HudCheck ? INVENTORY_ROWNUM : INVENTORY_WEAPONS);
	if (iSlot >= iCount || iSlot < 0)
		return false;
	bool bWeapon = false;
	for (int i = 0; i < iCount; i++) //if there's at least one weapon in slot
	{
		if (!bEcon)
		{
			if (Weapons[iClass][iSlot][i])
			{
				bWeapon = true;
				break;
			}
		}
		else
		{
			if (Items[iClass][iSlot][i])
			{
				bWeapon = true;
				break;
			}
		}
	}
	return bWeapon;
};

bool CTFInventory::CheckValidWeapon(int iClass, int iSlot, int iWeapon, bool bEcon, bool HudCheck)
{
	if (iClass < TF_CLASS_UNDEFINED || iClass >= TF_CLASS_COUNT_ALL)
		return false;

	int iCount = (HudCheck ? INVENTORY_ROWNUM : INVENTORY_WEAPONS);

	// Array bounds check.
	if (iWeapon >= iCount || iWeapon < 0)
		return false;

	// Don't allow switching if this class has no weapon at this position.
	if (!bEcon)
	{
		if (!Weapons[iClass][iSlot][iWeapon])
			return false;
	}
	else
	{
		if (!Items[iClass][iSlot][iWeapon])
			return false;
	}

	return true;
};

#if defined( CLIENT_DLL )
void CTFInventory::LoadInventory()
{
	bool bExist = filesystem->FileExists("scripts/tf_inventory.txt", "MOD");
	if (bExist)
	{
		if (!m_pInventory)
		{
			m_pInventory = new KeyValues("Inventory");
		}
		m_pInventory->LoadFromFile(filesystem, "scripts/tf_inventory.txt");
	}
	else
	{
		ResetInventory();
	}
};

void CTFInventory::SaveInventory()
{
	m_pInventory->SaveToFile(filesystem, "scripts/tf_inventory.txt");
};

void CTFInventory::ResetInventory()
{
	if (m_pInventory)
	{
		m_pInventory->deleteThis();
	}
	m_pInventory = new KeyValues("Inventory");
	for (int i = TF_CLASS_UNDEFINED; i < TF_CLASS_COUNT_ALL; i++)
	{
		KeyValues *pClassInv = new KeyValues(g_aPlayerClassNames_NonLocalized[i]);
		for (int j = 0; j < INVENTORY_SLOTS; j++)
		{
			pClassInv->SetInt(g_aPlayerSlotNames[j], 0);
		}
		m_pInventory->AddSubKey(pClassInv);
	}
	SaveInventory();
}

int CTFInventory::GetWeaponPreset(int iClass, int iSlot)
{
	KeyValues *pClass = m_pInventory->FindKey(g_aPlayerClassNames_NonLocalized[iClass]);
	if (!pClass)	//cannot find class node
	{	
		ResetInventory();
		return 0;
	}
	int iPreset = pClass->GetInt(g_aPlayerSlotNames[iSlot], -1);
	if (iPreset == -1)	//cannot find slot node
	{
		ResetInventory();
		return 0;
	}
	return iPreset;
};

void CTFInventory::SetWeaponPreset(int iClass, int iSlot, int iPreset)
{
	KeyValues* pClass = m_pInventory->FindKey(g_aPlayerClassNames_NonLocalized[iClass]);
	if (!pClass)	//cannot find class node
	{
		ResetInventory();
		pClass = m_pInventory->FindKey(g_aPlayerClassNames_NonLocalized[iClass]);
	}
	pClass->SetInt(GetSlotName(iSlot), iPreset);
	SaveInventory();
}

const char* CTFInventory::GetSlotName(int iSlot)
{
	return g_aPlayerSlotNames[iSlot];
};

const char *CTFInventory::g_aPlayerSlotNames[INVENTORY_SLOTS] =
{
	"primary",
	"secondary",
	"melee",
	"pda1",
	"pda2"
};
#endif

// Legacy array, used when we're forced to use old method of giving out weapons.
const int CTFInventory::Weapons[TF_CLASS_COUNT_ALL][INVENTORY_SLOTS][INVENTORY_WEAPONS] =
{
	{

	},
	{
		{
			TF_WEAPON_SCATTERGUN
		},
		{
			TF_WEAPON_PISTOL_SCOUT
		},
		{
			TF_WEAPON_BAT
		}
	},
	{
		{
			TF_WEAPON_SNIPERRIFLE
		},
		{
			TF_WEAPON_SMG
		},
		{
			TF_WEAPON_CLUB
		}
	},
	{
		{
			TF_WEAPON_ROCKETLAUNCHER,
		},
		{
			TF_WEAPON_SHOTGUN_SOLDIER
		},
		{
			TF_WEAPON_SHOVEL
		}
	},
	{
		{
			TF_WEAPON_GRENADELAUNCHER
		},
		{
			TF_WEAPON_PIPEBOMBLAUNCHER
		},
		{
			TF_WEAPON_BOTTLE
		}
	},
	{
		{
			TF_WEAPON_SYRINGEGUN_MEDIC
		},
		{
			TF_WEAPON_MEDIGUN
		},
		{
			TF_WEAPON_BONESAW
		}
	},
	{
		{
			TF_WEAPON_MINIGUN
		},
		{
			TF_WEAPON_SHOTGUN_HWG
		},
		{
			TF_WEAPON_FISTS
		}
	},
	{
		{
			TF_WEAPON_FLAMETHROWER
		},
		{
			TF_WEAPON_SHOTGUN_PYRO
		},
		{
			TF_WEAPON_FIREAXE
		}
	},
	{
		{
			TF_WEAPON_REVOLVER
		},
		{

		},
		{
			TF_WEAPON_KNIFE
		},
		{
			TF_WEAPON_PDA_SPY
		},
		{
			TF_WEAPON_INVIS
		}
	},
	{
		{
			TF_WEAPON_SHOTGUN_PRIMARY
		},
		{
			TF_WEAPON_PISTOL
		},
		{
			TF_WEAPON_WRENCH
		},
		{
			TF_WEAPON_PDA_ENGINEER_BUILD
		},
		{
			TF_WEAPON_PDA_ENGINEER_DESTROY
		}
	},
	{
		{

		},
		{

		},
		{
			TF_WEAPON_UMBRELLA
		}
	},
	{
		{

		},
		{
			TF_WEAPON_PISTOL
		},
		{
			TF_WEAPON_CROWBAR
		}
	}
};

const int CTFInventory::Items[TF_CLASS_COUNT_ALL][INVENTORY_SLOTS][INVENTORY_WEAPONS] =
{
	{ // Unassigned

	},
	{ // Scout
		{
			13, 9002
		},
		{
			23, 9003
		},
		{
			0
		}
	},
	{ // Sniper
		{
			14, 9007
		},
		{
			16
		},
		{
			3, 9008
		}
	},
	{ // Soldier
		{
			18, 9004
		},
		{
			10
		},
		{
			6
		}
	},
	{ // Demoman
		{
			19, 9005
		},
		{
			20
		},
		{
			1
		}
	},
	{ // Medic
		{
			17
		},
		{
			29, 35, 9006
		},
		{
			8, 37
		}
	},
	{ // Heavy
		{
			15
		},
		{
			11
		},
		{
			5
		}
	},
	{ // Pyro
		{
			21
		},
		{
			12, 39
		},
		{
			2
		}
	},
	{ // Spy
		{
			24, 9009
		},
		{
			735					// HACK!!! This is to make sapper show up on the loadout screen so spy doesn't end up with an empty slot.
		},
		{
			4
		},
		{
			27
		},
		{
			30
		}
	},
	{ // Engineer
		{
			9
		},
		{
			22
		},
		{
			7
		},
		{
			25
		},
		{
			26
		}
	},
	{ // Civilian
		{

		},
		{

		},
		{
			//TF_WEAPON_UMBRELLA
		}
	},
	{ // Mercenary
		{

		},
		{
			22
		},
		{
			9010
		}
	}
};
