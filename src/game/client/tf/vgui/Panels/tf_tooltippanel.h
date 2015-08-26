#ifndef TFMAINMENUTOOLTIPPANEL_H
#define TFMAINMENUTOOLTIPPANEL_H

#include "tf_dialogpanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFToolTipPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFToolTipPanel, CTFMenuPanelBase);

public:
	CTFToolTipPanel(vgui::Panel* parent, const char *panelName);
	virtual bool Init();
	virtual ~CTFToolTipPanel();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void Show();
	void Hide();
	void OnCommand(const char* command);
	void DefaultLayout();
	void GameLayout();
	void ShowToolTip(char *sText);
	void HideToolTip();

private:
	char		m_sText[256];
	CExLabel	*m_pText;
};

#endif // TFMAINMENUTOOLTIPPANEL_H