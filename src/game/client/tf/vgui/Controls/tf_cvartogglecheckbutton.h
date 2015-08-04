//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef tf_cvartogglecheckbutton_H
#define tf_cvartogglecheckbutton_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_advcheckbutton.h"

class CCvarToggleCheckButton : public CTFAdvCheckButton
{
	DECLARE_CLASS_SIMPLE(CCvarToggleCheckButton, CTFAdvCheckButton);

public:
	CCvarToggleCheckButton( vgui::Panel *parent, const char *panelName, const char *text, 
		char const *cvarname );
	~CCvarToggleCheckButton();

	virtual void	SetSelected( bool state );

	virtual void	Paint();

	void			Reset();
	void			ApplyChanges();
	bool			HasBeenModified();
	virtual void	ApplySettings( KeyValues *inResourceData );

private:
	MESSAGE_FUNC( OnButtonChecked, "CheckButtonChecked" );

	char			*m_pszCvarName;
	bool			m_bStartValue;
};

#endif // tf_cvartogglecheckbutton_H
