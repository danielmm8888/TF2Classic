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

#ifdef GAME_DLL
#include "iscorer.h"
#endif

#ifdef CLIENT_DLL
#define CTFProjectile_Arrow C_TFProjectile_Arrow
#endif

#ifdef GAME_DLL
class CTFProjectile_Arrow : public CTFBaseRocket, public IScorer
#else
class CTFProjectile_Arrow : public C_TFBaseRocket
#endif
{
public:
	DECLARE_CLASS( CTFProjectile_Arrow, CTFBaseRocket );
	DECLARE_NETWORKCLASS();
#ifdef GAME_DLL
	DECLARE_DATADESC();
#endif

	CTFProjectile_Arrow();
	~CTFProjectile_Arrow();

	void	SetType( int iType ) { m_iType = iType; }

	virtual int		GetWeaponID( void ) const { return TF_WEAPON_COMPOUND_BOW; }

#ifdef GAME_DLL
	static CTFProjectile_Arrow *Create( CBaseEntity *pWeapon, const Vector &vecOrigin, const QAngle &vecAngles, float flSpeed, float flGravity, CBaseEntity *pOwner, CBaseEntity *pScorer, int iType );

	// IScorer interface
	virtual CBasePlayer *GetScorer( void );
	virtual CBasePlayer *GetAssistant( void ) { return NULL; }

	virtual void	Precache( void );
	virtual void	Spawn( void );

	void			SetScorer( CBaseEntity *pScorer );

	void			SetCritical( bool bCritical ) { m_bCritical = bCritical; }
	virtual int		GetDamageType();

	virtual bool	IsDeflectable() { return true; }
	virtual void	Deflected( CBaseEntity *pDeflectedBy, Vector &vecDir );

	bool			CanHeadshot( void );
	void			ArrowTouch( CBaseEntity *pOther );
	const char		*GetTrailParticleName( void );
	void			CreateTrail( void );

	virtual void	UpdateOnRemove( void );
#else

#endif

private:
#ifdef GAME_DLL
	EHANDLE m_Scorer;
	CNetworkVar( bool, m_bCritical );
	CNetworkVar( int, m_iType );

	EHANDLE m_hSpriteTrail;
#else
	bool		m_bCritical;
	int			m_iType;
#endif
};

#endif // TF_PROJECTILE_ARROW_H
