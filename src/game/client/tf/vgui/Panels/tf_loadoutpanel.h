#ifndef TFMAINMENULOADOUTPANEL_H
#define TFMAINMENULOADOUTPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_menupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFLoadoutPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFLoadoutPanel, CTFMenuPanelBase);

public:
	CTFLoadoutPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFLoadoutPanel();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void OnCommand(const char* command);
	void Hide();
	void Show();
	void DefaultLayout();
	void GameLayout();
};

#endif // TFMAINMENULOADOUTPANEL_H