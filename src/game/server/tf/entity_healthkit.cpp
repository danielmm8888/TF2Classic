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
#include "tf_gamestats.h"

//=============================================================================
//
// CTF HealthKit defines.
//

#define TF_HEALTHKIT_MODEL			"models/items/healthkit.mdl"
#define TF_HEALTHKIT_PICKUP_SOUND	"HealthKit.Touch"

LINK_ENTITY_TO_CLASS( item_healthkit_full, CHealthKit );
LINK_ENTITY_TO_CLASS( item_healthkit_small, CHealthKitSmall );
LINK_ENTITY_TO_CLASS( item_healthkit_medium, CHealthKitMedium );
LINK_ENTITY_TO_CLASS( item_healthkit_tiny, CHealthKitTiny );
LINK_ENTITY_TO_CLASS( item_healthkit_mega, CHealthKitMega );

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
		CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
		Assert( pTFPlayer );

		int iHealthToAdd = ceil( pPlayer->GetMaxHealth() * PackRatios[GetPowerupSize()] );
		bool bTiny = GetPowerupSize() == POWERUP_TINY;
		bool bMega = GetPowerupSize() == POWERUP_MEGA;
		int iHealthRestored = 0;

		// Don't heal the player who dropped this healthkit.
		if ( pTFPlayer != GetOwnerEntity() )
		{
			// Overheal pellets, well, overheal.
			if ( bTiny || bMega )
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
				if ( pTFPlayer->m_Shared.AddDisguiseHealth( iFakeHealthToAdd, ( bTiny || bMega ) ) )
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

			CTFPlayer *pTFOwner = ToTFPlayer( GetOwnerEntity() );
			if ( pTFOwner && pTFOwner->InSameTeam( pTFPlayer ) )
			{
				// BONUS DUCKS!
				CTF_GameStats.Event_PlayerAwardBonusPoints( pTFOwner, pPlayer, 1 );
			}

			// Disabled for overheal pills since they'll cause too much spam.
			if ( iHealthRestored && !bTiny )
			{
				IGameEvent *event_healonhit = gameeventmanager->CreateEvent( "player_healonhit" );
				if ( event_healonhit )
				{
					event_healonhit->SetInt( "amount", iHealthRestored );
					event_healonhit->SetInt( "entindex", pPlayer->entindex() );

					gameeventmanager->FireEvent( event_healonhit );
				}

				// Show healing to the one who dropped the healthkit.
				CBasePlayer *pOwner = ToBasePlayer( GetOwnerEntity() );
				if ( pOwner )
				{
					IGameEvent *event_healed = gameeventmanager->CreateEvent( "player_healed" );
					if ( event_healed )
					{
						event_healed->SetInt( "patient", pPlayer->GetUserID() );
						event_healed->SetInt( "healer", pOwner->GetUserID() );
						event_healed->SetInt( "amount", iHealthRestored );

						gameeventmanager->FireEvent( event_healed );
					}
				}
			}
		}
		else
		{
			// Recharge lunchbox if player's at full health.
			CTFWeaponBase *pLunch = pTFPlayer->Weapon_OwnsThisID( TF_WEAPON_LUNCHBOX );
			if ( pLunch && pLunch->GetEffectBarProgress() < 1.0f )
			{
				pLunch->EffectBarRegenFinished();
				bSuccess = true;
			}
		}
	}

	return bSuccess;
}
