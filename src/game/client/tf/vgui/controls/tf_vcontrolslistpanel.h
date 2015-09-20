//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#if !defined( vcontrolslistpanel_H )
#define vcontrolslistpanel_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/SectionedListPanel.h>

//-----------------------------------------------------------------------------
// Purpose: Special list subclass to handle drawing of trap mode prompt on top of
//			lists client area
//-----------------------------------------------------------------------------
class vcontrolslistpanel : public vgui::SectionedListPanel
{
public:
	// Construction
					vcontrolslistpanel( vgui::Panel *parent, const char *listName );
	virtual			~vcontrolslistpanel();

	// Start/end capturing
	virtual void	StartCaptureMode(vgui::HCursor hCursor = NULL);
	virtual void	EndCaptureMode(vgui::HCursor hCursor = NULL);
	virtual bool	IsCapturing();

	// Set which item should be associated with the prompt
	virtual void	SetItemOfInterest(int itemID);
	virtual int		GetItemOfInterest();

	virtual void	OnMousePressed(vgui::MouseCode code);
	virtual void	OnMouseDoublePressed(vgui::MouseCode code);

	virtual void ApplySettings(KeyValues *inResourceData);
	virtual vgui::HFont GetFont(){ return m_hFont; };
	virtual const char* GetFontString(){ return m_szFont; };

private:
	void ApplySchemeSettings(vgui::IScheme *pScheme);

	// Are we showing the prompt?
	bool			m_bCaptureMode;
	// If so, where?
	int				m_nClickRow;
	// Font to use for showing the prompt
	vgui::HFont				m_hFont;
	char				m_szFont[64];
	// panel used to edit
	class CInlineEditPanel *m_pInlineEditPanel;
	int m_iMouseX, m_iMouseY;

	typedef vgui::SectionedListPanel BaseClass;
};

#endif // vcontrolslistpanel_H