#ifndef TFMAINMENUPANEL_H
#define TFMAINMENUPANEL_H

#include "tf_menupanelbase.h"

class CAvatarImagePanel;
class CTFAdvButton;

#include "steam/steam_api.h"
#include "steam/isteamhttp.h"
//class CSteamID;
//class ISteamHTTP;
//class HTTPRequestHandle;
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuPanel, CTFMenuPanelBase);

public:
	CTFMainMenuPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFMainMenuPanel();
	bool Init();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void Show();
	void Hide();
	void OnCommand(const char* command);
	void DefaultLayout();
	void GameLayout();
	void SetVersionLabel();
	void PlayMusic();
	void OnNotificationUpdate();

private:
	CExLabel			*m_pVersionLabel;
	CTFAdvButton		*m_pNicknameButton;
	CTFAdvButton		*m_pNotificationButton;
	CAvatarImagePanel	*m_pProfileAvatar;

	char				m_pzMusicLink[64];	
	int					m_nSongGuid;
	float				m_flMusicThink;
	bool				m_bShouldPlay;
	char*				GetRandomMusic();
	char*				GetVersionString();
	
	CSteamID			m_SteamID;
	ISteamHTTP*			m_SteamHTTP;
	HTTPRequestHandle	m_httpRequest;
	float				fPercent;

	bool				bOutdated;
	bool				bChecking;
	bool				bCompleted;
	void				CheckVersion();
	CCallResult<CTFMainMenuPanel, HTTPRequestCompleted_t> m_CallResult;
	void				CHTTPRequestCompleted(HTTPRequestCompleted_t *m_CallResult, bool iofailure);
};


#endif // TFMAINMENUPANEL_H