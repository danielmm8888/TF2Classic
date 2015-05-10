#ifndef TFMAINMENU_H
#define TFMAINMENU_H

#include "vgui_controls/Panel.h"
#include "GameUI/IGameUI.h"

#include "tf_mainmenubutton.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/Button.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_mainmenubutton.h"
#include "tf_mainmenuvideo.h"
#include "tf_controls.h"

#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include "tf_vgui_video.h"

#include "hud.h"
#include "hudelement.h"
#include "ienginevgui.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CMainMenuPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CMainMenuPanel, vgui::Frame);

public:
	CMainMenuPanel(vgui::VPANEL parent);
	virtual ~CMainMenuPanel();
	IGameUI* GetGameUI();

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PaintBackground();
	bool InGame();
	void OnCommand(const char* command);
	void OnThink();
	void OnTick();
	void DefaultLayout();
	void GameLayout();
	void SetVersionLabel();

private:
	CExLabel			*m_pVersionLabel;
	CTFMainMenuButton	*m_pDisconnectButton;
	CTFImagePanel		*m_pBackground;
	CTFImagePanel		*m_pLogo;
	bool				InGameLayout;
	EditablePanel		*m_pWeaponIcon;
	CTFVideoPanel		*m_pVideo;
	bool				b_ShowVideo;
	float			m_flActionThink;

	bool			LoadGameUI();
	IGameUI*		gameui;
};

extern CMainMenuPanel *guiroot;

#endif // TFMAINMENU_H