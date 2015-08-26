//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef LABELEDCOMMANDCOMBOBOX_H
#define LABELEDCOMMANDCOMBOBOX_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/ComboBox.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Panel.h>
#include "utlvector.h"

class CCvarComboBox : public vgui::ComboBox
{
	DECLARE_CLASS_SIMPLE( CCvarComboBox, vgui::ComboBox );

public:
	CCvarComboBox(vgui::Panel *parent, const char *panelName);
	~CCvarComboBox();

	virtual void DeleteAllItems();
	virtual void AddItem(char const *text, char const *engineCommand);
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void PerformLayout(); 
	virtual void Paint();
	const char *GetActiveItemCommand();

	void			ApplyChanges();
	void			Reset();
	bool			HasBeenModified();
	
	enum
	{
		MAX_NAME_LEN = 256,
		MAX_COMMAND_LEN = 256
	};
	
private:
	MESSAGE_FUNC_CHARPTR( OnTextChanged, "TextChanged", text );

	char	m_szCvarName[64];
	int		m_iCurrentSelection;
	int		m_iStartSelection;
};

#endif // LABELEDCOMMANDCOMBOBOX_H
