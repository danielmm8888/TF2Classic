//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef HUD_TF_CROSSHAIR_H
#define HUD_TF_CROSSHAIR_H
#ifdef _WIN32
#pragma once
#endif

#include "hudelement.h"
#include "hud_crosshair.h"
#include <vgui_controls/Panel.h>

namespace vgui
{
	class IScheme;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudTFCrosshair : public CHudCrosshair
{
public:
	DECLARE_CLASS_SIMPLE(CHudTFCrosshair, CHudCrosshair);

	CHudTFCrosshair(const char *pElementName);

	virtual void Paint();
	virtual void Init();
	virtual bool ShouldDraw();
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );

	virtual void LevelShutdown( void );

	//stub
	void SetCrosshair( CHudTexture *texture, Color& clr );
	void ResetCrosshair() {}

private:
	int					m_iCrosshairTextureID;
	IVguiMatInfo		*m_pCrosshairOverride;
	IVguiMatInfoVar		*m_pFrameVar;				// interface for material frame
	int					m_nNumFrames;				// how many frames this crosshair has

	char				m_szPreviousCrosshair[256];	// name of the current crosshair
	float				m_flAccuracy;
};


// Enable/disable crosshair rendering.
extern ConVar crosshair;


#endif // HUD_TF_CROSSHAIR_H
