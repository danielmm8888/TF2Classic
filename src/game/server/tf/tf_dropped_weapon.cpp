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
	m_pWeaponInfo = NULL;
	m_iMaxAmmo = 0;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFDroppedWeapon::Spawn( void )
{
	m_pWeaponInfo = GetTFWeaponInfo( m_nWeaponID );

	m_iMaxAmmo = m_pWeaponInfo->m_WeaponData[TF_WEAPON_PRIMARY_MODE].m_iMaxAmmo;

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
	// Remove entity after 15 seconds
	if ( gpGlobals->curtime > m_flRemoveTime )
		UTIL_Remove( this );

	SetNextThink( gpGlobals->curtime + 1.5f );
}

CTFDroppedWeapon *CTFDroppedWeapon::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, const char *pszModelName, unsigned int nWeaponID )
{
	CTFDroppedWeapon *pDroppedWeapon = static_cast<CTFDroppedWeapon*>( CBaseAnimating::CreateNoSpawn( "tf_dropped_weapon", vecOrigin, vecAngles, pOwner ) );
	if ( pDroppedWeapon )
	{
		pDroppedWeapon->SetModelName( AllocPooledString( pszModelName ) );
		pDroppedWeapon->SetWeaponID( nWeaponID );
		DispatchSpawn( pDroppedWeapon );
	}

	return pDroppedWeapon;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFDroppedWeapon::ValidTouch( CBaseEntity *pPlayer )
{
	// Only touch a live player.
	if ( !pPlayer || !pPlayer->IsPlayer() || !pPlayer->IsAlive() )
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

	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>( pPlayer );

	if ( ValidTouch( pTFPlayer ) && pTFPlayer->IsPlayerClass( TF_CLASS_MERCENARY ) )
	{
		// Don't remove weapon while a player is standing over it.
		SetThink( NULL );

#ifndef DM_WEAPON_BUCKET
		CTFWeaponBase *pWeapon = (CTFWeaponBase *)pTFPlayer->Weapon_GetSlot( GetTFWeaponInfo( m_nWeaponID )->iSlot );
		const char *pszWeaponName = WeaponIdToClassname( m_nWeaponID );
		int iAmmoType = GetTFWeaponInfo( m_nWeaponID )->iAmmoType;

		if ( pWeapon )
		{
			if ( pWeapon->GetWeaponID() == m_nWeaponID )
			{
				// Give however many ammo we have
				if ( pTFPlayer->GiveAmmo( m_iAmmo, GetTFWeaponInfo( m_nWeaponID )->iAmmoType ) )
					bSuccess = true;
			}
			else if ( !(pTFPlayer->m_nButtons & IN_ATTACK) && ( pTFPlayer->m_nButtons & IN_USE ) )
			{
				// Drop a usable weapon
				pTFPlayer->DropWeapon( pWeapon );

				// Check Use button
				if ( pWeapon == pTFPlayer->GetActiveTFWeapon() )
				{
					pWeapon->Holster();
				}
				pTFPlayer->Weapon_Detach( pWeapon );
				UTIL_Remove( pWeapon );
				pWeapon = NULL;
			}
			else
			{
				pTFPlayer->m_Shared.SetDesiredWeaponIndex( m_nWeaponID );
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
			CTFWeaponBase *pNewWeapon = (CTFWeaponBase *)pTFPlayer->GiveNamedItem( pszWeaponName );
			if ( pNewWeapon )
			{
				pNewWeapon->DefaultTouch( pPlayer );
				pPlayer->SetAmmoCount( m_iAmmo, iAmmoType );
				if ( pPlayer == GetOwnerEntity() )
				{
					// If this is the same guy who dropped it restore old clip size to avoid exploiting swapping
					// weapons for faster reload.
					pNewWeapon->m_iClip1 = m_iClip;
				}
				pTFPlayer->m_Shared.SetDesiredWeaponIndex( TF_WEAPON_NONE );
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
		pTFPlayer->m_Shared.SetDesiredWeaponIndex( TF_WEAPON_NONE );
		SetThink( &CTFDroppedWeapon::RemovalThink );
		// Don't remove weapon immediately after player stopped touching it.
		SetNextThink( gpGlobals->curtime + 3.5f );
	}
}
