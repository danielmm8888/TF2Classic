//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Deathmatch weapon spawning entity.
//
//=============================================================================//
#include "cbase.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_weaponspawn.h"
#include "tf_weaponbase.h"
#include "basecombatcharacter.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern CTFWeaponInfo *GetTFWeaponInfo(int iWeapon);

// We don't have a proper sound yet, so we're using this
#define TF_HEALTHKIT_PICKUP_SOUND	"HealthKit.Touch"

BEGIN_DATADESC(CWeaponSpawner)

	DEFINE_KEYFIELD(m_iWeaponNumber, FIELD_INTEGER, "WeaponNumber"),
	DEFINE_KEYFIELD(m_iRespawnTime, FIELD_INTEGER, "RespawnTime"),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST(CWeaponSpawner, DT_WeaponSpawner)
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS(tf_weaponspawner, CWeaponSpawner);


CWeaponSpawner::CWeaponSpawner()
{
	m_iWeaponNumber = TF_WEAPON_SHOTGUN_SOLDIER;
	m_iRespawnTime = 10;
	m_hUser = NULL;
}


//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Spawn(void)
{
	pWeaponInfo = GetTFWeaponInfo(m_iWeaponNumber);
	if ( !pWeaponInfo )
	{
		Warning( "tf_weaponspawner has incorrect weapon number %d \n", m_iWeaponNumber );
		UTIL_Remove( this );
		return;
	}

	Precache();

	SetModel(pWeaponInfo->szWorldModel);
	BaseClass::Spawn();
	// Ensures consistent BBOX size for all weapons. (danielmm8888)
	SetSolid( SOLID_BBOX );
	SetCollisionBounds( -Vector(22, 22, 15), Vector(22, 22, 15) );
}

float CWeaponSpawner::GetRespawnDelay(void)
{
	return (float)m_iRespawnTime;
}

//-----------------------------------------------------------------------------
// Purpose: Precache function 
//-----------------------------------------------------------------------------
void CWeaponSpawner::Precache(void)
{
	PrecacheScriptSound(TF_HEALTHKIT_PICKUP_SOUND);
}

void CWeaponSpawner::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );

	if ( pPlayer )
	{
		m_hUser = pPlayer;
		ItemTouch( pActivator );
		m_hUser = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CWeaponSpawner::MyTouch(CBasePlayer *pPlayer)
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>(pPlayer);

	if (ValidTouch(pTFPlayer) && pTFPlayer->IsPlayerClass(TF_CLASS_MERCENARY))
	{
		CTFWeaponBase *pWeapon = pTFPlayer->Weapon_GetWeaponByType( pWeaponInfo->m_iWeaponType );
		const char *pszWeaponName = WeaponIdToAlias( m_iWeaponNumber );

		if (pWeapon)
		{
			if (pWeapon->GetWeaponID() == m_iWeaponNumber)
			{
				if ( pPlayer->GiveAmmo(999, pWeaponInfo->iAmmoType) )
					bSuccess = true;
			}
			else if ( pPlayer == m_hUser )
			{
				// Only dump current weapon if player +USE'd us.
				pTFPlayer->Weapon_Detach(pWeapon);
				UTIL_Remove(pWeapon);
				pWeapon = NULL;
			}
		}

		if ( !pWeapon )
		{
			pTFPlayer->GiveNamedItem( pszWeaponName );
			bSuccess = true;
		}

		if ( bSuccess )
		{
			CSingleUserRecipientFilter user(pPlayer);
			user.MakeReliable();

			UserMessageBegin(user, "ItemPickup");
			WRITE_STRING(GetClassname());
			MessageEnd();

			//EmitSound(user, entindex(), TF_HEALTHKIT_PICKUP_SOUND);
		}
	}

	return bSuccess;
}
