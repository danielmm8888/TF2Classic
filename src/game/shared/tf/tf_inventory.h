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
#include "igamesystem.h"
#include "tf_shareddefs.h"
#if defined( CLIENT_DLL )
#include "c_tf_player.h"
#include "filesystem.h" 
#endif

#define INVENTORY_WEAPONS		5
#define INVENTORY_WEAPONS_COUNT	500
#define INVENTORY_COLNUM		5
#define INVENTORY_ROWNUM		3
#define INVENTORY_VECTOR_NUM	INVENTORY_COLNUM * INVENTORY_ROWNUM

class CTFInventory : public CAutoGameSystemPerFrame
{
public:
	CTFInventory();
	~CTFInventory();

	virtual char const *Name() { return "CTFInventory"; }

	virtual bool	Init( void );
	virtual void	LevelInitPreEntity( void );

	int				GetWeapon( int iClass, int iSlot );
	CEconItemView	*GetItem( int iClass, int iSlot, int iNum );
	bool			CheckValidSlot( int iClass, int iSlot, bool bHudCheck = false );
	bool			CheckValidWeapon( int iClass, int iSlot, int iWeapon, bool bHudCheck = false );
	int				NumWeapons( int iClass, int iSlot );

#if defined( CLIENT_DLL )
	int				GetWeaponPreset( int iClass, int iSlot );
	void			SetWeaponPreset( int iClass, int iSlot, int iPreset );
	const char		*GetSlotName( int iSlot );
#endif

private:
	static const int			Weapons[TF_CLASS_COUNT_ALL][TF_PLAYER_WEAPON_COUNT];
	CUtlVector<CEconItemView *>	m_Items[TF_CLASS_COUNT_ALL][TF_LOADOUT_SLOT_COUNT];

#if defined( CLIENT_DLL )
	void LoadInventory();
	void ResetInventory();
	void SaveInventory();
	KeyValues* m_pInventory;
#endif
};

CTFInventory *GetTFInventory();

#endif // TF_INVENTORY_H
