//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_BETASTAMP_H
#define TF_HUD_BETASTAMP_H
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
class CTFHudBetaStamp : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudBetaStamp, vgui::EditablePanel );

public:

	CTFHudBetaStamp( const char *pElementName );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout( void );
	virtual bool ShouldDraw( void );
	void OnThink( void );

private:
	vgui::ImagePanel *m_pBetaImage;

	bool			m_bGlowing;
	bool			m_bAnimationIn;
	float			m_flAnimationThink;
};

#endif	// TF_HUD_BETASTAMP_H
