#ifndef TFMAINMENUPAUSEPANEL_H
#define TFMAINMENUPAUSEPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"
#include "tf_mainmenurgbpanel.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuPausePanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuPausePanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuPausePanel(vgui::Panel* parent);
	virtual ~CTFMainMenuPausePanel();

	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void OnCommand(const char* command);
	void DefaultLayout();
	void GameLayout();

private:
	CTFMainMenuButton	*m_pDisconnectButton;
	CTFMainMenuRGBPanel	*m_pRGBPanel;
};

#endif // TFMAINMENUPAUSEPANEL_H