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

#include <vgui_controls/Frame.h>
#include "tf_mainmenuscriptobject.h"
#include <vgui/KeyCode.h>

#include "vgui_controls/Panel.h"
#include "tf_mainmenupanelbase.h"
#include "tf_mainmenuscriptobject.h"
#include "vgui_controls/CheckButton.h"

//-----------------------------------------------------------------------------
// Purpose: Displays a game-specific list of options
//-----------------------------------------------------------------------------
class CTFMainMenuOptionsPanel : public CTFMainMenuPanelBase 
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuOptionsPanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuOptionsPanel(vgui::Panel *parent, const char *panelName);
	~CTFMainMenuOptionsPanel();
	bool Init();
	void Show();
	void Hide();
	void PerformLayout();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:
	void CreateControls();
	void DestroyControls();
	void GatherCurrentValues();
	void SaveValues();

	CInfoDescription *m_pDescription;

	mpcontrol_t *m_pList;

	CPanelListPanel *m_pListPanel;
	virtual void OnCommand(const char *command);
	virtual void OnKeyCodeTyped(vgui::KeyCode code);
};


#endif // MULTIPLAYERADVANCEDDIALOG_H