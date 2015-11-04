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

CTFDroppedWeapon *CTFDroppedWeapon::Create( const Vector &vecOrigin, const QAngle &vecAngles, const char *pszModelName, unsigned int nWeaponID )
{
	CTFDroppedWeapon *pDroppedWeapon = static_cast<CTFDroppedWeapon*>( CBaseAnimating::CreateNoSpawn( "tf_dropped_weapon", vecOrigin, vecAngles ) );
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
		SetThink( NULL );
#ifndef DM_WEAPON_BUCKET
		CTFWeaponBase *pWeapon = pTFPlayer->Weapon_GetWeaponByBucket( GetTFWeaponInfo( m_nWeaponID )->iSlot );

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
				pTFPlayer->Weapon_Detach( pWeapon );
				pWeapon->WeaponReset();
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
		SetNextThink( gpGlobals->curtime + 3.5f );
	}
}




