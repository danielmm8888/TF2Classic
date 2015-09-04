#ifndef TFMAINMENU_H
#define TFMAINMENU_H

#include "GameUI/IGameUI.h"
#include "vgui_controls/Frame.h"
#include "tf_hud_statpanel.h"
#include "steam/steam_api.h"
#include "steam/isteamhttp.h"

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
	TOOLTIP_MENU,
	COUNT_MENU,

	FIRST_MENU = NONE_MENU + 1
};

#define CURRENT_MENU (!InGame() ? MAIN_MENU : PAUSE_MENU)
#define MAINMENU_ROOT guiroot
#define AutoLayout() (!InGame() ? DefaultLayout() : GameLayout())

struct MainMenuNotification
{
	char sTitle[64];
	char sMessage[128];
	bool bUnread;
	MainMenuNotification() { bUnread = true; };
	MainMenuNotification(char* Title, char* Message)
	{ 
		Q_snprintf(sTitle, sizeof(sTitle), Title);
		Q_snprintf(sMessage, sizeof(sMessage), Message);
		bUnread = true; 
	};
};

class CTFMenuPanelBase;
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenu : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CTFMainMenu, vgui::EditablePanel);
	friend class CTFMenuPanelBase;

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
	virtual void ShowPanel(MenuPanel iPanel, bool bShowSingle = false);
	virtual void HidePanel(MenuPanel iPanel);
	virtual void InvalidatePanelsLayout(bool layoutNow = false, bool reloadScheme = false);
	virtual void LaunchInvalidatePanelsLayout();
	virtual void DefaultLayout();
	virtual void GameLayout();
	virtual bool InGame();
	virtual void SendNotification(MainMenuNotification pMessage);
	virtual MainMenuNotification *GetNotification(int iIndex) { return &pNotifications[iIndex]; };
	virtual int GetNotificationsCount() { return pNotifications.Count(); };
	virtual int GetUnreadNotificationsCount();
	virtual void RemoveNotification(int iIndex);
	virtual void SetStats(CUtlVector<ClassStats_t> &vecClassStats);
	virtual void ShowToolTip(char* sText);
	virtual void HideToolTip();
	virtual char*GetVersionString();
	virtual void CheckMessage(bool Version = false);
	virtual bool IsOutdated() { return bOutdated; };
	//virtual void CheckVersion();

private:
	CUtlVector<CTFMenuPanelBase*>		m_pPanels;

	void								AddMenuPanel(CTFMenuPanelBase *m_pPanel, int iPanel);
	CTFMenuPanelBase*					GetMenuPanel(int iPanel);

	bool								LoadGameUI();
	bool								bInGameLayout;
	IGameUI*							gameui;
	CUtlVector<MainMenuNotification>	pNotifications;
	int									m_iStopGameStartupSound;
	int									m_iUpdateLayout;



	ISteamHTTP*			m_SteamHTTP;
	HTTPRequestHandle	m_httpRequest;
	bool				bOutdated;
	bool				bCompleted;
	bool				pVersionCheck;
	float				fLastCheck;
	char				m_pzLastMessage[128];
	void				OnMessageCheckCompleted(const char* pMessage);
	void				OnVersionCheckCompleted(const char* pMessage);
	CCallResult<CTFMainMenu, HTTPRequestCompleted_t> m_CallResult;
	void				OnHTTPRequestCompleted(HTTPRequestCompleted_t *m_CallResult, bool iofailure);
};
float toProportionalWide(float iWide);
float toProportionalTall(float iTall);

extern CTFMainMenu *guiroot;

#endif // TFMAINMENU_H