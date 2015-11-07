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

BEGIN_DATADESC( CTFDroppedWeapon )
	DEFINE_ENTITYFUNC( WeaponTouch )
END_DATADESC()

IMPLEMENT_NETWORKCLASS_ALIASED( TFDroppedWeapon, DT_TFDroppedWeapon )

BEGIN_SEND_TABLE( CTFDroppedWeapon, DT_TFDroppedWeapon )
END_SEND_TABLE()

LINK_ENTITY_TO_CLASS( tf_dropped_weapon, CTFDroppedWeapon );

CTFDroppedWeapon::CTFDroppedWeapon()
{

}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void CTFDroppedWeapon::Spawn( void )
{
	BaseClass::Spawn();
	
	SetCollisionGroup( COLLISION_GROUP_DEBRIS );
	SetSolidFlags( FSOLID_TRIGGER );

	m_flCreationTime = gpGlobals->curtime;

	// Remove 30s after spawning
	m_flRemoveTime = gpGlobals->curtime + 30.0f;
	SetTouch( &CTFDroppedWeapon::WeaponTouch );
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
void CTFDroppedWeapon::WeaponTouch( CBaseEntity *pEntity )
{
	CTFPlayer *pTFPlayer = dynamic_cast<CTFPlayer*>( pEntity );

	if ( ValidTouch( pTFPlayer ) && pTFPlayer->IsPlayerClass( TF_CLASS_MERCENARY ) )
	{
		// Don't remove weapon while a player is standing over it.
		SetThink( NULL );
#ifndef DM_WEAPON_BUCKET
		CTFWeaponBase *pWeapon = (CTFWeaponBase *)pTFPlayer->Weapon_GetSlot( GetTFWeaponInfo( m_nWeaponID )->iSlot );

		if ( pWeapon )
		{
			if ( pWeapon->GetWeaponID() == m_nWeaponID )
			{
				pTFPlayer->GiveAmmo( 999, GetTFWeaponInfo( m_nWeaponID )->iAmmoType );
				UTIL_Remove( this );
				return;
			}
			else if ( !(pTFPlayer->m_nButtons & IN_ATTACK) && ( pTFPlayer->m_nButtons & IN_USE ) )
			{
				// Spawn a weapon model.
				pTFPlayer->DropFakeWeapon( pWeapon );

				// Check Use button
				pWeapon->Holster();
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
			pTFPlayer->GiveAmmo( 999, GetTFWeaponInfo( m_nWeaponID )->iAmmoType );
			UTIL_Remove( this );
			return;
		}
#endif
		if ( !pWeapon )
		{
			pTFPlayer->GiveNamedItem( WeaponIdToAlias( m_nWeaponID ) );
			pTFPlayer->m_Shared.SetDesiredWeaponIndex( TF_WEAPON_NONE );
			UTIL_Remove( this );
		}
	}
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




