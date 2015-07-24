#ifndef TFMAINMENURGBPANEL_H
#define TFMAINMENURGBPANEL_H

#include "tf_menupanelbase.h"

class CTFAdvSlider;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFRGBPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFRGBPanel, CTFMenuPanelBase);

public:
	CTFRGBPanel(vgui::Panel* parent, const char *panelName);
	bool Init();
	virtual ~CTFRGBPanel();

	void PerformLayout();
	void OnCommand(const char* command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:

	CTFAdvSlider	*m_pRedScrollBar;
	CTFAdvSlider	*m_pGrnScrollBar;
	CTFAdvSlider	*m_pBluScrollBar;
	vgui::ImagePanel		*m_pColorBG;
};

#endif // TFMAINMENURGBPANEL_H