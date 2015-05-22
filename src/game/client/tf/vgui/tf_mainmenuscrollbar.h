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

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuScrollBar : public CTFMainMenuButtonBase
{
	friend CTFScrollButton;
public:
	DECLARE_CLASS_SIMPLE(CTFMainMenuScrollBar, CTFMainMenuButtonBase);

	CTFMainMenuScrollBar(vgui::Panel *parent, const char *panelName, const char *text);

	void ApplySettings(KeyValues *inResourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);

	void SendAnimation(MouseState flag);
	void SetDefaultAnimation();

	void OnTick();
	float GetPercentage();
	float GetValue();
	void SetPercentage();
	void SetPercentage(float fPerc);
	void SetMinMax(float fMin, float fMax) { fMinValue = fMin; fMaxValue = fMax; };
	void RunCommand();
	void GetGlobalPosition(Panel *pPanel);

protected:
	CTFScrollButton	*pButton;
	float	fMinValue;
	float	fMaxValue;
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
