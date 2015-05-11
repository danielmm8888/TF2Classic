//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef TF_MAINMENUBUTTON_H
#define TF_MAINMENUBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_controls.h"

using namespace vgui;

class CTFButton;

enum MouseState
{
	MOUSE_DEFAULT,
	MOUSE_ENTERED,
	MOUSE_EXITED,
	MOUSE_PRESSED
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuButton : public CExButton
{
	friend CTFButton;
public:
	DECLARE_CLASS_SIMPLE(CTFMainMenuButton, CExButton);

	CTFMainMenuButton(vgui::Panel *parent, const char *panelName, const char *text);

	void ApplySettings(KeyValues *inResourceData);
	void ApplySchemeSettings(vgui::IScheme *pScheme);

	void SendAnimation(MouseState flag);
	void SetDefaultAnimation();

	bool OnlyInGame(void){ return m_bOnlyInGame; };
	bool OnlyAtMenu(void){ return m_bOnlyAtMenu; };
	void OnThink();

private:
	bool			m_bOnlyInGame;
	bool			m_bOnlyAtMenu;
	bool			m_bImageVisible;
	bool			m_bBorderVisible;
	float			m_fXShift;
	float			m_fYShift;
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
	CTFButton		*pButton;
	vgui::Label::Alignment GetAlignment(char* m_szAlignment);
};


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFButton : public CExButton
{
public:
	DECLARE_CLASS_SIMPLE(CTFButton, CExButton);

	CTFButton(vgui::Panel *parent, const char *panelName, const char *text);

	void SetParent(CTFMainMenuButton *m_pButton) { m_pParent = m_pButton; };
	void ApplySettings(KeyValues *inResourceData);

	// Set armed button border attributes.
	virtual void SetArmedBorder(vgui::IBorder *border);
	virtual void SetSelectedBorder(vgui::IBorder *border);
	virtual void ApplySchemeSettings(IScheme *pScheme);
	// Get button border attributes.
	virtual IBorder *GetBorder(bool depressed, bool armed, bool selected, bool keyfocus);

	void OnCursorExited();
	void OnCursorEntered();
	void OnMousePressed(vgui::MouseCode code);
	void OnMouseReleased(vgui::MouseCode code);

private:
	void SetMouseEnteredState(MouseState flag);

private:
	IBorder			  *_armedBorder;
	IBorder			  *_selectedBorder;

	CTFMainMenuButton *m_pParent;
	MouseState iState;
};


#endif // TF_MAINMENUBUTTON_H
