#ifndef TF_MAINMENUBUTTON_H
#define TF_MAINMENUBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_mainmenubuttonbase.h"

using namespace vgui;

class CTFButton;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuButton : public CTFMainMenuButtonBase
{
	friend CTFButton;
public:
	DECLARE_CLASS_SIMPLE(CTFMainMenuButton, CTFMainMenuButtonBase);

	CTFMainMenuButton(vgui::Panel *parent, const char *panelName, const char *text);
	~CTFMainMenuButton();
	void Init();

	void ApplySettings(KeyValues *inResourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformLayout();

	void SendAnimation(MouseState flag);
	void SetDefaultAnimation();
	void SetText(const char *tokenName);
	void SetCommand(const char *command);

	void OnTick();

protected:
	CTFButton		*pButton;
	float			m_fXShift;
	float			m_fYShift;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFButton : public CTFButtonBase
{
public:
	DECLARE_CLASS_SIMPLE(CTFButton, CTFButtonBase);

	CTFButton(vgui::Panel *parent, const char *panelName, const char *text);

	void OnCursorExited();
	void OnCursorEntered();
	void OnMousePressed(vgui::MouseCode code);
	void OnMouseReleased(vgui::MouseCode code);
	void SetMouseEnteredState(MouseState flag);
	void SetParent(CTFMainMenuButton *m_pButton) { m_pParent = m_pButton; };
	char *GetCommandStr() { return m_pParent->m_szCommand; };

private:
	CTFMainMenuButton *m_pParent;
};


#endif // TF_MAINMENUBUTTON_H
