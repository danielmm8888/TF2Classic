#ifndef TF_MAINMENU_SCROLLBAR_H
#define TF_MAINMENU_SCROLLBAR_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_mainmenubuttonbase.h"

using namespace vgui;

class CTFScrollButton;
#undef	DEFAULT_IMAGE
#undef	ARMED_IMAGE
#undef	DEPRESSED_IMAGE
#undef	DEFAULT_BORDER
#undef	ARMED_BORDER
#undef	DEPRESSED_BORDER
#define DEFAULT_IMAGE				DEFAULT_PATH	"scroll_bg"
#define ARMED_IMAGE					DEFAULT_PATH	"scroll_bg"
#define DEPRESSED_IMAGE				DEFAULT_PATH	"scroll_bg"
#define DEFAULT_BORDER				"TFFatLineBorder"
#define ARMED_BORDER				"TFFatLineBorderOpaque"
#define DEPRESSED_BORDER			"TFFatLineBorderRedBGOpaque"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuScrollBar : public CTFMainMenuButtonBase
{
	friend CTFScrollButton;
public:
	DECLARE_CLASS_SIMPLE(CTFMainMenuScrollBar, CTFMainMenuButtonBase);

	CTFMainMenuScrollBar(vgui::Panel *parent, const char *panelName, const char *text);
	~CTFMainMenuScrollBar();
	void Init();
	void ApplySettings(KeyValues *inResourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformLayout();

	void SendAnimation(MouseState flag);
	void SetDefaultAnimation();

	void OnThink();
	float GetPercentage();
	int GetValue();
	int GetScrollValue();
	void SetPercentage();
	void SetPercentage(float fPerc);
	void SetValue(float fVal);
	void SetMinMax(float fMin, float fMax) { fMinValue = fMin; fMaxValue = fMax; };
	void RunCommand();
	void GetGlobalPosition(Panel *pPanel);
	void UpdateValue();

protected:
	CTFScrollButton	*pButton;
	CExLabel		*pTitleLabel;
	CExLabel		*pValueLabel;
	float			fMinValue;
	float			fMaxValue;
	float			fLabelWidth;
	float			fValue;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFScrollButton : public CTFButtonBase
{
public:
	DECLARE_CLASS_SIMPLE(CTFScrollButton, CTFButtonBase);

	CTFScrollButton(vgui::Panel *parent, const char *panelName, const char *text);

	void OnCursorExited();
	void OnCursorEntered();
	void OnMousePressed(vgui::MouseCode code);
	void OnMouseReleased(vgui::MouseCode code);
	void SetMouseEnteredState(MouseState flag);
	void SetParent(CTFMainMenuScrollBar *m_pButton) { m_pParent = m_pButton; };
	char *GetCommandStr() { return m_pParent->m_szCommand; };

private:
	CTFMainMenuScrollBar *m_pParent;
};


#endif // TF_MAINMENU_SCROLLBAR_H
