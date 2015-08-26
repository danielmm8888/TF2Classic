//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "tf_cvarcombobox.h"
#include "tf_advbuttonbase.h"
#include <KeyValues.h>
#include <vgui/ILocalize.h>

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

vgui::Panel *CCvarComboBox_Factory()
{
	return new CCvarComboBox(NULL, "CCvarComboBox");
}
DECLARE_BUILD_FACTORY_CUSTOM(CCvarComboBox, CCvarComboBox_Factory);

CCvarComboBox::CCvarComboBox( vgui::Panel *parent, const char *panelName ) : vgui::ComboBox( parent, panelName, 6, false )
{
	AddActionSignalTarget(this);
	m_iCurrentSelection = -1;
	m_iStartSelection = -1;
	Q_strncpy(m_szCvarName, EMPTY_STRING, sizeof(m_szCvarName));
}

CCvarComboBox::~CCvarComboBox( void )
{
}

void CCvarComboBox::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	Q_strncpy(m_szCvarName, inResourceData->GetString("cvar_name", EMPTY_STRING), sizeof(m_szCvarName));

	char m_szFont[32];
	Q_strncpy(m_szFont, inResourceData->GetString("font", EMPTY_STRING), sizeof(m_szFont));
	SetFont(GETSCHEME()->GetFont(m_szFont, IsProportional()));

	ConVarRef var(m_szCvarName);
	if (!var.IsValid())
		return;

	m_iStartSelection = var.GetInt();
	m_iCurrentSelection = var.GetInt();
}

void CCvarComboBox::PerformLayout()
{
	BaseClass::PerformLayout();
	ActivateItem(m_iStartSelection - 1);
};

void CCvarComboBox::DeleteAllItems()
{
	BaseClass::DeleteAllItems();
}

void CCvarComboBox::AddItem( char const *text, char const *engineCommand )
{
	BaseClass::AddItem(text, NULL);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarComboBox::Paint()
{
	// Get engine's current value
	//	float curvalue = engine->pfnGetCvarFloat( m_szCvarName );
	ConVarRef var(m_szCvarName);
	if (!var.IsValid())
		return;
	int curvalue = var.GetInt();

	// did it get changed from under us?
	if (curvalue != m_iCurrentSelection)
	{
		m_iStartSelection = curvalue;
		m_iCurrentSelection = curvalue;

		ActivateItem(m_iStartSelection - 1);
	}

	BaseClass::Paint();
}

void CCvarComboBox::OnTextChanged( char const *text )
{
	m_iCurrentSelection = GetActiveItem() + 1;
	if (HasBeenModified())
	{
		PostActionSignal(new KeyValues("ControlModified"));
		char szCommand[MAX_PATH];
		Q_snprintf(szCommand, sizeof(szCommand), "%s %i", m_szCvarName, GetActiveItem() + 1);
		engine->ExecuteClientCmd(szCommand);
		m_iStartSelection = m_iCurrentSelection;
	}
}

const char *CCvarComboBox::GetActiveItemCommand()
{
	return "";
}

void CCvarComboBox::ApplyChanges()
{
	/*
	if (m_iCurrentSelection == -1)
		return;
	if (m_Items.Size() < 1)
		return;

	Assert( m_iCurrentSelection < m_Items.Size() );
	COMMANDITEM *item = &m_Items[ m_iCurrentSelection ];
	engine->ClientCmd_Unrestricted( item->command );
	m_iStartSelection = m_iCurrentSelection;
	*/
}

bool CCvarComboBox::HasBeenModified()
{
	return m_iStartSelection != m_iCurrentSelection;
}

void CCvarComboBox::Reset()
{
	/*
	if (m_iStartSelection != -1)
	{
		ActivateItem(m_iStartSelection);
	}
	*/
}
