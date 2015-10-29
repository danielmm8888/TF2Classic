//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======//
//
// Purpose: Dropped DM weapon
//
//=============================================================================//

#include "cbase.h"
#include "glow_outline_effect.h"

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

private:
	int m_nGlowHandle;
};

IMPLEMENT_NETWORKCLASS_ALIASED( TFDroppedWeapon, DT_TFDroppedWeapon )

BEGIN_RECV_TABLE( C_TFDroppedWeapon, DT_TFDroppedWeapon )
END_RECV_TABLE()

LINK_ENTITY_TO_CLASS( tf_dropped_weapon, C_TFDroppedWeapon );


C_TFDroppedWeapon::C_TFDroppedWeapon()
{
	m_nGlowHandle = -1;
}


C_TFDroppedWeapon::~C_TFDroppedWeapon()
{
	if ( m_nGlowHandle != -1 )
	{
			g_GlowObjectManager.UnregisterGlowObject( m_nGlowHandle );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Spawn function 
//-----------------------------------------------------------------------------
void C_TFDroppedWeapon::Spawn( void )
{
	BaseClass::Spawn();
	m_nGlowHandle = g_GlowObjectManager.RegisterGlowObject( this, Vector( 1.0f, 1.0f, 0.0f ) , 0.9f, true, true, 0 );
}


