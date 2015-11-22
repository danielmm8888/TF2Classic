//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Dropped DM weapon
//
//=============================================================================//

#include "cbase.h"
#include "glow_outline_effect.h"
#include "collisionutils.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class C_TFDroppedWeapon : public CBaseAnimating
{
public:
	DECLARE_CLASS( C_TFDroppedWeapon, CBaseAnimating );
	DECLARE_CLIENTCLASS();

	C_TFDroppedWeapon();
	~C_TFDroppedWeapon();

	virtual void	Spawn( void );
	void	ClientThink();
	void	HandleGlowEffect();

private:
	int m_iGlowEffectHandle;
};

IMPLEMENT_NETWORKCLASS_ALIASED( TFDroppedWeapon, DT_TFDroppedWeapon )

BEGIN_RECV_TABLE( C_TFDroppedWeapon, DT_TFDroppedWeapon )
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( tf_dropped_weapon, C_TFDroppedWeapon );


C_TFDroppedWeapon::C_TFDroppedWeapon()
{
	m_iGlowEffectHandle = -1;
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
	ClientThink();
}

void C_TFDroppedWeapon::ClientThink()
{
	HandleGlowEffect();

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

void C_TFDroppedWeapon::HandleGlowEffect()
{
	if ( !g_GlowObjectManager.HasGlowEffect( this ) )
	{
		m_iGlowEffectHandle = g_GlowObjectManager.RegisterGlowObject( this, Vector( 1.0f, 1.0f, 0.0f ), 1.0f, true, false, GLOW_FOR_ALL_SPLIT_SCREEN_SLOTS );
	}
}
