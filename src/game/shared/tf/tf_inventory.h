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
#include "filesystem.h" 
#if defined( CLIENT_DLL )
#include "c_tf_player.h"
#endif

#define INVENTORY_SLOTS			5
#define INVENTORY_WEAPONS		5
#define INVENTORY_WEAPONS_COUNT	500
#define INVENTORY_COLNUM		5
#define INVENTORY_ROWNUM		3
#define INVENTORY_VECTOR_NUM	INVENTORY_COLNUM * INVENTORY_ROWNUM

class CTFInventory
{
public:
	CTFInventory();

	int GetWeapon(int iClass, int iSlot, int iNum);
	bool CheckValidSlot(int iClass, int iSlot);
	bool CheckValidWeapon(int iClass, int iSlot, int iWeapon);
	bool CheckValidSlot(int iClass, int iSlot, bool HudCheck);
	bool CheckValidWeapon(int iClass, int iSlot, int iWeapon, bool HudCheck);

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

CTFInventory *GetTFInventory();

#endif // TF_INVENTORY_H
