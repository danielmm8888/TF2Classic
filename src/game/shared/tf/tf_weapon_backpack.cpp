//======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_backpack.h"
#include "decals.h"

// Client specific.
#ifdef CLIENT_DLL
#include "c_tf_player.h"
// Server specific.
#else
#include "tf_player.h"
#include "tf_powerup.h"
#endif

//=============================================================================
//
// Weapon Backpack tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFBackpack, DT_TFWeaponBackpack )

BEGIN_NETWORK_TABLE( CTFBackpack, DT_TFWeaponBackpack )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFBackpack )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_Backpack, CTFBackpack );
PRECACHE_WEAPON_REGISTER( tf_weapon_Backpack );

//=============================================================================
//
// Weapon Backpack functions.
//

const char *CTFBackpack::HealthEntities[3] =
{
	"item_healthkit_small",
	"item_healthkit_medium",
	"item_healthkit_full"
};

const char *CTFBackpack::AmmoEntities[3] =
{
	"item_ammopack_small",
	"item_ammopack_medium",
	"item_ammopack_full"
};


const char *CTFBackpack::GetStatus(void)
{
	if (cEntityName[0] == '\0')
		return "Empty";
	else
		return "Full";
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
CTFBackpack::CTFBackpack()
{
	cEntityName = "";
	iEntityType = -1;
	iEntitySize = -1;
}

void  CTFBackpack::AddNewEntityByType(int iType, int iSize)
{
	const char* cEntity;
	switch (iType)
	{
	case HealthKit:
		cEntity = HealthEntities[iSize];
		break;
	case AmmoKit:
		cEntity = AmmoEntities[iSize];
		break;
	default:
		cEntity = "";
		break;
	}
#ifdef GAME_DLL
	CTFPlayer *pPlayer = ToTFPlayer(GetPlayerOwner());
	pPlayer->m_Shared.SetBackpackEntity(iType, iSize);
#endif
	iEntityType = iType;
	iEntitySize = iSize;
	AddNewEntity(cEntity);
}

void CTFBackpack::PrimaryAttack()
{
	if (cEntityName[0] != '\0')
	{
#ifdef GAME_DLL
		CTFPowerup *pEnt = (CTFPowerup*)CreateEntityByName(cEntityName);
		if (!pEnt)
			return;
		Vector vecForward; // Sets up a vector.
		AngleVectors(EyeAngles(), &vecForward); // Sets up a angle vector and gets the eyeangles of the player.
		Vector vecOrigin = GetAbsOrigin() + vecForward * 56 + Vector(0, 0, 64); // Makes sure the entity spawns a view unites away from the player.
		QAngle vecAngles(0, GetAbsAngles().y - 90, 0); // Changes the angle of the entity.
		pEnt->SetAbsOrigin(vecOrigin); // Apply origin changes.
		pEnt->SetAbsAngles(vecAngles); // Apply angle changes.
		pEnt->SetBaseVelocity(Vector(0, 200, -128));
		pEnt->SetRespawnDelay(-1); // Spawns the entity.
		pEnt->Spawn();
#endif
		cEntityName = "";
#ifdef GAME_DLL
		CTFPlayer *pPlayer = ToTFPlayer(GetPlayerOwner());
		pPlayer->m_Shared.SetBackpackEntity(-1, -1);
#endif
		iEntityType = -1;
		iEntitySize = -1;
	}
};
