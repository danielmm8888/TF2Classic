//=============================================================================//
//
// Purpose: Arrow used by Huntsman.
//
//=============================================================================//
#ifndef TF_PROJECTILE_ARROW_H
#define TF_PROJECTILE_ARROW_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_rocket.h"

#ifdef CLIENT_DLL
#define CTFProjectile_Arrow C_TFProjectile_Arrow
#endif

class CTFProjectile_Arrow : public CTFBaseRocket
{
public:
	DECLARE_CLASS( CTFProjectile_Arrow, CTFBaseRocket );
	DECLARE_NETWORKCLASS();
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFProjectile_Arrow();
	~CTFProjectile_Arrow();

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_COMPOUND_BOW; }

#ifdef GAME_DLL
	static CTFProjectile_Arrow *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, float flSpeed, float flGravity, CBaseEntity *pOwner, CBaseEntity *pScorer, int iType );

	virtual void	Precache( void );
	virtual void	Spawn( void );

	virtual int		GetDamageType( void );

	virtual void	Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );

	void			SetType( int iType ) { m_iType = iType; }
	bool			CanHeadshot( void );
	void			ArrowTouch( CBaseEntity *pOther );
	const char		*GetTrailParticleName( void );
	void			CreateTrail( void );

	virtual void	UpdateOnRemove( void );
#else

#endif

private:
#ifdef GAME_DLL
	CNetworkVar( int, m_iType );

	EHANDLE		m_hSpriteTrail;
#else
	int			m_iType;
#endif
};

#endif // TF_PROJECTILE_ARROW_H
