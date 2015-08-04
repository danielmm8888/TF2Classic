#ifndef TFMAINMENUQUITPANEL_H
#define TFMAINMENUQUITPANEL_H

#include "tf_dialogpanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFQuitDialogPanel : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFQuitDialogPanel, CTFDialogPanelBase);

public:
	CTFQuitDialogPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFQuitDialogPanel();

	void Show();
	void Hide();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
};

#endif // TFTESTMENU_H