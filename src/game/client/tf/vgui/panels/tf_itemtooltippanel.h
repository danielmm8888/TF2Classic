#ifndef TF_ITEMMODELTOOLTIPPANEL_H
#define TF_ITEMMODELTOOLTIPPANEL_H

#include "tf_dialogpanelbase.h"
#include "tf_tooltippanel.h"

class CTFAdvModelPanel;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFItemToolTipPanel : public CTFToolTipPanel
{
	DECLARE_CLASS_SIMPLE(CTFItemToolTipPanel, CTFToolTipPanel);

public:
	CTFItemToolTipPanel(vgui::Panel* parent, const char *panelName);
	virtual bool Init();
	virtual ~CTFItemToolTipPanel();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void Show();
	void Hide();
	void ShowToolTip(CEconItemDefinition *pItemData);
	void HideToolTip();

private:
	int iItemID;
	CExLabel	*m_pTitle;
	CExLabel	*m_pClassName;
	CExLabel	*m_pAttributeText;
	CTFAdvModelPanel *m_pClassModelPanel;
	CUtlVector<CExLabel*> m_pAttributes;
};

#endif // TF_ITEMMODELTOOLTIPPANEL_H