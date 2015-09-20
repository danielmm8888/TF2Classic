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
	PrecacheScriptSound( "OverhealPillRattle.Touch" );
	PrecacheScriptSound( "OverhealPillNoRattle.Touch" );
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

					if (pPlayer->GetHealth() > pPlayer->GetMaxHealth())
					{
						EmitSound( user, entindex(), "OverhealPillRattle.Touch" );
					}
					else
					{
						EmitSound(user, entindex(), "OverhealPillNoRattle.Touch");
					}

					bSuccess = true;
				}
			}
		}
		else
		{
			if ( pPlayer->TakeHealth( ceil(pPlayer->GetMaxHealth() * PackRatios[GetPowerupSize()]), DMG_GENERIC ) )
				bSuccess = true;

			CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );

			Assert( pTFPlayer );
			
			// Remove any negative conditions whether player got healed or not.
			if ( pTFPlayer->m_Shared.InCond( TF_COND_BURNING ) )
			{
				pTFPlayer->m_Shared.RemoveCond( TF_COND_BURNING );
				bSuccess = true;
			}
			if ( pTFPlayer->m_Shared.InCond( TF_COND_SLOWED ) )
			{
				pTFPlayer->m_Shared.RemoveCond( TF_COND_SLOWED );
				bSuccess = true;
			}

			if ( bSuccess )
			{
				CSingleUserRecipientFilter user( pPlayer );
				user.MakeReliable();

				UserMessageBegin( user, "ItemPickup" );
				WRITE_STRING( GetClassname() );
				MessageEnd();

				EmitSound( user, entindex(), TF_HEALTHKIT_PICKUP_SOUND );
			}
		}
	}

	return bSuccess;
}
