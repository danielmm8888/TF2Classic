//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Dropped DM weapon
//
//=============================================================================//

#include "cbase.h"
#include "tf_dropped_weapon.h"
#include "tf_gamerules.h"
#include "in_buttons.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_SERVERCLASS_ST( CTFDroppedWeapon, DT_TFDroppedWeapon )
	SendPropInt( SENDINFO( m_iAmmo ), 10, SPROP_UNSIGNED ),
	SendPropInt( SENDINFO( m_iMaxAmmo ), 10, SPROP_UNSIGNED ),
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tf_dropped_weapon, CTFDroppedWeapon );

CTFDroppedWeapon::CTFDroppedWeapon()
{
	m_iMaxAmmo = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFDroppedWeapon::Spawn( void )
{
	SetModel( STRING( GetModelName() ) );
	AddSpawnFlags( SF_NORESPAWN );

	BaseClass::Spawn();

	if ( VPhysicsGetObject() )
	{
		// All weapons must have same weight.
		VPhysicsGetObject()->SetMass( 25.0f );
	}

	SetCollisionGroup( COLLISION_GROUP_DEBRIS );

	m_flCreationTime = gpGlobals->curtime;

	// Remove 30s after spawning
	m_flRemoveTime = gpGlobals->curtime + 30.0f;
	SetThink( &CTFDroppedWeapon::RemovalThink );
	SetNextThink( gpGlobals->curtime );
}

void CTFDroppedWeapon::RemovalThink( void )
{
	if ( gpGlobals->curtime >= m_flRemoveTime )
		UTIL_Remove( this );

	SetNextThink( gpGlobals->curtime + 0.1f );
}

CTFDroppedWeapon *CTFDroppedWeapon::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, CTFWeaponBase *pWeapon )
{
	CTFDroppedWeapon *pDroppedWeapon = static_cast<CTFDroppedWeapon *>( CBaseAnimating::CreateNoSpawn( "tf_dropped_weapon", vecOrigin, vecAngles, pOwner ) );
	if ( pDroppedWeapon )
	{
		pDroppedWeapon->SetModelName( pWeapon->GetModelName() );
		pDroppedWeapon->SetItem( pWeapon->GetItem() );

		DispatchSpawn( pDroppedWeapon );
	}

	return pDroppedWeapon;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFDroppedWeapon::ValidTouch( CBasePlayer *pPlayer )
{
	// Only touch a live player.
	if ( !pPlayer || !pPlayer->IsAlive() )
	{
		return false;
	}

	// Dropper can't pick us up for 1 second.
	if ( pPlayer == GetOwnerEntity() && gpGlobals->curtime - m_flCreationTime < 1.0f )
	{
		return false;
	}


	return true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFDroppedWeapon::MyTouch( CBasePlayer *pPlayer )
{
	bool bSuccess = false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );

	if ( ValidTouch( pTFPlayer ) && pTFPlayer->IsPlayerClass( TF_CLASS_MERCENARY ) )
	{
		// Don't remove weapon while a player is standing over it.
		SetThink( NULL );

#ifndef DM_WEAPON_BUCKET
		int iSlot = m_Item.GetStaticData()->GetLoadoutSlot( TF_CLASS_MERCENARY );
		CTFWeaponBase *pWeapon = (CTFWeaponBase *)pTFPlayer->GetEntityForLoadoutSlot( iSlot );

		if ( pWeapon )
		{
			if ( pWeapon->GetItemID() == m_Item.GetItemDefIndex() )
			{
				// Give however many ammo we have.
				if ( pTFPlayer->GiveAmmo( m_iAmmo, pWeapon->GetPrimaryAmmoType(), true, TF_AMMO_SOURCE_AMMOPACK ) )
					bSuccess = true;
			}
			else if ( !( pTFPlayer->m_nButtons & IN_ATTACK ) && ( pTFPlayer->m_nButtons & IN_USE ) ) // Check Use button.
			{
				// Drop a usable weapon.
				pTFPlayer->DropWeapon( pWeapon );

				pWeapon->UnEquip( pTFPlayer );
				pWeapon = NULL;
			}
			else
			{
				pTFPlayer->m_Shared.SetDesiredWeaponIndex( m_Item.GetItemDefIndex() );
			}
		}
#else
		CTFWeaponBase *pWeapon = pTFPlayer->Weapon_OwnsThisID( m_nWeaponID );
		if ( pWeapon )
		{
			if ( pTFPlayer->GiveAmmo( 999, GetTFWeaponInfo( m_nWeaponID )->iAmmoType ) );
			bSuccess = true;
		}
#endif

		if ( !pWeapon )
		{
			const char *pszWeaponName = m_Item.GetEntityName();
			CTFWeaponBase *pNewWeapon = (CTFWeaponBase *)pTFPlayer->GiveNamedItem( pszWeaponName, 0, &m_Item );

			if ( pNewWeapon )
			{
				pPlayer->SetAmmoCount( m_iAmmo, pNewWeapon->GetPrimaryAmmoType() );
				pNewWeapon->GiveTo( pPlayer );

				// If this is the same guy who dropped it restore old clip size to avoid exploiting swapping
				// weapons for faster reload.
				if ( pPlayer == GetOwnerEntity() )
				{
					pNewWeapon->m_iClip1 = m_iClip;
				}

				pTFPlayer->m_Shared.SetDesiredWeaponIndex( -1 );
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

			pPlayer->EmitSound( "BaseCombatCharacter.AmmoPickup" );
		}
	}

	return bSuccess;
}

void CTFDroppedWeapon::EndTouch( CBaseEntity *pOther )
{
	BaseClass::EndTouch( pOther );

	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>( pOther );
	if ( pTFPlayer )
	{
		pTFPlayer->m_Shared.SetDesiredWeaponIndex( -1 );
		SetThink( &CTFDroppedWeapon::RemovalThink );
		// Don't remove weapon immediately after player stopped touching it.
		SetNextThink( gpGlobals->curtime + 3.5f );
	}
}
