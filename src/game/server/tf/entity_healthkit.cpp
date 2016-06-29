//====== Copyright � 1996-2005, Valve Corporation, All rights reserved. =======//
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
extern ConVar tf2c_dm_max_health_boost;

LINK_ENTITY_TO_CLASS( item_healthkit_full, CHealthKit );
LINK_ENTITY_TO_CLASS( item_healthkit_small, CHealthKitSmall );
LINK_ENTITY_TO_CLASS( item_healthkit_medium, CHealthKitMedium );
LINK_ENTITY_TO_CLASS( item_healthkit_tiny, CHealthKitTiny );

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
		int iHealthToAdd = ceil( pPlayer->GetMaxHealth() * PackRatios[GetPowerupSize()] );
		bool bTiny = GetPowerupSize() == POWERUP_TINY;
		int iHealthRestored = 0;

		CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
		Assert( pTFPlayer );

		// Overheal pellets, well, overheal.
		if ( bTiny )
		{
			iHealthToAdd = clamp( iHealthToAdd, 0, pTFPlayer->m_Shared.GetMaxBuffedHealth() - pTFPlayer->GetHealth() );
			iHealthRestored = pPlayer->TakeHealth( iHealthToAdd, DMG_IGNORE_MAXHEALTH );
		}
		else
		{
			iHealthRestored = pPlayer->TakeHealth( iHealthToAdd, DMG_GENERIC );
		}

		if ( iHealthRestored )
			bSuccess = true;

		// Restore disguise health.
		if ( pTFPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
		{
			int iFakeHealthToAdd = ceil( pTFPlayer->m_Shared.GetDisguiseMaxHealth() * PackRatios[GetPowerupSize()] );
			if ( pTFPlayer->m_Shared.AddDisguiseHealth( iFakeHealthToAdd, bTiny ) )
				bSuccess = true;
		}

		if ( !bTiny )
		{
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
		}

		if ( bSuccess )
		{
			CSingleUserRecipientFilter user( pPlayer );
			user.MakeReliable();

			UserMessageBegin( user, "ItemPickup" );
			WRITE_STRING( GetClassname() );
			MessageEnd();

			const char *pszSound = TF_HEALTHKIT_PICKUP_SOUND;

			if ( bTiny )
			{
				if ( pPlayer->GetHealth() > pPlayer->GetMaxHealth() )
					pszSound = "OverhealPillRattle.Touch";
				else
					pszSound = "OverhealPillNoRattle.Touch";
			}

			EmitSound( user, entindex(), pszSound );

			// Disabled for overheal pills since they'll cause too much spam.
			if ( iHealthRestored && !bTiny )
			{
				IGameEvent *event = gameeventmanager->CreateEvent( "player_healonhit" );

				if ( event )
				{
					event->SetInt( "amount", iHealthRestored );
					event->SetInt( "entindex", pPlayer->entindex() );

					gameeventmanager->FireEvent( event );
				}
			}
		}
	}

	return bSuccess;
}
