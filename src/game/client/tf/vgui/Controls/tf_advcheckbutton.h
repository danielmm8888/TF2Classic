#ifndef tf_advcheckbutton_H
#define tf_advcheckbutton_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_advbuttonbase.h"

using namespace vgui;

class CTFCheckButton;
#undef	DEFAULT_BG
#undef	ARMED_BG
#undef	DEPRESSED_BG
#define DEFAULT_BG					"AdvCheckButtonDefault"
#define ARMED_BG					"AdvCheckButtonArmed"
#define DEPRESSED_BG				"AdvCheckButtonDepressed"
#define DEFAULT_CHECKIMAGE			DEFAULT_PATH	"glyph_close_x"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAdvCheckButton : public CTFAdvButtonBase
{
	friend CTFCheckButton;
public:
	DECLARE_CLASS_SIMPLE(CTFAdvCheckButton, CTFAdvButtonBase);

	CTFAdvCheckButton(vgui::Panel *parent, const char *panelName, const char *text);
	~CTFAdvCheckButton();
	virtual void Init();
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PerformLayout();

	virtual void SendAnimation(MouseState flag);
	virtual void SetDefaultAnimation();

	virtual void OnTick();
	virtual void OnThink();
	virtual void GetCommandValue();
	virtual void SetSelected(bool state){ m_bState = state; };
	virtual bool IsSelected() { return m_bState; };

protected:
	CTFCheckButton		*pButton;
	ImagePanel		*pCheckImage;
	char				m_szValueTrue[64];
	char				m_szValueFalse[64];
	char				pDefaultCheckImage[64];
	char				pArmedCheckImage[64];
	char				pDepressedCheckImage[64];
	bool				m_bState;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFCheckButton : public CTFButtonBase
{
public:
	DECLARE_CLASS_SIMPLE(CTFCheckButton, CTFButtonBase);

	CTFCheckButton(vgui::Panel *parent, const char *panelName, const char *text);

	void OnCursorExited();
	void OnCursorEntered();
	void OnMousePressed(vgui::MouseCode code);
	void OnMouseReleased(vgui::MouseCode code);
	void SetMouseEnteredState(MouseState flag);
	void SetParent(CTFAdvCheckButton *m_pButton) { m_pParent = m_pButton; };
	const char *GetCommandStr() { return m_pParent->GetCommandString(); };
	char *GetCommandValue(bool bState) { return (bState ? m_pParent->m_szValueTrue : m_pParent->m_szValueFalse); };

private:
	CTFAdvCheckButton *m_pParent;
};


#endif // tf_advcheckbutton_H
