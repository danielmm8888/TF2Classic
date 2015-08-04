#ifndef tf_advbutton_H
#define tf_advbutton_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_advbuttonbase.h"

using namespace vgui;

class CTFButton;


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAdvButton : public CTFAdvButtonBase
{
	friend CTFButton;
public:
	DECLARE_CLASS_SIMPLE(CTFAdvButton, CTFAdvButtonBase);

	CTFAdvButton(vgui::Panel *parent, const char *panelName, const char *text);
	~CTFAdvButton();
	void Init();

	void ApplySettings(KeyValues *inResourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformLayout();

	void SendAnimation(MouseState flag);
	void SetDefaultAnimation();
	void SetText(const char *tokenName);
	void SetCommand(const char *command);
	void SetGlowing(bool Glowing);
	void SetShouldScaleImage(bool state) { m_bScaleImage = state; };
	bool GetShouldScaleImage() { return m_bScaleImage; };
	void SetBorderVisible(bool bVisible);

	void OnTick();

protected:
	CTFButton		*pButton;
	float			m_fXShift;
	float			m_fYShift;
	bool			bGlowing;

	float			m_flActionThink;
	float			m_flAnimationThink;
	bool			m_bAnimationIn;
	bool			m_bScaleImage;
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
	void SetParent(CTFAdvButton *m_pButton) { m_pParent = m_pButton; };
	char *GetCommandStr() { return m_pParent->m_szCommand; };

private:
	CTFAdvButton *m_pParent;
};


#endif // tf_advbutton_H
