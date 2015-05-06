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

private:
	bool			m_bOnlyInGame;
	bool			m_bImageVisible;
	char			pImageIdle[64];
	char			pImageHover[64];
	char			pImageClick[64];
	char			m_szCommand[64];
	char			m_szText[64];
	CTFImagePanel	*pImage;
	CTFButton		*pButton;
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

	void OnCursorExited();
	void OnCursorEntered();
	void OnMousePressed(vgui::MouseCode code);
	void OnMouseReleased(vgui::MouseCode code);

private:
	void SetMouseEnteredState(MouseState flag);

private:
	float			m_flHoverTimeToWait;	// length of time to wait before reporting a "hover" message (-1 = no hover)
	float			m_flHoverTime;			// when should a "hover" message be sent?
	bool			m_bMouseEntered;		// used to track when the mouse is over a button
	CTFMainMenuButton *m_pParent;
	MouseState iState;
};

#endif // TF_MAINMENUBUTTON_H
