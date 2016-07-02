//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_CONDSTATUS_H
#define TF_HUD_CONDSTATUS_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_hud_objectivestatus.h"

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFPowerupPanel : public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFPowerupPanel, vgui::EditablePanel );

	CTFPowerupPanel( vgui::Panel *parent, const char *name );
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void UpdateStatus( void );

	void SetData( int cond, float dur, float initdur );

	int m_nCond;
	float m_flDuration;
	float m_flInitDuration;

private:
	CTFProgressBar *m_pProgressBar;
};

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFHudCondStatus : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudCondStatus, vgui::EditablePanel );

public:
	CTFHudCondStatus( const char *pElementName );
	~CTFHudCondStatus();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void PerformLayout( void );
	virtual bool ShouldDraw( void );
	virtual void OnTick( void );

private:
	CUtlVector<CTFPowerupPanel *>	m_pPowerups;
};

#endif	// TF_HUD_CONDSTATUS_H
