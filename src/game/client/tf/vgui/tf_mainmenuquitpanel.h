#ifndef TFMAINMENUQUITPANEL_H
#define TFMAINMENUQUITPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuQuitPanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuQuitPanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuQuitPanel(vgui::Panel* parent);
	virtual ~CTFMainMenuQuitPanel();

	void Show();
	void Hide();
	void PerformLayout();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
};

#endif // TFTESTMENU_H