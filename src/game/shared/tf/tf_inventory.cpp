//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: Simple Inventory
// by MrModez
// $NoKeywords: $
//=============================================================================//


#include "cbase.h"
#include "server_class.h"
#include "tf_playeranimstate.h"
#include "tf_shareddefs.h"
#include "tf_inventory.h"

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
				TF_WEAPON_KNIFE
			},
			{
				TF_WEAPON_REVOLVER, TF_WEAPON_TRANQ
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



CTFInventory::CTFInventory(){};