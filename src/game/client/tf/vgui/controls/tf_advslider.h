#ifndef TF_MAINMENU_SCROLLBAR_H
#define TF_MAINMENU_SCROLLBAR_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_advbuttonbase.h"
#include "tf_imagepanel.h"

using namespace vgui;

class CTFScrollButton;
#undef	DEFAULT_BG
#undef	ARMED_BG
#undef	DEPRESSED_BG
#undef	DEFAULT_BORDER
#undef	ARMED_BORDER
#undef	DEPRESSED_BORDER
#define DEFAULT_BG					"AdvSlider"
#define ARMED_BG					"AdvSlider"
#define DEPRESSED_BG				"AdvSlider"
#define DEFAULT_BORDER				"TFFatLineBorder"
#define ARMED_BORDER				"TFFatLineBorderOpaque"
#define DEPRESSED_BORDER			"TFFatLineBorderRedBGOpaque"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAdvSlider : public CTFAdvButtonBase
{
	friend class CTFScrollButton;
public:
	DECLARE_CLASS_SIMPLE(CTFAdvSlider, CTFAdvButtonBase);

	CTFAdvSlider(vgui::Panel *parent, const char *panelName, const char *text);
	~CTFAdvSlider();
	void Init();
	void ApplySettings(KeyValues *inResourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformLayout();

	void SendAnimation(MouseState flag);
	void SetDefaultAnimation();

	void OnThink();
	float GetPercentage();
	float GetValue();
	int GetScrollValue();
	void SetPercentage();
	void SetPercentage(float fPerc);
	void SetValue(float fVal);
	void SetMinMax(float fMin, float fMax) { fMinValue = fMin; fMaxValue = fMax; };
	void SetRange(float fMin, float fMax) { SetMinMax(fMin, fMax); };
	void RunCommand();
	void GetGlobalPosition(Panel *pPanel);
	void UpdateValue();
	bool IsVertical() { return bVertical; };
	int GetPanelWide() { return pBGBorder->GetWide(); };

	CTFScrollButton *GetButton(){ return pButton; };

protected:
	CTFScrollButton	*pButton;
	CExLabel		*pTitleLabel;
	CExLabel		*pValueLabel;
	vgui::EditablePanel *pBGBorder;
	float			fMinValue;
	float			fMaxValue;
	float			fLabelWidth;
	float			fValue;
	bool			bValueVisible;
	bool			bVertical;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFScrollButton : public CTFButtonBase
{
	friend class CTFAdvSlider;
public:
	DECLARE_CLASS_SIMPLE(CTFScrollButton, CTFButtonBase);

	CTFScrollButton(vgui::Panel *parent, const char *panelName, const char *text);

	void ApplySettings(KeyValues *inResourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformLayout();
	void OnCursorExited();
	void OnCursorEntered();
	void OnMousePressed(vgui::MouseCode code);
	void OnMouseReleased(vgui::MouseCode code);
	void SetMouseEnteredState(MouseState flag);
	void SetParent(CTFAdvSlider *m_pButton) { m_pParent = m_pButton; };

private:
	CTFAdvSlider *m_pParent;
};


#endif // TF_MAINMENU_SCROLLBAR_H
