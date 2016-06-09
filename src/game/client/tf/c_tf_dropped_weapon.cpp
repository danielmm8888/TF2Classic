//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Dropped DM weapon
//
//=============================================================================//

#include "cbase.h"
#include "glow_outline_effect.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_TFDroppedWeapon : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_TFDroppedWeapon, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	C_TFDroppedWeapon();
	~C_TFDroppedWeapon();

	virtual void	OnDataChanged( DataUpdateType_t type );

	void	Spawn( void );
	void	ClientThink();
	void	UpdateGlowEffect();

private:
	CGlowObject *m_pGlowEffect;
	bool m_bShouldGlow;

	int m_iAmmo;
	int m_iMaxAmmo;
};

IMPLEMENT_CLIENTCLASS_DT( C_TFDroppedWeapon, DT_TFDroppedWeapon, CTFDroppedWeapon )
	RecvPropInt( RECVINFO( m_iAmmo ) ),
	RecvPropInt( RECVINFO( m_iMaxAmmo ) ),
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( tf_dropped_weapon, C_TFDroppedWeapon );


C_TFDroppedWeapon::C_TFDroppedWeapon()
{
	m_pGlowEffect = NULL;
	m_bShouldGlow = false;
}


C_TFDroppedWeapon::~C_TFDroppedWeapon()
{
	delete m_pGlowEffect;
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void C_TFDroppedWeapon::Spawn( void )
{
	BaseClass::Spawn();
}

//-----------------------------------------------------------------------------
// Purpose: Start thinking
//-----------------------------------------------------------------------------
void C_TFDroppedWeapon::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
}

void C_TFDroppedWeapon::ClientThink()
{
	bool bShouldGlow = false;

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( pPlayer )
	{
		// Temp crutch for Occluded\Unoccluded glow parameters not working.
		trace_t tr;
		UTIL_TraceLine( GetAbsOrigin(), pPlayer->EyePosition(), MASK_VISIBLE, this, COLLISION_GROUP_NONE, &tr );
		
		if ( tr.fraction == 1.0f )
		{
			bShouldGlow = true;
		}
	}

	if ( m_bShouldGlow != bShouldGlow )
	{
		m_bShouldGlow = bShouldGlow;
		UpdateGlowEffect();
	}

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

void C_TFDroppedWeapon::UpdateGlowEffect()
{
	if ( !m_pGlowEffect )
	{
		float flRed = RemapValClamped( m_iAmmo, m_iMaxAmmo / 2, m_iMaxAmmo, 0.75f, 0.15f );
		float flGreen = RemapValClamped( m_iAmmo, 0, m_iMaxAmmo / 2, 0.15f, 0.75f );
		float flBlue = 0.15f;
		Vector vecColor = Vector( flRed, flGreen, flBlue );

		m_pGlowEffect = new CGlowObject( this, vecColor, 1.0f, true, true );
	}

	if ( m_bShouldGlow )
	{
		m_pGlowEffect->SetAlpha( 1.0f );
	}
	else
	{
		m_pGlowEffect->SetAlpha( 0.0f );
	}
}
