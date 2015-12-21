//======= Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: CTF Flag detection trigger.
//
//=============================================================================//

#include "cbase.h"
#include "func_flagdetectionzone.h"
#include "entity_capture_flag.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


BEGIN_DATADESC( CFlagDetectionZone )

	DEFINE_KEYFIELD( m_bShouldAlarm, FIELD_BOOLEAN, "alarm" ),

	// Inputs.
	//DEFINE_INPUTFUNC( FIELD_VOID, "Test", InputTest ),

	// Outputs.
	DEFINE_OUTPUT( m_outOnStartTouchFlag, "OnStartTouchFlag" ),
	DEFINE_OUTPUT( m_outOnEndTouchFlag, "OnEndTouchFlag" ),
	DEFINE_OUTPUT( m_outOnDroppedFlag, "OnDroppedFlag" ),
	DEFINE_OUTPUT( m_outOnPickedUpFlag, "OnPickedUpFlag" )

END_DATADESC()

IMPLEMENT_AUTO_LIST( IFlagDetectionZoneAutoList );

LINK_ENTITY_TO_CLASS( func_flagdetectionzone, CFlagDetectionZone );


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::Spawn( void )
{
	BaseClass::Spawn();

	AddSpawnFlags( SF_TRIGGER_ALLOW_CLIENTS );

	InitTrigger();

	if ( m_bDisabled )
	{
		SetDisabled( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::SetDisabled( bool bDisabled )
{
	m_bDisabled = bDisabled;

	if ( bDisabled )
	{
		BaseClass::Disable();
		//SetTouch( NULL );
	}
	else
	{
		BaseClass::Enable();
		//SetTouch( &CFlagDetectionZone::Touch );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::InputEnable( inputdata_t &inputdata )
{
	SetDisabled( false );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::InputDisable( inputdata_t &inputdata )
{
	SetDisabled( true );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CFlagDetectionZone::EntityIsFlagCarrier( CBaseEntity *pEntity )
{
	CTFPlayer *pPlayer = ToTFPlayer( pEntity );
	if ( pPlayer && pPlayer->HasItem() )
	{
		CCaptureFlag *pFlag = dynamic_cast<CCaptureFlag*>( pPlayer->GetItem() );
		if ( pFlag )
			return true;
	}
	
	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::FlagCaptured( CTFPlayer *pPlayer )
{
	// I have no idea why
	if ( !Q_strcmp( gpGlobals->mapname.ToCStr(), "sd_doomsday" ) )
		return;

	if ( pPlayer && IsTouching( pPlayer ) )
	{
		// Apparently this function is used for giving an achievement in live tf2
		// however since we don't have that, we'll just leave this function as a stub
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void HandleFlagCapturedInDetectionZone( CTFPlayer *pPlayer )
{
	for ( int i = 0; i < IFlagDetectionZoneAutoList::AutoList().Count(); i++ )
	{
		CFlagDetectionZone *pZone = static_cast< CFlagDetectionZone *>( IFlagDetectionZoneAutoList::AutoList()[i] );
		if ( pZone && !pZone->IsDisabled() )
		{
			pZone->FlagCaptured( pPlayer );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::FlagDropped( CTFPlayer *pPlayer )
{
	if ( pPlayer && IsTouching( pPlayer ) )
	{
		m_outOnDroppedFlag.FireOutput( this, this);
		m_outOnEndTouchFlag.FireOutput( this, this );
	};
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void HandleFlagDroppedInDetectionZone( CTFPlayer *pPlayer )
{
	for ( int i = 0; i < IFlagDetectionZoneAutoList::AutoList().Count(); i++ )
	{
		CFlagDetectionZone *pZone = static_cast< CFlagDetectionZone *>( IFlagDetectionZoneAutoList::AutoList()[i] );
		if ( pZone && !pZone->IsDisabled() )
		{
			pZone->FlagDropped( pPlayer );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::StartTouch( CBaseEntity *pOther )
{
	if ( IsDisabled() || !pOther )
		return;

	BaseClass::StartTouch( pOther );

	if ( IsTouching( pOther ) && EntityIsFlagCarrier( pOther ) )
	{
		m_outOnStartTouchFlag.FireOutput( this, this );
		IGameEvent *pEvent = gameeventmanager->CreateEvent( "flag_carried_in_detection_zone" );
		if ( pEvent )
		{
			gameeventmanager->FireEventClientSide( pEvent );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::EndTouch( CBaseEntity *pOther )
{
	if ( !pOther )
		return;

	if ( IsTouching( pOther ) && EntityIsFlagCarrier( pOther ) )
	{
		m_outOnEndTouchFlag.FireOutput( this, this );
	}

	BaseClass::EndTouch( pOther );
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::FlagPickedUp( CTFPlayer *pPlayer )
{
	if ( pPlayer && IsTouching( pPlayer ) )
	{
		m_outOnPickedUpFlag.FireOutput( this, this );
		m_outOnStartTouchFlag.FireOutput( this, this );
	};
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void HandleFlagPickedUpInDetectionZone( CTFPlayer *pPlayer )
{
	for ( int i = 0; i < IFlagDetectionZoneAutoList::AutoList().Count(); i++ )
	{
		CFlagDetectionZone *pZone = static_cast< CFlagDetectionZone *>( IFlagDetectionZoneAutoList::AutoList()[i] );
		if ( pZone && !pZone->IsDisabled() )
		{
			pZone->FlagPickedUp( pPlayer );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CFlagDetectionZone::InputTest(inputdata_t &inputdata)
{
	
}