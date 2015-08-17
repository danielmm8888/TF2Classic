//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_IMAGEPANEL_H
#define TF_IMAGEPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"
#include <vgui/IScheme.h>
#include <vgui_controls/ImagePanel.h>
#include "GameEventListener.h"
#include "vgui_controls/ScalableImagePanel.h"

#define MAX_BG_LENGTH		128

class CTFImageColoredPanel : public vgui::ScalableImagePanel
{
public:
	DECLARE_CLASS_SIMPLE( CTFImageColoredPanel, vgui::ScalableImagePanel );

	CTFImageColoredPanel(vgui::Panel *parent, const char *name);

	virtual void PaintBackground();
};


#endif // TF_IMAGEPANEL_H
