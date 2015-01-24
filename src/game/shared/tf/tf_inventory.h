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

#define INVENTORY_SLOTS			6
#define INVENTORY_WEAPONS		5

class CTFInventory
{
public:
	CTFInventory();

	int GetWeapon(int iClass, int iSlot, int iNum){
		return Weapons[iClass][iSlot][iNum];
	};
	
private:
	static const int Weapons[TF_CLASS_COUNT_ALL][INVENTORY_SLOTS][INVENTORY_WEAPONS];
};

#endif // TF_SHAREDDEFS_H
