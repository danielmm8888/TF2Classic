//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Deathmatch weapon spawning entity.
//
//=============================================================================//
#include "cbase.h"
#include "glow_outline_effect.h"
#include "c_tf_player.h"
#include "collisionutils.h"

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

private:
	QAngle				m_qAngle;
	CGlowObject		   *m_pGlowEffect;
	bool				m_bInactive;
};

LINK_ENTITY_TO_CLASS(tf_weaponspawner, C_WeaponSpawner);

IMPLEMENT_CLIENTCLASS_DT(C_WeaponSpawner, DT_WeaponSpawner, CWeaponSpawner)
	RecvPropBool( RECVINFO( m_bInactive ) ),
END_RECV_TABLE()

void C_WeaponSpawner::Spawn( void )
{
	BaseClass::Spawn();
	m_qAngle = GetAbsAngles();
	ClientThink();
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

void C_WeaponSpawner::HandleGlowEffect()
{
	if ( !m_pGlowEffect )
	{
		m_pGlowEffect = new CGlowObject( this, Vector( 0.0f, 0.0f, 1.0f ), 1.0, true, true, 0 );
	}

	// DIsable the outline if the weapon has been picked up
	if ( m_bInactive )
	{
		m_pGlowEffect->SetAlpha( 0.0f );
		return;
	}

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( pPlayer )
	{
		Vector vecPlayerOrigin = pPlayer->GetAbsOrigin();
		Vector vecPlayerMins = vecPlayerOrigin + pPlayer->GetPlayerMins();
		Vector vecPlayerMaxs = vecPlayerOrigin + pPlayer->GetPlayerMaxs();
		Vector vecDir = pPlayer->GetAbsOrigin() - vecPlayerOrigin;

		if ( IsBoxIntersectingBox( GetAbsOrigin() + WorldAlignMins(), GetAbsOrigin() + WorldAlignMaxs(), vecPlayerMins, vecPlayerMaxs ) )
		{
			// White glow.
			m_pGlowEffect->SetColor( Vector( 0.76f, 0.76f, 0.76f ) );
			m_pGlowEffect->SetAlpha( 1.0f );
			return;
		}

		// Temp crutch for glow occluded\unoccluded parameters not working.
		trace_t tr;
		UTIL_TraceLine( GetAbsOrigin(), pPlayer->EyePosition(), MASK_OPAQUE, this, COLLISION_GROUP_NONE, &tr );

		if ( tr.fraction == 1.0f )
		{
			// Blue glow.
			m_pGlowEffect->SetColor( Vector( 0.6f, 0.6f, 1.0f ) );
			m_pGlowEffect->SetAlpha( 1.0f );
			return;
		}

		m_pGlowEffect->SetAlpha( 0.0f );
		return;
	}

	// DIsable the outline if the weapon has been picked up
	/*if ( !m_bInactive )
		m_pGlowEffect->SetAlpha( 1.0f );
	else*/
		m_pGlowEffect->SetAlpha( 0.0f );
}



