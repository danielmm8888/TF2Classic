#ifndef TFMAINMENULOADOUTPANEL_H
#define TFMAINMENULOADOUTPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuLoadoutPanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuLoadoutPanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuLoadoutPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFMainMenuLoadoutPanel();
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