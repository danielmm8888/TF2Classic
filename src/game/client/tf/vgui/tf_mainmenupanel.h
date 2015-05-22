#ifndef TFMAINMENUPANEL_H
#define TFMAINMENUPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"
#include "tf_mainmenurgbpanel.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuPanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuPanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuPanel(vgui::Panel* parent);
	virtual ~CTFMainMenuPanel();

	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void OnCommand(const char* command);
	void DefaultLayout();
	void GameLayout();
	void SetVersionLabel();
	void PlayMusic();

private:
	CExLabel			*m_pVersionLabel;
	char				m_pzMusicLink[64];
	float				m_flActionThink;
	float				m_flMusicThink;
	float				m_flAnimationThink;
	bool				m_bAnimationIn;
	bool				m_bMusicPlay;
	char*				GetRandomMusic();

	CTFMainMenuRGBPanel	*m_pRGBPanel;
};

#endif // TFMAINMENUPANEL_H