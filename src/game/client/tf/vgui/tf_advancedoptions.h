#ifndef TF_ADVANCEDOPSTIONS_H
#define TF_ADVANCEDOPSTIONS_H
#ifdef _WIN32
#pragma once
#endif

using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/EditablePanel.h>

class IAdvPanel
{
public:
	virtual void      Create(vgui::VPANEL parent) = 0;
	virtual void      Destroy(void) = 0;
};

extern IAdvPanel* advpanel;

#endif	// TF_HUD_MENU_WEAPONSET_H