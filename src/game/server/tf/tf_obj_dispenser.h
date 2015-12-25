//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engineer's Dispenser
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_OBJ_DISPENSER_H
#define TF_OBJ_DISPENSER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

enum
{
	DISPENSER_LEVEL_1 = 0,
	DISPENSER_LEVEL_2,
	DISPENSER_LEVEL_3,
};

#define SF_IGNORE_LOS	0x0004
#define SF_NO_DISGUISED_SPY_HEALING	0x0008

// ------------------------------------------------------------------------ //
// Resupply object that's built by the player
// ------------------------------------------------------------------------ //
class CObjectDispenser : public CBaseObject
{
	DECLARE_CLASS( CObjectDispenser, CBaseObject );

public:
	DECLARE_SERVERCLASS();

	CObjectDispenser();
	~CObjectDispenser();

	static CObjectDispenser* Create(const Vector &vOrigin, const QAngle &vAngles);

	virtual void	Spawn();
	virtual void	GetControlPanelInfo( int nPanelIndex, const char *&pPanelName );
	virtual void	Precache();
	virtual bool	ClientCommand( CTFPlayer *pPlayer, const CCommand &args );

	virtual void	DetonateObject( void );
	virtual void	OnGoActive( void );	
	virtual bool	StartBuilding( CBaseEntity *pBuilder );
	virtual void	InitializeMapPlacedObject( void );
	virtual int		DrawDebugTextOverlays(void) ;
	virtual void	SetModel( const char *pModel );

	void RefillThink( void );
	void DispenseThink( void );

	virtual float GetDispenserRadius( void );
	virtual float GetHealRate( void );

	virtual void StartTouch( CBaseEntity *pOther );
	virtual void EndTouch( CBaseEntity *pOther );

	virtual int	ObjectCaps( void ) { return (BaseClass::ObjectCaps() | FCAP_IMPULSE_USE); }

	virtual int GetBaseHealth( void );

	bool DispenseAmmo( CTFPlayer *pPlayer );

	void StartHealing( CBaseEntity *pOther );
	void StopHealing( CBaseEntity *pOther );

	void AddHealingTarget( CBaseEntity *pOther );
	void RemoveHealingTarget( CBaseEntity *pOther );
	bool IsHealingTarget( CBaseEntity *pTarget );

	void ResetHealingTargets( void );

	bool CouldHealTarget( CBaseEntity *pTarget );

	Vector GetHealOrigin( void );

	CUtlVector< EHANDLE >	m_hHealingTargets;

	virtual bool	OnWrenchHit( CTFPlayer *pPlayer, CTFWrench *pWrench, Vector vecHitPos );

	virtual bool	IsUpgrading( void ) const;
	virtual int		GetMaxUpgradeLevel( void );
	virtual char	*GetPlacementModel( void );

	virtual void	MakeCarriedObject( CTFPlayer *pPlayer );
	virtual void	DropCarriedObject( CTFPlayer *pPlayer );

private:

	void StartUpgrading( void );
	void FinishUpgrading( void );


	//CNetworkArray( EHANDLE, m_hHealingTargets, MAX_DISPENSER_HEALING_TARGETS );


	// Entities currently being touched by this trigger
	CUtlVector< EHANDLE >	m_hTouchingEntities;

	CNetworkVar( int, m_iAmmoMetal );

	// Time when the upgrade animation will complete
	float m_flUpgradeCompleteTime;

	float m_flNextAmmoDispense;

	bool m_bIsUpgrading;

	EHANDLE m_hTouchTrigger;
	string_t m_szTriggerName;

	DECLARE_DATADESC();
};

class CObjectCartDispenser : public CObjectDispenser
{
	DECLARE_CLASS( CObjectCartDispenser, CObjectDispenser );
	DECLARE_DATADESC();

public:
	DECLARE_SERVERCLASS();

	virtual int		GetMaxUpgradeLevel( void ) { return 1; }
	virtual void	Spawn( void );
	virtual bool	CanBeUpgraded( CTFPlayer *pPlayer ) { return false; }
	virtual void	GetControlPanelInfo( int nPanelIndex, const char *&pPanelName ) { return; }
	virtual void	SetModel( const char *pModel );
	virtual void	OnGoActive( void );

private:

	CNetworkVar( int, m_iAmmoMetal );

};

#endif // TF_OBJ_DISPENSER_H
