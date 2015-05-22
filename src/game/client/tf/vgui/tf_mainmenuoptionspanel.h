#ifndef TFMAINMENUOPTIONSPANEL_H
#define TFMAINMENUOPTIONSPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuOptionsPanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuOptionsPanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuOptionsPanel(vgui::Panel* parent);
	virtual ~CTFMainMenuOptionsPanel();

	void Show();
	void Hide();
	void PerformLayout();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
};

#endif // TFTESTMENU_H