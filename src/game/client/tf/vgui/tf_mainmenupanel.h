#ifndef TFMAINMENUPANEL_H
#define TFMAINMENUPANEL_H

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"

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
	MESSAGE_FUNC(VideoReplay, "IntroFinished");

private:
	CExLabel			*m_pVersionLabel;
	CTFMainMenuButton	*m_pDisconnectButton;
	CTFImagePanel		*m_pBackground;
	CTFImagePanel		*m_pLogo;
	EditablePanel		*m_pWeaponIcon;
	CTFVideoPanel		*m_pVideo;
	char				m_pzVideoLink[64];
	char				m_pzMusicLink[64];
	float				m_flActionThink;
	float				m_flMusicThink;
	float				m_flAnimationThink;
	bool				m_bAnimationIn;
	char*				GetRandomVideo();
	char*				GetRandomMusic();
	bool				m_bMusicPlay;
};

#endif // TFMAINMENUPANEL_H