#ifndef TFMAINMENUSHADEBACKGROUNDPANEL_H
#define TFMAINMENUSHADEBACKGROUNDPANEL_H

#include "tf_mainmenupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuShadeBackgroundPanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuShadeBackgroundPanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuShadeBackgroundPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFMainMenuShadeBackgroundPanel();
	void Show();
	void Hide();
	void PerformLayout();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:

};

#endif // TFMAINMENUSHADEBACKGROUNDPANEL_H