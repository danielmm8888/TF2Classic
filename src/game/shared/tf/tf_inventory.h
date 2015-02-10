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
//#include "server_class.h"
#include "tf_playeranimstate.h"
#include "tf_shareddefs.h"
#include "tf_weapon_parse.h"
#include "Filesystem.h" 
#if defined( CLIENT_DLL )
#include "c_tf_player.h"
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

	int GetWeapon(int iClass, int iSlot, int iNum);

#if defined( CLIENT_DLL )
	KeyValues* GetInventory(IBaseFileSystem *pFileSystem);
	void SetInventory(IBaseFileSystem *pFileSystem, KeyValues* pInventory);
	int GetLocalPreset(KeyValues* pInventory, int iClass, int iSlot);
	int GetWeaponPreset(IBaseFileSystem *pFileSystem, int iClass, int iSlot);
	char* GetWeaponBucket(int iWeapon, int iTeam);
	const char* GetSlotName(int iSlot);
	CHudTexture *FindHudTextureInDict(CUtlDict< CHudTexture *, int >& list, const char *psz);
#endif
	
	
private:
	static const int Weapons[TF_CLASS_COUNT_ALL][INVENTORY_SLOTS][INVENTORY_WEAPONS];
	static const char *g_aPlayerSlotNames[INVENTORY_SLOTS];
};

#endif // TF_INVENTORY_H
