//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Critical Damage: Crits for 30/15 seconds with a crit glow. 
//			Crit glow effect would match the player’s merc color or be blue.
//	  TODO: Add unique model and sound effect. Health kit assets as placeholder.
//
//=============================================================================//
#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "tf_powerup_critdamage.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//=============================================================================

#define TF_HEALTHKIT_PICKUP_SOUND	"HealthKit.Touch"

LINK_ENTITY_TO_CLASS(item_powerup_critdamage, CTFPowerupCritdamage);

//=============================================================================

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFPowerupCritdamage::Spawn(void)
{
	Precache();
	SetModel( GetPowerupModel() );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache 
//-----------------------------------------------------------------------------
void CTFPowerupCritdamage::Precache(void)
{
	PrecacheModel( GetPowerupModel() );
	PrecacheScriptSound(TF_HEALTHKIT_PICKUP_SOUND);
}

//-----------------------------------------------------------------------------
// Purpose: Touch function
//-----------------------------------------------------------------------------
bool CTFPowerupCritdamage::MyTouch(CBasePlayer *pPlayer)
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>(pPlayer);
	if  (pTFPlayer && ValidTouch(pPlayer) )
	{
		pTFPlayer->m_Shared.AddCond( TF_COND_POWERUP_CRITDAMAGE, 15.0f );

		CSingleUserRecipientFilter user(pPlayer);
		user.MakeReliable();

		UserMessageBegin(user, "ItemPickup");
		WRITE_STRING(GetClassname());
		MessageEnd();

		EmitSound(user, entindex(), TF_HEALTHKIT_PICKUP_SOUND);

		bSuccess = true;
	}

	return bSuccess;
}
