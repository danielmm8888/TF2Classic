#ifndef TFMAINMENU_H
#define TFMAINMENU_H

#include "GameUI/IGameUI.h"
#include "vgui_controls/Frame.h"
#include "tf_hud_statpanel.h"

enum MenuPanel //position in this enum = zpos on the screen
{
	NONE_MENU,
	BACKGROUND_MENU,
	MAIN_MENU,
	PAUSE_MENU,
	SHADEBACKGROUND_MENU, //add popup/additional menus below:		
	LOADOUT_MENU,
	STATSUMMARY_MENU,
	NOTIFICATION_MENU,
	OPTIONSDIALOG_MENU,
	QUIT_MENU,
	COUNT_MENU,

	FIRST_MENU = NONE_MENU + 1
};

#define CURRENT_MENU (!InGame() ? MAIN_MENU : PAUSE_MENU)
#define MAINMENU_ROOT guiroot
#define AutoLayout() (!InGame() ? DefaultLayout() : GameLayout())

struct MainMenuNotification
{
	char* sTitle;
	char* sMessage;
	MainMenuNotification() {};
	MainMenuNotification(char* Title, char* Message) : sTitle(Title), sMessage(Message) {};
};

class CTFMenuPanelBase;
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenu : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CTFMainMenu, vgui::EditablePanel);
	friend CTFMenuPanelBase;

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
	virtual void InvalidatePanelsLayout(bool layoutNow = false, bool reloadScheme = false);
	virtual void LaunchInvalidatePanelsLayout();
	virtual void DefaultLayout();
	virtual void GameLayout();
	virtual bool InGame();
	virtual void SendNotification(MainMenuNotification pMessage);
	virtual MainMenuNotification GetNotification() { return pNotification; };
	void SetStats(CUtlVector<ClassStats_t> &vecClassStats);

private:
	CUtlVector<CTFMenuPanelBase*>		m_pPanels;

	void								AddMenuPanel(CTFMenuPanelBase *m_pPanel, int iPanel);
	CTFMenuPanelBase*					GetMenuPanel(int iPanel);

	bool								LoadGameUI();
	bool								bInGameLayout;
	IGameUI*							gameui;
	MainMenuNotification				pNotification;
	int									m_iStopGameStartupSound;
	int									m_iUpdateLayout;
};

extern CTFMainMenu *guiroot;

#endif // TFMAINMENU_H