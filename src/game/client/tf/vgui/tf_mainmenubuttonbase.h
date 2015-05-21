#ifndef TF_MAINMENUBUTTONBASE_H
#define TF_MAINMENUBUTTONBASE_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_controls.h"

using namespace vgui;

class CTFButtonBase;

enum MouseState
{
	MOUSE_DEFAULT,
	MOUSE_ENTERED,
	MOUSE_EXITED,
	MOUSE_PRESSED,
	MOUSE_RELEASED
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuButtonBase : public CExButton
{
	friend CTFButtonBase;
public:
	DECLARE_CLASS_SIMPLE(CTFMainMenuButtonBase, CExButton);

	CTFMainMenuButtonBase(vgui::Panel *parent, const char *panelName, const char *text);

	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

	virtual void SendAnimation(MouseState flag);
	virtual void SetDefaultAnimation();

	virtual void OnThink();

protected:
	bool			m_bImageVisible;
	bool			m_bBorderVisible;
	char			pDefaultImage[64];
	char			pArmedImage[64];
	char			pDepressedImage[64];
	char			pDefaultBorder[64];
	char			pArmedBorder[64];
	char			pDepressedBorder[64];
	char			pDefaultText[64];
	char			pArmedText[64];
	char			pDepressedText[64];
	char			m_szCommand[64];
	char			m_szText[64];
	char			m_szTextAlignment[64];
	CTFImagePanel	*pImage;
	virtual			vgui::Label::Alignment GetAlignment(char* m_szAlignment);
	//CTFButtonBase	*pButton;
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFButtonBase : public CExButton
{
public:
	DECLARE_CLASS_SIMPLE(CTFButtonBase, CExButton);

	CTFButtonBase(vgui::Panel *parent, const char *panelName, const char *text);

	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(IScheme *pScheme);

	// Set armed button border attributes.
	virtual void SetArmedBorder(vgui::IBorder *border);
	virtual void SetSelectedBorder(vgui::IBorder *border);
	// Get button border attributes.
	virtual IBorder *GetBorder(bool depressed, bool armed, bool selected, bool keyfocus);

	virtual void OnCursorExited();
	virtual void OnCursorEntered();
	virtual void OnMousePressed(vgui::MouseCode code);
	virtual void OnMouseReleased(vgui::MouseCode code);
	virtual MouseState GetState() { return iState; };
	//virtual void SetParent(CTFMainMenuButtonBase *m_pButton) { m_pParent = m_pButton; };
	//virtual char *GetCommandStr() { return m_pParent->m_szCommand; };

protected:
	virtual void	SetMouseEnteredState(MouseState flag);
	IBorder			*_armedBorder;
	IBorder			*_selectedBorder;
	MouseState		iState;

//private:
//	CTFMainMenuButtonBase *m_pParent;
};


#endif // TF_MAINMENUBUTTONBASE_H
