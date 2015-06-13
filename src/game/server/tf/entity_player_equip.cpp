//====== Copyright Valve Corporation, All rights reserved. =======//
//
// Purpose: A modified game_player_equip that works with TF2C
//
//=============================================================================//

#include "cbase.h"
#include "tf_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define SF_PLAYEREQUIP_USEONLY			0x0001

class CTFPlayerEquip : public CBaseEntity
{
	DECLARE_DATADESC();

public:
	DECLARE_CLASS(CTFPlayerEquip, CBaseEntity);

	void		Touch(CBaseEntity *pOther);
	void		Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	inline bool	UseOnly(void) { return (m_spawnflags & SF_PLAYEREQUIP_USEONLY) ? true : false; }

	void		InputEquipWeapons(inputdata_t &inputdata);

private:

	void		EquipPlayer(CBaseEntity *pPlayer);
	int			m_iWeaponNumber[TF_PLAYER_WEAPON_COUNT];
	bool		m_bStripWeapons;

};

LINK_ENTITY_TO_CLASS(tf_player_equip, CTFPlayerEquip);

//---------------------------------------------------------
// Save/Restore
//---------------------------------------------------------
BEGIN_DATADESC(CTFPlayerEquip)

	DEFINE_KEYFIELD(m_bStripWeapons, FIELD_BOOLEAN, "stripweapons"),

	DEFINE_KEYFIELD(m_iWeaponNumber[0], FIELD_INTEGER, "weapon1"),
	DEFINE_KEYFIELD(m_iWeaponNumber[1], FIELD_INTEGER, "weapon2"),
	DEFINE_KEYFIELD(m_iWeaponNumber[2], FIELD_INTEGER, "weapon3"),
	DEFINE_KEYFIELD(m_iWeaponNumber[3], FIELD_INTEGER, "weapon4"),
	DEFINE_KEYFIELD(m_iWeaponNumber[4], FIELD_INTEGER, "weapon5"),

	DEFINE_INPUTFUNC(FIELD_VOID, "EquipWeapons", InputEquipWeapons),

END_DATADESC()

void CTFPlayerEquip::Touch(CBaseEntity *pOther)
{
	if (UseOnly())
		return;

	EquipPlayer(pOther);
}

void CTFPlayerEquip::EquipPlayer(CBaseEntity *pEntity)
{
	CTFPlayer *pTFPlayer = ToTFPlayer(pEntity);

	if (!pTFPlayer)
		return;

	for (int i = 0; i < TF_PLAYER_WEAPON_COUNT; i++)
	{
		if (m_bStripWeapons)
		{
			CTFWeaponBase *pWeapon = pTFPlayer->Weapon_GetWeaponByType(i);
			if (pWeapon)
			{
				pTFPlayer->Weapon_Detach(pWeapon);
				pTFPlayer->SwitchToNextBestWeapon(NULL);
				UTIL_Remove(pWeapon);
			}
		}

		if (m_iWeaponNumber[i])
		{
			if (!m_bStripWeapons)
			{
				CTFWeaponBase *pWeapon = pTFPlayer->Weapon_GetWeaponByType(GetTFWeaponInfo(m_iWeaponNumber[i])->m_iWeaponType);
				if (pWeapon)
				{
					pTFPlayer->Weapon_Detach(pWeapon);
					pTFPlayer->SwitchToNextBestWeapon(NULL);
					UTIL_Remove(pWeapon);
				}
			}

			pTFPlayer->GiveNamedItem(GetTFWeaponInfo(m_iWeaponNumber[i])->szClassName);
		}
	}
}

void CTFPlayerEquip::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	EquipPlayer(pActivator);
}

/*
	Hammer input that equips all of the players on the current server with the specified weapons
*/
void CTFPlayerEquip::InputEquipWeapons(inputdata_t &inputdata)
{
	for (int i = 1; i <= gpGlobals->maxClients; i++)
	{
		EquipPlayer(UTIL_PlayerByIndex(i));
	}
}
