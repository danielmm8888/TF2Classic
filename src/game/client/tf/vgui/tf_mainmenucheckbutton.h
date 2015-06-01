#ifndef TF_MAINMENUCHECKBUTTON_H
#define TF_MAINMENUCHECKBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_mainmenubuttonbase.h"

using namespace vgui;

class CTFCheckButton;
#undef	DEFAULT_IMAGE
#undef	ARMED_IMAGE
#undef	DEPRESSED_IMAGE
#define DEFAULT_IMAGE				"MainMenuNewCheckButtonDefault"
#define ARMED_IMAGE					"MainMenuNewCheckButtonArmed"
#define DEPRESSED_IMAGE				"MainMenuNewCheckButtonDepressed"
#define DEFAULT_CHECKIMAGE			DEFAULT_PATH	"check_icon"
#define ARMED_CHECKIMAGE			DEFAULT_PATH	"check_icon_armed"
#define DEPRESSED_CHECKIMAGE		DEFAULT_PATH	"check_icon_depressed"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuCheckButton : public CTFMainMenuButtonBase
{
	friend CTFCheckButton;
public:
	DECLARE_CLASS_SIMPLE(CTFMainMenuCheckButton, CTFMainMenuButtonBase);

	CTFMainMenuCheckButton(vgui::Panel *parent, const char *panelName, const char *text);
	~CTFMainMenuCheckButton();
	void Init();
	void ApplySettings(KeyValues *inResourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void PerformLayout();

	void SendAnimation(MouseState flag);
	void SetDefaultAnimation();

	void OnTick();
	void OnThink();
	void GetCommandValue();
	void OnVisible();

	bool IsSelected() { return m_bState; };

protected:
	CTFCheckButton		*pButton;
	CTFImagePanel		*pCheckImage;
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
	void SetParent(CTFMainMenuCheckButton *m_pButton) { m_pParent = m_pButton; };
	const char *GetCommandStr() { return m_pParent->GetCommandString(); };
	char *GetCommandValue(bool bState) { return (bState ? m_pParent->m_szValueTrue : m_pParent->m_szValueFalse); };

private:
	CTFMainMenuCheckButton *m_pParent;
};


#endif // TF_MAINMENUCHECKBUTTON_H
