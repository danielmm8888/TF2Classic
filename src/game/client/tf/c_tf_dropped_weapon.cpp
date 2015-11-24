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
	DECLARE_CLASS( C_TFDroppedWeapon, CBaseAnimating );
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
};

IMPLEMENT_CLIENTCLASS_DT( C_TFDroppedWeapon, DT_TFDroppedWeapon, CTFDroppedWeapon )
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( tf_dropped_weapon, C_TFDroppedWeapon );


C_TFDroppedWeapon::C_TFDroppedWeapon()
{
	m_pGlowEffect = NULL;
	m_bShouldGlow = false;
}


C_TFDroppedWeapon::~C_TFDroppedWeapon()
{
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
		UTIL_TraceLine( GetAbsOrigin(), pPlayer->EyePosition(), MASK_OPAQUE, this, COLLISION_GROUP_NONE, &tr );
		
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
		m_pGlowEffect = new CGlowObject( this, Vector( 0.75f, 0.75f, 0.15f ), 1.0f, true, true );
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
