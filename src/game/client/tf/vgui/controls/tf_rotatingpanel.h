#ifndef TF_ROTATINGPANEL_H
#define TF_ROTATINGPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include "vgui_controls/EditablePanel.h"
#include <vgui/ISurface.h>

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose:  Draws the rotated arrow panels
//-----------------------------------------------------------------------------
class CTFRotatingImagePanel : public EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE(CTFRotatingImagePanel, EditablePanel);

	CTFRotatingImagePanel(vgui::Panel *parent, const char *name);
	virtual void Paint();
	virtual void ApplySettings(KeyValues *inResourceData);
	float GetAngleRotation(void);

private:
	float				flRetVal;
	char				pImage[64];
	CMaterialReference	m_Material;
};


#endif // TF_ROTATINGPANEL_H
