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
};

int CTFInventory::GetWeapon(int iClass, int iSlot, int iNum)
{
	return Weapons[iClass][iSlot][iNum];
};


bool CTFInventory::CheckValidSlot(int iClass, int iSlot)
{
	return CheckValidSlot(iClass, iSlot, 0);
}

bool CTFInventory::CheckValidWeapon(int iClass, int iSlot, int iWeapon)
{
	return CheckValidWeapon(iClass, iSlot, iWeapon, 0);
}

bool CTFInventory::CheckValidSlot(int iClass, int iSlot, bool HudCheck)
{
	if (iClass < TF_CLASS_UNDEFINED || iClass >= TF_CLASS_COUNT_ALL)
		return false;
	int iCount = (HudCheck ? INVENTORY_ROWNUM : INVENTORY_WEAPONS);
	if (iSlot >= iCount || iSlot < 0)
		return false;
	bool bWeapon = false;
	for (int i = 0; i < iCount; i++) //if there's at least one weapon in slot
	{
		if (Weapons[iClass][iSlot][i])
		{
			bWeapon = true;
			break;
		}
	}
	return bWeapon;
};

bool CTFInventory::CheckValidWeapon(int iClass, int iSlot, int iWeapon, bool HudCheck)
{
	if (iClass < TF_CLASS_UNDEFINED || iClass >= TF_CLASS_COUNT_ALL)
		return false;
	int iCount = (HudCheck ? INVENTORY_ROWNUM : INVENTORY_WEAPONS);
	if (iWeapon >= iCount || iWeapon < 0)
		return false;
	if (!Weapons[iClass][iSlot][iWeapon])
		return false;
	return true;
};

#if defined( CLIENT_DLL )
const char* CTFInventory::GetSlotName(int iSlot)
{
	return g_aPlayerSlotNames[iSlot];
};

CHudTexture *CTFInventory::FindHudTextureInDict(CUtlDict< CHudTexture *, int >& list, const char *psz)
{
	int idx = list.Find(psz);
	if (idx == list.InvalidIndex())
		return NULL;

	return list[idx];
};

KeyValues* CTFInventory::GetInventory(IBaseFileSystem *pFileSystem)
{
	KeyValues *pInv = new KeyValues("Inventory");
	pInv->LoadFromFile(pFileSystem, "scripts/tf_inventory.txt");
	return pInv;
};

void CTFInventory::SetInventory(IBaseFileSystem *pFileSystem, KeyValues* pInventory)
{
	pInventory->SaveToFile(pFileSystem, "scripts/tf_inventory.txt");
};

char* CTFInventory::GetWeaponBucket(int iWeapon, int iTeam)
{
	if (iWeapon == TF_WEAPON_BUILDER) //shit but works
		return "sprites/bucket_sapper";

	CTFWeaponInfo* pWeaponInfo = GetTFWeaponInfo(iWeapon);
	if (!pWeaponInfo)
		return "";
	CHudTexture *pHudTexture = (iTeam == TF_TEAM_RED ? pWeaponInfo->iconInactive : pWeaponInfo->iconActive);
	if (!pHudTexture)
		return "";
	return pHudTexture->szTextureFile;
};

int CTFInventory::GetLocalPreset(KeyValues* pInventory, int iClass, int iSlot)
{
	KeyValues *pSub = pInventory->FindKey(g_aPlayerClassNames_NonLocalized[iClass]);
	if (!pSub)
		return 0;
	const int iPreset = pSub->GetInt(g_aPlayerSlotNames[iSlot], 0);
	return iPreset;
};

int CTFInventory::GetWeaponPreset(IBaseFileSystem *pFileSystem, int iClass, int iSlot)
{
	return GetLocalPreset(GetInventory(pFileSystem), iClass, iSlot);
};
#endif

const char *CTFInventory::g_aPlayerSlotNames[INVENTORY_SLOTS] =
{
	"Primary",
	"Secondary",
	"Melee",
	"PDA",
	"PDA"
};

const int CTFInventory::Weapons[TF_CLASS_COUNT_ALL][INVENTORY_SLOTS][INVENTORY_WEAPONS] =
{
	{

	},
	{
		{
			TF_WEAPON_SCATTERGUN, TF_WEAPON_NAILGUN
		},
		{
			TF_WEAPON_PISTOL_SCOUT, TF_WEAPON_SMG_SCOUT
		},
		{
			TF_WEAPON_BAT
		}
	},
	{
		{
			TF_WEAPON_SNIPERRIFLE, TF_WEAPON_HUNTERRIFLE
		},
		{
			TF_WEAPON_SMG
		},
		{
			TF_WEAPON_CLUB, TF_WEAPON_FISHWHACKER
		}
	},
	{
		{
			TF_WEAPON_ROCKETLAUNCHER, TF_WEAPON_ROCKETLAUNCHERBETA
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
			TF_WEAPON_GRENADELAUNCHER, TF_WEAPON_CYCLOPS
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
			TF_WEAPON_SYRINGEGUN_MEDIC, TF_WEAPON_SHOTGUN_MEDIC
		},
		{
			TF_WEAPON_MEDIGUN, TF_WEAPON_OVERHEALER, TF_WEAPON_KRITZKRIEG
		},
		{
			TF_WEAPON_BONESAW, TF_WEAPON_UBERSAW
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
			TF_WEAPON_SHOTGUN_PYRO, TF_WEAPON_FLAREGUN
		},
		{
			TF_WEAPON_FIREAXE
		}
	},
	{
		{
			TF_WEAPON_REVOLVER, TF_WEAPON_TRANQ
		},
		{
			TF_WEAPON_BUILDER					// HACK!!! This is to make sapper show up on the loadout screen so spy doesn't end up with an empty slot.
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