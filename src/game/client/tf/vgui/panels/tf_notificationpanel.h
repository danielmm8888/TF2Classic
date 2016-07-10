#ifndef TFMAINMENUNOTIFICATIONPANEL_H
#define TFMAINMENUNOTIFICATIONPANEL_H

#include "tf_dialogpanelbase.h"
#include "controls/tf_advbutton.h"
#include "tf_notificationmanager.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFNotificationPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE( CTFNotificationPanel, CTFMenuPanelBase );

public:
	CTFNotificationPanel( vgui::Panel* parent, const char *panelName );
	virtual bool Init();
	virtual ~CTFNotificationPanel();
	void PerformLayout();
	void ApplySchemeSettings( vgui::IScheme *pScheme );
	void OnThink();
	void OnTick();
	void Show();
	void Hide();
	void OnCommand( const char* command );
	void DefaultLayout();
	void GameLayout();
	void OnNotificationUpdate();
	void UpdateLabels();
	void RemoveCurrent();

private:
	CTFAdvButton	*m_pPrevButton;
	CTFAdvButton	*m_pNextButton;
	CExLabel		*m_pMessageLabel;

	int				m_iMinHeight;
	int				m_iCurrent;
	int				m_iCount;
};

#endif // TFMAINMENUNOTIFICATIONPANEL_H
