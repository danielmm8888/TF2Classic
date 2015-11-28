//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_DEATHMATCHSTATUS_H
#define TF_HUD_DEATHMATCHSTATUS_H
#ifdef _WIN32
#pragma once
#endif

#include "entity_capture_flag.h"
#include "tf_controls.h"
#include "tf_imagepanel.h"
#include "GameEventListener.h"


//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFHudDeathMatchObjectives : public vgui::EditablePanel, public CGameEventListener
{
	DECLARE_CLASS_SIMPLE( CTFHudDeathMatchObjectives, vgui::EditablePanel );

public:

	CTFHudDeathMatchObjectives( vgui::Panel *parent, const char *name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual bool IsVisible( void );
	virtual void Reset();
	void OnThink();

public: // IGameEventListener:
	virtual void FireGameEvent( IGameEvent *event );

private:
	void UpdateStatus( void );
	void SetPlayingToLabelVisible( bool bVisible );

private:
	float m_flNextThink;
};

#endif	// TF_HUD_DEATHMATCHSTATUS_H
