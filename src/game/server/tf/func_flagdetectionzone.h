//======= Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: CTF Flag detection trigger.
//
//=============================================================================//
#ifndef FUNC_FLAGDETECTION_ZONE_H
#define FUNC_FLAGDETECTION_ZONE_H

#ifdef _WIN32
#pragma once
#endif

#include "triggers.h"
#include "tf_player.h"

//=============================================================================

DECLARE_AUTO_LIST( IFlagDetectionZoneAutoList );

class CFlagDetectionZone : public CBaseTrigger, public IFlagDetectionZoneAutoList
{
public:
	DECLARE_CLASS( CFlagDetectionZone, CBaseTrigger );
	DECLARE_DATADESC();

	virtual void	Spawn( void );
	virtual void	StartTouch( CBaseEntity *pOther );
	virtual void	EndTouch( CBaseEntity *pOther );

	virtual void	SetDisabled( bool bDisabled );

	bool	EntityIsFlagCarrier( CBaseEntity *pEntity );

	void	FlagCaptured( CTFPlayer *pPlayer );
	void	FlagDropped( CTFPlayer *pPlayer );
	void	FlagPickedUp( CTFPlayer *pPlayer );

	bool	IsDisabled( void ) { return m_bDisabled; };

	// Input handlers
	virtual void	InputEnable( inputdata_t &inputdata );
	virtual void	InputDisable( inputdata_t &inputdata );
	virtual void	InputTest( inputdata_t &inputdata );

private:
	bool	m_bDisabled;
	bool	m_bShouldAlarm;

	COutputEvent m_outOnStartTouchFlag; // Sent when a flag or flag carrier first touches the zone.
	COutputEvent m_outOnEndTouchFlag; // Sent when a flag or flag carrier stops touching the zone.
	COutputEvent m_outOnDroppedFlag; // Sent when a flag is dropped in the zone.
	COutputEvent m_outOnPickedUpFlag; // Sent when a flag is picked up in the zone.
};

void HandleFlagPickedUpInDetectionZone( CTFPlayer *pPlayer );
void HandleFlagDroppedInDetectionZone( CTFPlayer *pPlayer );
void HandleFlagCapturedInDetectionZone( CTFPlayer *pPlayer );

#endif // FUNC_FLAGDETECTION_ZONE_H



