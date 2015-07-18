//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Simple Inventory
// by MrModez
// $NoKeywords: $
//=============================================================================//


#include "cbase.h"
#include "tf_shareddefs.h"
#include "tf_inventory.h"

CTFInventory::CTFInventory(){};

int CTFInventory::GetWeapon(int iClass, int iSlot, int iNum)
{
	return Weapons[iClass][iSlot][iNum];
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
	const char *pszWeaponName = WeaponIdToAlias(iWeapon);
	char sz[128];
	Q_snprintf(sz, sizeof(sz), "scripts/%s", pszWeaponName);
	CUtlDict< CHudTexture *, int > tempList;
	LoadHudTextures(tempList, sz, g_pGameRules->GetEncryptionKey());
	CHudTexture *p;
	switch (iTeam)
	{
	case 0: p = FindHudTextureInDict(tempList, "weapon");
	case 1: p = FindHudTextureInDict(tempList, "weapon_s");
	case 2: p = FindHudTextureInDict(tempList, "weapon_g");
	case 3: p = FindHudTextureInDict(tempList, "weapon_y");
	default:
		p = FindHudTextureInDict(tempList, "weapon");
	}
	char* sTextureFile = p->szTextureFile;
	return sTextureFile;
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
	"Building",
	"PDA",
	"PDA"
};

const int CTFInventory::Weapons[TF_CLASS_COUNT_ALL][INVENTORY_SLOTS][INVENTORY_WEAPONS] = 
{
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
				TF_WEAPON_MEDIGUN, TF_WEAPON_OVERHEALER
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
				TF_WEAPON_FISTS, TF_WEAPON_PIPE
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
				TF_WEAPON_REVOLVER, TF_WEAPON_TRANQ
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
		}
};