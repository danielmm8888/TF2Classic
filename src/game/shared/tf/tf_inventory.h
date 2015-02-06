//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Simple Inventory
// by MrModez
// $NoKeywords: $
//=============================================================================//
#ifndef TF_INVENTORY_H
#define TF_INVENTORY_H
#ifdef _WIN32
#pragma once
#endif

#include "cbase.h"
#include "server_class.h"
#include "tf_playeranimstate.h"
#include "tf_shareddefs.h"
#include "tf_player_shared.h"
#include "tf_weapon_parse.h"
#include "basecombatweapon_shared.h"
#include "Filesystem.h" 
//#include "player.h"
//#include "c_tf_player.h"
#if defined( CLIENT_DLL )
#include "history_resource.h"
#endif

#define INVENTORY_SLOTS			6
#define INVENTORY_WEAPONS		5
#define INVENTORY_TEAMS			4
#define INVENTORY_WEAPONS_COUNT	500
#define COLNUM	3
#define ROWNUM	3
#define VECTOR_NUM	COLNUM * ROWNUM

class CTFInventory
{
public:
	CTFInventory();

	int GetWeapon(int iClass, int iSlot, int iNum)
	{
		return Weapons[iClass][iSlot][iNum];
	};

	const char* GetSlotName(int iSlot)
	{
		return g_aPlayerSlotNames[iSlot];
	};

	static CHudTexture *FindHudTextureInDict(CUtlDict< CHudTexture *, int >& list, const char *psz)
	{
		int idx = list.Find(psz);
		if (idx == list.InvalidIndex())
			return NULL;

		return list[idx];
	}

	KeyValues* GetInventory(IBaseFileSystem *pFileSystem)
	{
		KeyValues *pInv = new KeyValues("Inventory");
		pInv->LoadFromFile(pFileSystem, "scripts/tf_inventory.txt");
		return pInv;
	};

	void SetInventory(IBaseFileSystem *pFileSystem, KeyValues* pInventory)
	{
		pInventory->SaveToFile(pFileSystem, "scripts/tf_inventory.txt");
	};

	int GetLocalPreset(KeyValues* pInventory, int iClass, int iSlot)
	{
		KeyValues *pSub = pInventory->FindKey(g_aPlayerClassNames_NonLocalized[iClass]);
		if (!pSub)
			return 0;
		const int iPreset = pSub->GetInt(g_aPlayerSlotNames[iSlot], 0);
		return iPreset;
	};

	char* GetWeaponBucket(int iWeapon, int iTeam)
	{
		//FileWeaponInfo_t *pWeaponInfo = new FileWeaponInfo_t();		
		//Q_snprintf(sz, sizeof(sz), "scripts/%s", pWeaponnfo->szIClassName);
		//return WeaponBuckets[iWeapon][iTeam];
#if defined( CLIENT_DLL )
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
#else
		return NULL;
#endif
	}; 
	
	
private:
	static const int Weapons[TF_CLASS_COUNT_ALL][INVENTORY_SLOTS][INVENTORY_WEAPONS];
	static const char *g_aPlayerSlotNames[INVENTORY_SLOTS];
};

#endif // TF_SHAREDDEFS_H
