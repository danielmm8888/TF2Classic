#ifndef TFMAINMENU_H
#define TFMAINMENU_H

#include "vgui_controls/Frame.h"
#include "vgui_controls/Panel.h"
#include "GameUI/IGameUI.h"
#include "tf_mainmenupanel.h"
#include "tf_testmenupanel.h"

enum MenuPanel 
{
	NONE_MENU,
	MAIN_MENU,
	TEST_MENU,
	COUNT_MENU
};

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

private:
	CTFMainMenuPanel	*MainMenuPanel;
	CTFTestMenuPanel	*TestMenuPanel;
	bool				LoadGameUI();
	bool				InGame();
	IGameUI*			gameui;
};

extern CTFMainMenu *guiroot;

#endif // TFMAINMENU_H