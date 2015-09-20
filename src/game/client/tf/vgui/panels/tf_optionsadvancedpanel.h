//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef MULTIPLAYERADVANCEDDIALOG_H
#define MULTIPLAYERADVANCEDDIALOG_H
#ifdef _WIN32
#pragma once
#endif


#include "tf_dialogpanelbase.h"

class CInfoDescription;
class mpcontrol_t;

//-----------------------------------------------------------------------------
// Purpose: Displays a game-specific list of options
//-----------------------------------------------------------------------------
class CTFOptionsAdvancedPanel : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFOptionsAdvancedPanel, CTFDialogPanelBase);

public:
	CTFOptionsAdvancedPanel(vgui::Panel *parent, const char *panelName);
	~CTFOptionsAdvancedPanel();
	bool Init();
	void OnCommand(const char *command);
	void OnKeyCodeTyped(vgui::KeyCode code);
	void OnApplyChanges();

protected:
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void CreateControls();
	void DestroyControls();
	void GatherCurrentValues();
	void SaveValues();

	CInfoDescription *m_pDescription;

	mpcontrol_t *m_pList;
};


#endif // MULTIPLAYERADVANCEDDIALOG_H