#ifndef TFMAINMENU_H
#define TFMAINMENU_H

#include "vgui_controls/Frame.h"
#include "vgui_controls/Panel.h"
#include "GameUI/IGameUI.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenupausepanel.h"
#include "tf_mainmenubackgroundpanel.h"
#include "tf_mainmenushadebackgroundpanel.h"
#include "tf_mainmenuoptionspanel.h"
#include "tf_mainmenuquitpanel.h"

enum MenuPanel //position in this enum = zpos on the screen
{
	NONE_MENU,
	BACKGROUND_MENU,
	MAIN_MENU,
	PAUSE_MENU,
	SHADEBACKGROUND_MENU,
	OPTIONS_MENU,
	QUIT_MENU,
	COUNT_MENU,
	FIRST_MENU = NONE_MENU + 1
};

#define CURRENT_MENU (!InGame() ? MAIN_MENU : PAUSE_MENU)

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenu : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CTFMainMenu, vgui::EditablePanel);

public:
	CTFMainMenu(vgui::VPANEL parent);
	virtual ~CTFMainMenu();
	IGameUI*	 GetGameUI();
	virtual void PerformLayout();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PaintBackground();
	virtual void OnCommand(const char* command);
	virtual void OnThink();
	virtual void OnTick();
	virtual void ShowPanel(MenuPanel iPanel);
	virtual void HidePanel(MenuPanel iPanel);
	virtual void DefaultLayout();
	virtual void GameLayout();
	virtual bool InGame();

private:
	CUtlVector<CTFMainMenuPanelBase*>	m_pPanels;

	CTFMainMenuPanel					*m_pMainMenuPanel;
	CTFMainMenuPanel					*m_pPauseMenuPanel;
	CTFMainMenuBackgroundPanel			*m_pBackgroundMenuPanel;
	CTFMainMenuShadeBackgroundPanel		*m_pShadeBackgroundMenuPanel;
	CTFMainMenuQuitPanel				*m_pQuitMenuPanel;
	CTFMainMenuOptionsPanel				*m_pOptionsMenuPanel;

	void								AddMenuPanel(CTFMainMenuPanelBase *m_pPanel, int iPanel);
	CTFMainMenuPanelBase*				GetMenuPanel(int iPanel);

	bool								LoadGameUI();
	bool								bInGameLayout;
	IGameUI*							gameui;
};

extern CTFMainMenu *guiroot;

#endif // TFMAINMENU_H