#ifndef TFTESTMENU_H
#define TFTESTMENU_H

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFTestMenuPanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFTestMenuPanel, CTFMainMenuPanelBase);

public:
	CTFTestMenuPanel(vgui::Panel* parent);
	virtual ~CTFTestMenuPanel();

	void PerformLayout();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
};

#endif // TFTESTMENU_H