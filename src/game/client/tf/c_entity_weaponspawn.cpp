//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Deathmatch weapon spawning entity.
//
//=============================================================================//
#include "cbase.h"
#include "glow_outline_effect.h"
#include "c_tf_player.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_WeaponSpawner : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_WeaponSpawner, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	void	Spawn();
	void	ClientThink();
	void	HandleGlowEffect();

	virtual int	InternalDrawModel( int flags );
	virtual void OnPreDataChanged( DataUpdateType_t updateType );
	virtual void OnDataChanged( DataUpdateType_t updateType );

	CUtlVector< EHANDLE > m_hNearbyPlayers;

private:
	CMaterialReference	m_InactiveMaterial;
	QAngle				m_qAngle;
	CGlowObject		   *m_pGlowEffect;
	bool				m_bInactive;
};

//-----------------------------------------------------------------------------
// Purpose: RecvProxy that converts the UtlVector to entindexes
//-----------------------------------------------------------------------------
void RecvProxy_NearbyPlayerList( const CRecvProxyData *pData, void *pStruct, void *pOut )
{
	C_WeaponSpawner *pSpawner = (C_WeaponSpawner*)pStruct;

	CBaseHandle *pHandle = (CBaseHandle*)(&(pSpawner->m_hNearbyPlayers[pData->m_iElement])); 
	RecvProxy_IntToEHandle( pData, pStruct, pHandle );
}

void RecvProxyArrayLength_NearbyPlayerList( void *pStruct, int objectID, int currentArrayLength )
{
	C_WeaponSpawner *pSpawner = (C_WeaponSpawner*)pStruct;

	if ( pSpawner->m_hNearbyPlayers.Size() != currentArrayLength )
		pSpawner->m_hNearbyPlayers.SetSize( currentArrayLength );
}

LINK_ENTITY_TO_CLASS(tf_weaponspawner, C_WeaponSpawner);

IMPLEMENT_CLIENTCLASS_DT(C_WeaponSpawner, DT_WeaponSpawner, CWeaponSpawner)
	RecvPropBool( RECVINFO( m_bInactive ) ),
	RecvPropArray2( 
	RecvProxyArrayLength_NearbyPlayerList,
	RecvPropInt( "nearby_player_list_element", 0, SIZEOF_IGNORE, 0, RecvProxy_NearbyPlayerList ), 
	MAX_PLAYERS, 
	0, 
	"nearby_player_list"
	)
END_RECV_TABLE()

void C_WeaponSpawner::Spawn( void )
{
	BaseClass::Spawn();
	m_qAngle = GetAbsAngles();
	ClientThink();
	m_InactiveMaterial.Init( "models/weapons/weapon_spawner.vmt", TEXTURE_GROUP_CLIENT_EFFECTS );
}

void C_WeaponSpawner::ClientThink()
{
	m_qAngle.y += 90 * gpGlobals->frametime;
	if ( m_qAngle.y >= 360 )
		m_qAngle.y -= 360;

	SetAbsAngles( m_qAngle );

	HandleGlowEffect();

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

int C_WeaponSpawner::InternalDrawModel( int flags )
{
	/*if ( m_bInactive )
	{
		modelrender->ForcedMaterialOverride( m_InactiveMaterial );
		int iRet = BaseClass::InternalDrawModel( flags );
		modelrender->ForcedMaterialOverride( NULL );
		return iRet;
	}
	else
	{
		return BaseClass::InternalDrawModel( flags );
	}*/

	return BaseClass::InternalDrawModel( flags );
}

void C_WeaponSpawner::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );
}

void C_WeaponSpawner::OnDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnDataChanged( updateType );

	/*if ( updateType == DATA_UPDATE_DATATABLE_CHANGED )
	{
		HandleGlowEffect();
	}*/
}

void C_WeaponSpawner::HandleGlowEffect()
{
	if ( !m_pGlowEffect )
	{
		m_pGlowEffect = new CGlowObject( this, Vector( 0.76f, 0.76f, 0.76f ), 1.0, true, true, 0 );
	}

	// DIsable the outline if the weapon has been picked up
	if ( m_bInactive )
	{
		m_pGlowEffect->SetAlpha( 0.0f );
		return;
	}

	// Only enable the glow if the local player is nearby
	for ( int i = 0; i < m_hNearbyPlayers.Size(); i++ )
	{
		if ( ToBasePlayer( m_hNearbyPlayers.Element(i) ) && ToBasePlayer( m_hNearbyPlayers.Element(i) ) == C_BasePlayer::GetLocalPlayer() )
		{
 			m_pGlowEffect->SetAlpha( 1.0f );
			return;
		}
		else
		{
 			m_pGlowEffect->SetAlpha( 0.0f );
		}
	}

	// DIsable the outline if the weapon has been picked up
	/*if ( !m_bInactive )
		m_pGlowEffect->SetAlpha( 1.0f );
	else*/
		m_pGlowEffect->SetAlpha( 0.0f );
}



