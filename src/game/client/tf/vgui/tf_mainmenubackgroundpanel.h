#ifndef TFMAINMENUBACKGROUNDPANEL_H
#define TFMAINMENUBACKGROUNDPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"
#include "tf_mainmenurgbpanel.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuBackgroundPanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuBackgroundPanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuBackgroundPanel(vgui::Panel* parent);
	virtual ~CTFMainMenuBackgroundPanel();

	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void OnCommand(const char* command);
	void DefaultLayout();
	void GameLayout();
	MESSAGE_FUNC(VideoReplay, "IntroFinished");

private:
	CTFImagePanel		*m_pBackground;
	CTFVideoPanel		*m_pVideo;
	char				m_pzVideoLink[64];
	char*				GetRandomVideo();
};

#endif // TFMAINMENUBACKGROUNDPANEL_H