//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include <assert.h>
#include <vgui_controls/ScrollBar.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include "vgui_controls/Frame.h"

#include <KeyValues.h>
#include <vgui/MouseCode.h>
#include <vgui/KeyCode.h>
#include <vgui/IInput.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include "tf_advtabs.h"
#include "tf_advbutton.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;


//DECLARE_BUILD_FACTORY( CCvarSlider );
vgui::Panel *CAdvTabs_Factory()
{
	return new CAdvTabs(NULL, "CAdvTabs");
}
DECLARE_BUILD_FACTORY_CUSTOM(CAdvTabs, CAdvTabs_Factory);

CAdvTabs::CAdvTabs(vgui::Panel *parent, char const *panelName) : vgui::EditablePanel(parent, panelName)
{
	m_pButtons.RemoveAll();
	m_pCurrentButton = nullptr;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CAdvTabs::~CAdvTabs()
{
	DeleteAllItems();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *inResourceData - 
//-----------------------------------------------------------------------------
void CAdvTabs::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);
	iOffset = inResourceData->GetInt("offset", 0);

	m_pButtons.RemoveAll();
	int iCount = GetChildCount();
	for (int i = 0; i < iCount; i++)
	{
		CTFAdvButton *pButton = dynamic_cast<CTFAdvButton*>(GetChild(i));
		if (pButton)
		{
			m_pButtons.AddToTail(pButton);
		}
	}

	int iWide = GetWide() / iCount - iOffset;
	int iTall = GetTall();

	iCount = m_pButtons.Count();
	for (int i = 0; i < iCount; i++)
	{
		CTFAdvButton *pButton = m_pButtons[i];
		if (pButton)
		{
			pButton->SetPos(i * (iWide + iOffset), 0);
			pButton->SetZPos(i - 1);
			pButton->SetSize(iWide, iTall);
			pButton->SetShouldScaleImage(true);
		}
	}

	if (!m_pCurrentButton)
	{
		m_pCurrentButton = m_pButtons[0];
		m_pCurrentButton->SetSelected(true);
	}

	InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

void CAdvTabs::OnButtonPressed(Panel *pPanel)
{
	if (m_pCurrentButton)
		m_pCurrentButton->SetSelected(false);
	m_pCurrentButton = dynamic_cast<CTFAdvButton*>(pPanel);
	m_pCurrentButton->SetSelected(true);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAdvTabs::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

//-----------------------------------------------------------------------------
// Purpose: relayouts out the panel after any internal changes
//-----------------------------------------------------------------------------
void CAdvTabs::PerformLayout()
{
	BaseClass::PerformLayout();
}

//-----------------------------------------------------------------------------
// Purpose: clears and deletes all the memory used by the data items
//-----------------------------------------------------------------------------
void CAdvTabs::DeleteAllItems()
{
	m_pButtons.RemoveAll();
	InvalidateLayout();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAdvTabs::OnCommand(const char* command)
{
	GetParent()->OnCommand(command);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAdvTabs::PaintBackground()
{
	Panel::PaintBackground();
}