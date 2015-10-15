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

struct powerupinfo_t
{
	powerupinfo_t(int id, float dur, float initdur)
	{
		ID = id;
		fDuration = dur;
		fInitDuration = initdur;
	}
	int ID;
	float fDuration;
	float fInitDuration;
};

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFHudCondStatus : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudCondStatus, vgui::EditablePanel );

public:
	CTFHudCondStatus( const char *pElementName );
	virtual ~CTFHudCondStatus();

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void ApplySettings( KeyValues *inResourceData );
	virtual void PerformLayout();
	virtual bool ShouldDraw( void );

private:	
	void UpdateStatus( void );
	CUtlVector<EditablePanel*>	m_pPowerups;
	CUtlVector<powerupinfo_t>	powerups;
};

#endif	// TF_HUD_CONDSTATUS_H
