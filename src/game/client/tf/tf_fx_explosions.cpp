//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: Game-specific explosion effects
//
//=============================================================================//
#include "cbase.h"
#include "c_te_effect_dispatch.h"
#include "tempent.h"
#include "c_te_legacytempents.h"
#include "tf_shareddefs.h"
#include "engine/IEngineSound.h"
#include "tf_weapon_parse.h"
#include "c_basetempentity.h"
#include "tier0/vprof.h"
#include "econ_item_system.h"
#include "tf_gamerules.h"

//--------------------------------------------------------------------------------------------------------------
extern CTFWeaponInfo *GetTFWeaponInfo( int iWeapon );
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void TFExplosionCallback( const Vector &vecOrigin, const Vector &vecNormal, int iWeaponID, ClientEntityHandle_t hEntity, C_TFPlayer *pPlayer, int iTeam, bool bCrit, int iItemID )
{
	// Get the weapon information.
	CTFWeaponInfo *pWeaponInfo = NULL;
	switch ( iWeaponID )
	{
	case TF_WEAPON_GRENADE_PIPEBOMB:
	case TF_WEAPON_GRENADE_DEMOMAN:
		pWeaponInfo = GetTFWeaponInfo( TF_WEAPON_PIPEBOMBLAUNCHER );
		break;
	default:
		pWeaponInfo = GetTFWeaponInfo( iWeaponID );
		break;
	}

	bool bIsPlayer = false;
	if ( hEntity.Get() )
	{
		C_BaseEntity *pEntity = C_BaseEntity::Instance( hEntity );
		if ( pEntity && pEntity->IsPlayer() )
		{
			bIsPlayer = true;
		}
	}

	// Calculate the angles, given the normal.
	bool bIsWater = ( UTIL_PointContents( vecOrigin ) & CONTENTS_WATER );
	bool bInAir = false;
	QAngle angExplosion( 0.0f, 0.0f, 0.0f );

	// Cannot use zeros here because we are sending the normal at a smaller bit size.
	if ( fabs( vecNormal.x ) < 0.05f && fabs( vecNormal.y ) < 0.05f && fabs( vecNormal.z ) < 0.05f )
	{
		bInAir = true;
		angExplosion.Init();
	}
	else
	{
		VectorAngles( vecNormal, angExplosion );
		bInAir = false;
	}

	bool bDeathmatchOverride = ( pPlayer && TFGameRules()->IsDeathmatch() );

	// Base explosion effect and sound.
	const char *pszEffect = "explosion";
	const char *pszSound = "BaseExplosionEffect.Sound";

	if ( pWeaponInfo )
	{
		// Explosions.
		if ( bIsWater )
		{
			if ( bCrit && pWeaponInfo->m_szExplosionWaterEffect_Crit[0] )
			{
				pszEffect = ConstructTeamParticle( pWeaponInfo->m_szExplosionWaterEffect_Crit, iTeam, bDeathmatchOverride );
			}
			else if ( pWeaponInfo->m_szExplosionWaterEffect[0] )
			{
				pszEffect = pWeaponInfo->m_szExplosionWaterEffect;
			}
		}
		else
		{
			if ( bIsPlayer || bInAir )
			{
				if ( bCrit && pWeaponInfo->m_szExplosionPlayerEffect_Crit[0] )
				{
					pszEffect = ConstructTeamParticle( pWeaponInfo->m_szExplosionPlayerEffect_Crit, iTeam, bDeathmatchOverride );
				}
				else if ( pWeaponInfo->m_szExplosionPlayerEffect[0] )
				{
					pszEffect = pWeaponInfo->m_szExplosionPlayerEffect;
				}
			}
			else
			{
				if ( bCrit && pWeaponInfo->m_szExplosionEffect_Crit[0] )
				{
					pszEffect = ConstructTeamParticle( pWeaponInfo->m_szExplosionEffect_Crit, iTeam, bDeathmatchOverride );
				}
				else if ( pWeaponInfo->m_szExplosionEffect[0] )
				{
					pszEffect = pWeaponInfo->m_szExplosionEffect;
				}
			}
		}

		// Sound.
		if ( pWeaponInfo->m_szExplosionSound[0] != '\0' )
		{
			pszSound = pWeaponInfo->m_szExplosionSound;
		}
	}

	// Allow schema to override explosion sound.
	if ( iItemID >= 0 )
	{
		CEconItemDefinition *pItemDef = GetItemSchema()->GetItemDefinition( iItemID );
		if ( pItemDef && pItemDef->GetVisuals()->aWeaponSounds[SPECIAL1][0] != '\0' )
		{
			pszSound = pItemDef->GetVisuals()->aWeaponSounds[SPECIAL1];
		}
	}
	
	CLocalPlayerFilter filter;
	C_BaseEntity::EmitSound( filter, SOUND_FROM_WORLD, pszSound, &vecOrigin );

	if ( bDeathmatchOverride )
	{
		DispatchParticleEffect( pszEffect, vecOrigin, angExplosion, pPlayer->m_vecPlayerColor, vec3_origin, true );
	}
	else
	{
		DispatchParticleEffect( pszEffect, vecOrigin, angExplosion );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_TETFExplosion : public C_BaseTempEntity
{
public:

	DECLARE_CLASS( C_TETFExplosion, C_BaseTempEntity );
	DECLARE_CLIENTCLASS();

	C_TETFExplosion( void );

	virtual void	PostDataUpdate( DataUpdateType_t updateType );

public:

	Vector			m_vecOrigin;
	Vector			m_vecNormal;
	int				m_iWeaponID;
	int				m_iItemID;
	C_TFPlayer	*m_pPlayer;
	int				m_iTeamNum;
	bool			m_bCritical;
	ClientEntityHandle_t m_hEntity;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
C_TETFExplosion::C_TETFExplosion( void )
{
	m_vecOrigin.Init();
	m_vecNormal.Init();
	m_iWeaponID = TF_WEAPON_NONE;
	m_iItemID = -1;
	m_pPlayer = NULL;
	m_iTeamNum = TEAM_UNASSIGNED;
	m_bCritical = false;
	m_hEntity = INVALID_EHANDLE_INDEX;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_TETFExplosion::PostDataUpdate( DataUpdateType_t updateType )
{
	VPROF( "C_TETFExplosion::PostDataUpdate" );

	TFExplosionCallback( m_vecOrigin, m_vecNormal, m_iWeaponID, m_hEntity, m_pPlayer, m_iTeamNum, m_bCritical, m_iItemID );
}

static void RecvProxy_ExplosionEntIndex( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	int nEntIndex = pData->m_Value.m_Int;
	((C_TETFExplosion*)pStruct)->m_hEntity = (nEntIndex < 0) ? INVALID_EHANDLE_INDEX : ClientEntityList().EntIndexToHandle( nEntIndex );
}

static void RecvProxy_ExplosionUserID( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	int iUserID = pData->m_Value.m_Int;
	( (C_TETFExplosion*)pStruct )->m_pPlayer = ( iUserID > 0 ) ? ToTFPlayer( UTIL_PlayerByUserId( iUserID ) ) : NULL;
}

IMPLEMENT_CLIENTCLASS_EVENT_DT( C_TETFExplosion, DT_TETFExplosion, CTETFExplosion )
	RecvPropFloat( RECVINFO( m_vecOrigin[0] ) ),
	RecvPropFloat( RECVINFO( m_vecOrigin[1] ) ),
	RecvPropFloat( RECVINFO( m_vecOrigin[2] ) ),
	RecvPropVector( RECVINFO( m_vecNormal ) ),
	RecvPropInt( RECVINFO( m_iWeaponID ) ),
	RecvPropInt( RECVINFO( m_iItemID ) ),
	RecvPropInt( "m_iUserID", 0, SIZEOF_IGNORE, 0, RecvProxy_ExplosionUserID ),
	RecvPropInt( RECVINFO( m_iTeamNum ) ),
	RecvPropBool( RECVINFO( m_bCritical ) ),
	RecvPropInt( "entindex", 0, SIZEOF_IGNORE, 0, RecvProxy_ExplosionEntIndex ),
END_RECV_TABLE()

