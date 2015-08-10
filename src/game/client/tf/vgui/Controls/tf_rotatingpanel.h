#ifndef TF_ROTATINGPANEL_H
#define TF_ROTATINGPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_imagepanel.h"
#include <vgui/ISurface.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose:  Draws the rotated arrow panels
//-----------------------------------------------------------------------------
class CTFRotationPanel : public ImagePanel
{
public:
	DECLARE_CLASS_SIMPLE(CTFRotationPanel, ImagePanel);

	CTFRotationPanel(vgui::Panel *parent, const char *name);
	virtual void Paint();
	virtual void ApplySettings(KeyValues *inResourceData);
	float GetAngleRotation(void);

private:
	float				flRetVal;
	char				pImage[64];
	CMaterialReference	m_Material;
};


#endif // TF_ROTATINGPANEL_H
