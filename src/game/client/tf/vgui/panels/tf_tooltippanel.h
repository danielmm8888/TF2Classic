#ifndef TF_MAINMENUTOOLTIPPANEL_H
#define TF_MAINMENUTOOLTIPPANEL_H

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
	void Show();
	void Hide();
	void ShowToolTip(char *sText);
	void HideToolTip();

protected:
	char		m_sText[256];
	CExLabel	*m_pText;
};

#endif // TF_MAINMENUTOOLTIPPANEL_H