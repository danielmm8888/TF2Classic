//=============================================================================
//
// Purpose: MLG
//
//=============================================================================
#include "cbase.h"
#include "tf_weapon_displacer.h"
#include "tf_gamerules.h"
#include "particle_parse.h"

#ifdef GAME_DLL
#include "tf_player.h"
#else
#include "c_tf_player.h"
#endif

//=============================================================================
//
// Weapon Displacer tables.
//
IMPLEMENT_NETWORKCLASS_ALIASED( TFWeaponDisplacer, DT_WeaponDisplacer );
BEGIN_NETWORK_TABLE( CTFWeaponDisplacer, DT_WeaponDisplacer )
#ifdef CLIENT_DLL
	RecvPropTime( RECVINFO( m_flBlastTime ) ),
	RecvPropTime( RECVINFO( m_flTeleportTime ) ),
	RecvPropBool( RECVINFO( m_bLockedOn ) ),
#else
	SendPropTime( SENDINFO( m_flBlastTime ) ),
	SendPropTime( SENDINFO( m_flTeleportTime ) ),
	SendPropBool( SENDINFO( m_bLockedOn ) ),
#endif
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFWeaponDisplacer )
#ifdef CLIENT_DLL
	DEFINE_PRED_FIELD( m_flBlastTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
	DEFINE_PRED_FIELD( m_flTeleportTime, FIELD_FLOAT, FTYPEDESC_INSENDTABLE ),
#endif
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_displacer, CTFWeaponDisplacer );
PRECACHE_WEAPON_REGISTER( tf_weapon_displacer );

#ifdef GAME_DLL
extern EHANDLE g_pLastSpawnPoints[TF_TEAM_COUNT];
#endif

//=============================================================================
//
// Weapon Displacer functions.
//

CTFWeaponDisplacer::CTFWeaponDisplacer()
{
	m_bPlayedTeleportEffect = false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::Precache( void )
{
	// TEMP!!!
	PrecacheTeamParticles( "teleported_%s" );
	PrecacheTeamParticles( "teleportedin_%s" );
	PrecacheTeamParticles( "player_sparkles_%s" );

	BaseClass::Precache();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::WeaponReset( void )
{
	m_flBlastTime = 0.0f;
	m_flTeleportTime = 0.0f;
	m_bLockedOn = false;
	m_bPlayedTeleportEffect = false;

#ifdef GAME_DLL
	m_hTeleportSpot = NULL;
#endif

	BaseClass::WeaponReset();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFWeaponDisplacer::CanHolster( void ) const
{
	// Can't holster while charging up a shot.
	if ( IsChargingAnyShot() )
	{
		return false;
	}

	return BaseClass::CanHolster();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFWeaponDisplacer::Holster( CBaseCombatWeapon *pSwitchingTo )
{
	m_flBlastTime = 0.0f;
	m_flTeleportTime = 0.0f;
	m_bLockedOn = false;

#ifdef GAME_DLL
	m_hTeleportSpot = NULL;
#endif

	return BaseClass::Holster( pSwitchingTo );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::ItemPostFrame( void )
{
	if ( m_flBlastTime != 0.0f && gpGlobals->curtime >= m_flBlastTime )
	{
		// Deliberately skipping to base class since our function starts charging.
		m_flBlastTime = 0.0f;
		BaseClass::PrimaryAttack();
	}
	else if ( m_flTeleportTime != 0.0f )
	{
		if ( gpGlobals->curtime >= m_flTeleportTime )
		{
			FinishTeleport();
		}
		else if ( !m_bPlayedTeleportEffect && m_flTeleportTime - gpGlobals->curtime <= 0.1f )
		{
			m_bPlayedTeleportEffect = true;

			// Create an effect right before the teleport.
			// TODO: Will need a new particle that will follow player from pressing alt-fire to teleporting.
			CTFPlayer *pPlayer = GetTFPlayerOwner();
			if ( pPlayer )
			{
				const char *pszSparklesEffect = ConstructTeamParticle( "player_sparkles_%s", pPlayer->GetTeamNumber() );
				DispatchParticleEffect( pszSparklesEffect, PATTACH_ABSORIGIN, pPlayer );
			}
		}
	}

	BaseClass::ItemPostFrame();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::PrimaryAttack( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	// Already charging up a shot?
	if ( IsChargingAnyShot() )
		return;

	m_iWeaponMode = TF_WEAPON_PRIMARY_MODE;

	// Start charging.
	WeaponSound( SPECIAL2 );
	m_flBlastTime = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flSmackDelay;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::SecondaryAttack( void )
{
	// Get the player owning the weapon.
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( !pPlayer )
		return;

	if ( !CanAttack() )
		return;

	// Already charging up a shot?
	if ( IsChargingAnyShot() )
		return;

	// Do we have enough ammo?
	if ( pPlayer->GetAmmoCount( m_iSecondaryAmmoType ) < m_pWeaponInfo->GetWeaponData( TF_WEAPON_SECONDARY_MODE ).m_iAmmoPerShot )
		return;

	m_iWeaponMode = TF_WEAPON_SECONDARY_MODE;

#ifdef GAME_DLL
	// Find a furthest possible respawn point.
	CBaseEntity *pSpot = g_pLastSpawnPoints[pPlayer->GetTeamNumber()];

	if ( pPlayer->SelectFurthestSpawnSpot( "info_player_deathmatch", pSpot, false ) )
	{
		// Need to remove prediction filtering since this code only runs on server side.
		CDisablePredictionFiltering disabler;

		// Create a warning effect for other players at the chosen destination.
		const char *pszTeleportedEffect = ConstructTeamParticle( "teleported_%s", pPlayer->GetTeamNumber() );
		DispatchParticleEffect( pszTeleportedEffect, pSpot->GetAbsOrigin(), vec3_angle );

		g_pLastSpawnPoints[pPlayer->GetTeamNumber()] = pSpot;
		m_hTeleportSpot = pSpot;
		m_bLockedOn = true;
	}
#endif

	WeaponSound( SPECIAL3 );

	m_bPlayedTeleportEffect = false;
	m_flTeleportTime = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flSmackDelay;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFWeaponDisplacer::IsChargingAnyShot( void ) const
{
	return ( m_flBlastTime || m_flTeleportTime );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFWeaponDisplacer::FinishTeleport( void )
{
	CTFPlayer *pPlayer = GetTFPlayerOwner();
	if ( pPlayer )
	{
#ifdef GAME_DLL
		AssertMsg( m_hTeleportSpot.Get() != NULL, "Displacer teleport was triggered with no destination." );

		if ( m_hTeleportSpot.Get() )
#else
		if ( m_bLockedOn )
#endif
		{
#ifdef GAME_DLL
			// Need to remove prediction filtering since this code only runs on server side.
			CDisablePredictionFiltering *pDisabler = new CDisablePredictionFiltering();

			CBaseEntity *ent = NULL;
			for ( CEntitySphereQuery sphere( m_hTeleportSpot->GetAbsOrigin(), 128 ); ( ent = sphere.GetCurrentEntity() ) != NULL; sphere.NextEntity() )
			{
				// if ent is a client, telefrag 'em (unless they are ourselves)
				if ( ent->IsPlayer() && ent != pPlayer && ( !ent->InSameTeam( pPlayer ) || TFGameRules()->IsDeathmatch() ) )
				{
					CTakeDamageInfo info( pPlayer, pPlayer, 1000, DMG_CRUSH, TF_DMG_TELEFRAG );
					ent->TakeDamage( info );
				}
			}

			pPlayer->Teleport( &m_hTeleportSpot->GetAbsOrigin(), &m_hTeleportSpot->GetAbsAngles(), &vec3_origin );

			const char *pszTeleportedEffect = ConstructTeamParticle( "teleportedin_%s", pPlayer->GetTeamNumber() );
			DispatchParticleEffect( pszTeleportedEffect, m_hTeleportSpot->GetAbsOrigin(), vec3_angle );

			delete pDisabler;
#endif

			WeaponSound( WPN_DOUBLE );
			pPlayer->RemoveAmmo( m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_iAmmoPerShot, m_iSecondaryAmmoType );
		}
	}

#ifdef GAME_DLL
	m_hTeleportSpot = NULL;
#endif

	m_flTeleportTime = 0.0f;
	m_bLockedOn = false;
	m_flNextSecondaryAttack = gpGlobals->curtime + m_pWeaponInfo->GetWeaponData( m_iWeaponMode ).m_flTimeFireDelay;
}
