//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: CTF HealthKit.
//
//=============================================================================//
#include "cbase.h"
#include "items.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "tf_player.h"
#include "tf_team.h"
#include "engine/IEngineSound.h"
#include "entity_healthkit.h"

//=============================================================================
//
// CTF HealthKit defines.
//

#define TF_HEALTHKIT_MODEL			"models/items/healthkit.mdl"
#define TF_HEALTHKIT_PICKUP_SOUND	"HealthKit.Touch"
#define TF_HEALTHKIT_TINY_SOUND		"HealthKit.Touch" // TODO unique short sound

extern ConVar tf_max_health_boost;

LINK_ENTITY_TO_CLASS( item_healthkit_full, CHealthKit );
LINK_ENTITY_TO_CLASS( item_healthkit_small, CHealthKitSmall );
LINK_ENTITY_TO_CLASS( item_healthkit_medium, CHealthKitMedium );
LINK_ENTITY_TO_CLASS( item_healthkit_tiny, CHealthKitTiny);

//=============================================================================
//
// CTF HealthKit functions.
//

//-----------------------------------------------------------------------------
// Purpose: Spawn function for the healthkit
//-----------------------------------------------------------------------------
void CHealthKit::Spawn( void )
{
	Precache();
	SetModel( GetPowerupModel() );

	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Precache function for the healthkit
//-----------------------------------------------------------------------------
void CHealthKit::Precache( void )
{
	PrecacheModel( GetPowerupModel() );
	PrecacheScriptSound( TF_HEALTHKIT_PICKUP_SOUND );
}

//-----------------------------------------------------------------------------
// Purpose: MyTouch function for the healthkit
//-----------------------------------------------------------------------------
bool CHealthKit::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	if ( ValidTouch( pPlayer ) )
	{
		if (GetPowerupSize() == POWERUP_TINY) // TF2C tiny medkit, overheals. can't pick up if hp would exceed max
		{
			if (pPlayer->GetHealth() < pPlayer->GetMaxHealth() * (tf_max_health_boost.GetFloat() - PackRatios[GetPowerupSize()]))
			{
				if (pPlayer->TakeHealth(ceil(pPlayer->GetMaxHealth() * PackRatios[GetPowerupSize()]), DMG_IGNORE_MAXHEALTH))
				{
					CSingleUserRecipientFilter user(pPlayer);
					user.MakeReliable();

					UserMessageBegin(user, "ItemPickup");
					WRITE_STRING(GetClassname());
					MessageEnd();

					EmitSound(user, entindex(), TF_HEALTHKIT_TINY_SOUND);

					bSuccess = true;
				}
			}
		}
		else if ( pPlayer->TakeHealth( ceil(pPlayer->GetMaxHealth() * PackRatios[GetPowerupSize()]), DMG_GENERIC ) )
		{
			CSingleUserRecipientFilter user( pPlayer );
			user.MakeReliable();

			UserMessageBegin( user, "ItemPickup" );
				WRITE_STRING( GetClassname() );
			MessageEnd();

			EmitSound( user, entindex(), TF_HEALTHKIT_PICKUP_SOUND );

			bSuccess = true;

			CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );

			Assert( pTFPlayer );

			// Healthkits also contain a fire blanket.
			if ( pTFPlayer->m_Shared.InCond( TF_COND_BURNING ) )
			{
				pTFPlayer->m_Shared.RemoveCond( TF_COND_BURNING );		
			}

			// Remove tranq condition
			if ( pTFPlayer->m_Shared.InCond( TF_COND_SLOWED ) )
			{
				pTFPlayer->m_Shared.RemoveCond( TF_COND_SLOWED );
			}
		}
	}

	return bSuccess;
}
