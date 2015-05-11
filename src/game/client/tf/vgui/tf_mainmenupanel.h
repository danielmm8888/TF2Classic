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

private:
	CExLabel			*m_pVersionLabel;
	CTFMainMenuButton	*m_pDisconnectButton;
	CTFImagePanel		*m_pBackground;
	CTFImagePanel		*m_pLogo;
	EditablePanel		*m_pWeaponIcon;
	CTFVideoPanel		*m_pVideo;
	char				m_pzVideoLink[64];
	bool				b_ShowVideo;
	float				m_flActionThink;
	float				m_flAnimationThink;
	bool				m_bAnimationIn;
	char*				GetRandomVideo();
};

#endif // TFMAINMENUPANEL_H