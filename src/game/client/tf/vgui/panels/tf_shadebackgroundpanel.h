#ifndef TFMAINMENUSHADEBACKGROUNDPANEL_H
#define TFMAINMENUSHADEBACKGROUNDPANEL_H

#include "tf_menupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFShadeBackgroundPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFShadeBackgroundPanel, CTFMenuPanelBase);

public:
	CTFShadeBackgroundPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFShadeBackgroundPanel();
	void Show();
	void Hide();
	void PerformLayout();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:

};

#endif // TFMAINMENUSHADEBACKGROUNDPANEL_H