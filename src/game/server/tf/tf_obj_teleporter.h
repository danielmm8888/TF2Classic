//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Engineer's Teleporter
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_OBJ_TELEPORTER_H
#define TF_OBJ_TELEPORTER_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_obj.h"

class CTFPlayer;

// ------------------------------------------------------------------------ //
// Base Teleporter object
// ------------------------------------------------------------------------ //
class CObjectTeleporter : public CBaseObject
{
	DECLARE_CLASS( CObjectTeleporter, CBaseObject );

public:
	DECLARE_DATADESC();
	DECLARE_SERVERCLASS();

	CObjectTeleporter();

	virtual void	Spawn();
	virtual void	Precache();
	virtual bool	StartBuilding( CBaseEntity *pBuilder );
	virtual void	OnGoActive( void );
	virtual int		DrawDebugTextOverlays(void) ;
	virtual bool	IsPlacementPosValid( void );
	virtual void	SetModel( const char *pModel );

	virtual void	StartUpgrading( void );
	virtual void	FinishUpgrading( void );
	virtual bool	IsUpgrading( void ) const;

	virtual void	FinishedBuilding( void );

	void SetState( int state );
	virtual void	DeterminePlaybackRate( void );

	void TeleporterThink( void );
	void TeleporterTouch( CBaseEntity *pOther );

	virtual void TeleporterReceive( CTFPlayer *pPlayer, float flDelay );
	virtual void TeleporterSend( CTFPlayer *pPlayer );

	void CopyUpgradeStateToMatch( CObjectTeleporter *pMatch, bool bCopyFrom );

	CObjectTeleporter *GetMatchingTeleporter( void );
	CObjectTeleporter *FindMatch( void );	// Find the teleport partner to this object

	virtual bool InputWrenchHit( CTFPlayer *pPlayer, CTFWrench *pWrench, Vector vecHitPos );

	virtual bool Command_Repair( CTFPlayer *pActivator );

	virtual bool CheckUpgradeOnHit( CTFPlayer *pPlayer );

	virtual void InitializeMapPlacedObject( void );

	bool IsMatchingTeleporterReady( void );

	bool PlayerCanBeTeleported( CTFPlayer *pSender );

	bool IsSendingPlayer( CTFPlayer *pSender );

	int GetState( void ) { return m_iState; }	// state of the object ( building, charging, ready etc )

	void SetTeleportingPlayer( CTFPlayer *pPlayer )
	{
		m_hTeleportingPlayer = pPlayer;
	}

	virtual int GetBaseHealth( void );
	virtual int	GetMaxUpgradeLevel( void );
	virtual char *GetPlacementModel( void );

	virtual void	MakeCarriedObject( CTFPlayer *pPlayer );

protected:
	CNetworkVar( int, m_iState );
	CNetworkVar( float, m_flRechargeTime );
	CNetworkVar( int, m_iTimesUsed );
	CNetworkVar( float, m_flYawToExit );

	CHandle<CObjectTeleporter> m_hMatchingTeleporter;

	float m_flLastStateChangeTime;

	float m_flMyNextThink;	// replace me

	CHandle<CTFPlayer> m_hTeleportingPlayer;

	float m_flNextEnemyTouchHint;

	// Direction Arrow, shows roughly what direction the exit is from the entrance
	void ShowDirectionArrow( bool bShow );

	bool m_bShowDirectionArrow;
	int m_iDirectionBodygroup;
	int m_iBlurBodygroup;

private:
	// Only used by hammer placed entities
	int m_iTeleporterType;
	string_t m_szMatchingTeleporterName;
};

#endif // TF_OBJ_TELEPORTER_H
