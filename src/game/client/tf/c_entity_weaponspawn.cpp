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

private:
	CMaterialReference	m_InactiveMaterial;
	QAngle		qAngle;
	CGlowObject *m_pGlowObject;
	bool		m_bInactive;
};

LINK_ENTITY_TO_CLASS(tf_weaponspawner, C_WeaponSpawner);

IMPLEMENT_CLIENTCLASS_DT(C_WeaponSpawner, DT_WeaponSpawner, CWeaponSpawner)
	RecvPropBool( RECVINFO( m_bInactive ) )
END_RECV_TABLE()

void C_WeaponSpawner::Spawn( void )
{
	BaseClass::Spawn();
	qAngle = GetAbsAngles();
	ClientThink();
	m_InactiveMaterial.Init( "models/weapons/weapon_spawner.vmt", TEXTURE_GROUP_CLIENT_EFFECTS );
}

void C_WeaponSpawner::ClientThink()
{
	qAngle.y += 90 * gpGlobals->frametime;
	if (qAngle.y >= 360)
		qAngle.y -= 360;

	SetAbsAngles(qAngle);

	HandleGlowEffect();

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

int C_WeaponSpawner::InternalDrawModel( int flags )
{
	if ( m_bInactive )
	{
		modelrender->ForcedMaterialOverride( m_InactiveMaterial );
		int iRet = BaseClass::InternalDrawModel( flags );
		modelrender->ForcedMaterialOverride( NULL );
		return iRet;
	}
	else
	{
		return BaseClass::InternalDrawModel(flags);
	}

}

void C_WeaponSpawner::HandleGlowEffect()
{
	if ( !m_pGlowObject )
	{
		m_pGlowObject = new CGlowObject( this, Vector( 0.76f, 0.76f, 0.76f ), 0.0, false, true );
	}

	// DIsable the outline if the weapon has been picked up
	
	if ( !m_bInactive )
		m_pGlowObject->SetAlpha( 1.0f );
	else
		m_pGlowObject->SetAlpha( 0.0f );

}



