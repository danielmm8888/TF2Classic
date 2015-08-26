#ifndef TFMAINMENUBACKGROUNDPANEL_H
#define TFMAINMENUBACKGROUNDPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_menupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFBackgroundPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFBackgroundPanel, CTFMenuPanelBase);

public:
	CTFBackgroundPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFBackgroundPanel();
	bool Init();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void OnCommand(const char* command);
	void DefaultLayout();
	void GameLayout();
	MESSAGE_FUNC(VideoReplay, "IntroFinished");

private:
	CTFVideoPanel		*m_pVideo;
	char				m_pzVideoLink[64];
	char*				GetRandomVideo(bool bWidescreen);
};

#endif // TFMAINMENUBACKGROUNDPANEL_H