//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "tf_ammo_pack.h"
#include "tf_shareddefs.h"
#include "ammodef.h"
#include "tf_gamerules.h"
#include "explode.h"
#include "tf_powerup.h"
#include "entity_ammopack.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//----------------------------------------------

// Network table.
IMPLEMENT_SERVERCLASS_ST( CTFAmmoPack, DT_AmmoPack )
	SendPropVector( SENDINFO( m_vecInitialVelocity ), -1, SPROP_NOSCALE ),
END_SEND_TABLE()

BEGIN_DATADESC( CTFAmmoPack )
	DEFINE_THINKFUNC( FlyThink ),
	DEFINE_ENTITYFUNC( PackTouch ),
END_DATADESC();

LINK_ENTITY_TO_CLASS( tf_ammo_pack, CTFAmmoPack );

PRECACHE_REGISTER( tf_ammo_pack );

void CTFAmmoPack::Spawn( void )
{
	Precache();
	SetModel( STRING( GetModelName() ) );
	BaseClass::Spawn();

	SetNextThink( gpGlobals->curtime + 0.75f );
	SetThink( &CTFAmmoPack::FlyThink );

	SetTouch( &CTFAmmoPack::PackTouch );

	m_flCreationTime = gpGlobals->curtime;

	// no pickup until flythink
	m_bAllowOwnerPickup = false;

	// no ammo to start
	memset( m_iAmmo, 0, sizeof(m_iAmmo) );

	// Die in 30 seconds
	SetContextThink( &CBaseEntity::SUB_Remove, gpGlobals->curtime + 30, "DieContext" );

	if ( IsX360() )
	{
		RemoveEffects( EF_ITEM_BLINK );
	}
}

void CTFAmmoPack::Precache( void )
{
	PrecacheScriptSound( TF_AMMOPACK_PICKUP_SOUND );
}

CTFAmmoPack *CTFAmmoPack::Create( const Vector &vecOrigin, const QAngle &vecAngles, CBaseEntity *pOwner, const char *pszModelName )
{
	CTFAmmoPack *pAmmoPack = static_cast<CTFAmmoPack*>( CBaseAnimating::CreateNoSpawn( "tf_ammo_pack", vecOrigin, vecAngles, pOwner ) );
	if ( pAmmoPack )
	{
		pAmmoPack->SetModelName( AllocPooledString( pszModelName ) );
		DispatchSpawn( pAmmoPack );
	}

	return pAmmoPack;
}

void CTFAmmoPack::SetInitialVelocity( Vector &vecVelocity )
{ 
	m_vecInitialVelocity = vecVelocity;
}

int CTFAmmoPack::GiveAmmo( int iCount, int iAmmoType )
{
	if (iAmmoType == -1 || iAmmoType >= TF_AMMO_COUNT )
	{
		Msg("ERROR: Attempting to give unknown ammo type (%d)\n", iAmmoType);
		return 0;
	}

	m_iAmmo[iAmmoType] += iCount;

	return iCount;
}

void CTFAmmoPack::FlyThink( void )
{
	m_bAllowOwnerPickup = true;
}

void CTFAmmoPack::PackTouch( CBaseEntity *pOther )
{
	Assert( pOther );

	if( !pOther->IsPlayer() )
		return;

	if( !pOther->IsAlive() )
		return;

	//Don't let the person who threw this ammo pick it up until it hits the ground.
	//This way we can throw ammo to people, but not touch it as soon as we throw it ourselves
	if( GetOwnerEntity() == pOther && m_bAllowOwnerPickup == false )
		return;

	CBasePlayer *pPlayer = ToBasePlayer( pOther );

	Assert( pPlayer );

	// tf_ammo_pack (dropped weapons) originally packed killed player's ammo.
	// This was changed to make them act as medium ammo packs.
#if 0
	// Old ammo giving code.
	int iAmmoTaken = 0;

	int i;
	for ( i=0;i<TF_AMMO_COUNT;i++ )
	{
		iAmmoTaken += pPlayer->GiveAmmo( m_iAmmo[i], i );
	}

	if ( iAmmoTaken > 0 )
	{
		UTIL_Remove( this );
	}
#else
	// Copy-paste from CAmmoPack code.

	bool bSuccess = false;

	CTFPlayer *pTFPlayer = ToTFPlayer( pPlayer );
	if ( !pTFPlayer )
		return;

	int iMaxPrimary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_PRIMARY];
	if ( pPlayer->GiveAmmo( ceil(iMaxPrimary * PackRatios[POWERUP_MEDIUM]), TF_AMMO_PRIMARY, true ) )
	{
		bSuccess = true;
	}

	int iMaxSecondary = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_SECONDARY];
	if ( pPlayer->GiveAmmo( ceil(iMaxSecondary * PackRatios[POWERUP_MEDIUM]), TF_AMMO_SECONDARY, true ) )
	{
		bSuccess = true;
	}

	int iMaxMetal = pTFPlayer->GetPlayerClass()->GetData()->m_aAmmoMax[TF_AMMO_METAL];
	if ( pPlayer->GiveAmmo( ceil(iMaxMetal * PackRatios[POWERUP_MEDIUM]), TF_AMMO_METAL, true ) )
	{
		bSuccess = true;
	}

	if (pTFPlayer->m_Shared.GetSpyCloakMeter() < 100.0f)
	{
		pTFPlayer->m_Shared.SetSpyCloakMeter(min(100.0f, pTFPlayer->m_Shared.GetSpyCloakMeter() + ceil(100.0f * PackRatios[POWERUP_MEDIUM])));
		bSuccess = true;
	}

	// did we give them anything?
	if ( bSuccess )
	{
		CSingleUserRecipientFilter filter( pPlayer );
		EmitSound( filter, entindex(), TF_AMMOPACK_PICKUP_SOUND );
		UTIL_Remove( this );
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
unsigned int CTFAmmoPack::PhysicsSolidMaskForEntity( void ) const
{ 
	return BaseClass::PhysicsSolidMaskForEntity() | CONTENTS_DEBRIS;
}