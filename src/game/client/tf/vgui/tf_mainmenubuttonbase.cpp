#include "cbase.h"
#include "tf_mainmenubuttonbase.h"
#include "vgui_controls/Frame.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include "vgui_controls/Button.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_controls.h"
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>
#include "basemodelpanel.h"


using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFMainMenuButtonBase, CTFMainMenuButtonBase);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuButtonBase::CTFMainMenuButtonBase(vgui::Panel *parent, const char *panelName, const char *text) : CExButton(parent, panelName, text)
{
	pImage = new CTFImagePanel(this, "TestImage");

	vgui::ivgui()->AddTickSignal(GetVPanel());
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButtonBase::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	Q_strncpy(pDefaultImage, inResourceData->GetString("DefaultImage", ""), sizeof(pDefaultImage));
	Q_strncpy(pArmedImage, inResourceData->GetString("ArmedImage", ""), sizeof(pArmedImage));
	Q_strncpy(pDepressedImage, inResourceData->GetString("DepressedImage", ""), sizeof(pDepressedImage));

	Q_strncpy(pDefaultBorder, inResourceData->GetString("DefaultBorder", ""), sizeof(pDefaultBorder));
	Q_strncpy(pArmedBorder, inResourceData->GetString("ArmedBorder", ""), sizeof(pArmedBorder));
	Q_strncpy(pDepressedBorder, inResourceData->GetString("DepressedBorder", ""), sizeof(pDepressedBorder));

	Q_strncpy(pDefaultText, inResourceData->GetString("DefaultText", ""), sizeof(pDefaultText));
	Q_strncpy(pArmedText, inResourceData->GetString("ArmedText", ""), sizeof(pArmedText));
	Q_strncpy(pDepressedText, inResourceData->GetString("DepressedText", ""), sizeof(pDepressedText));

	Q_strncpy(m_szCommand, inResourceData->GetString("command", ""), sizeof(m_szCommand));
	Q_strncpy(m_szTextAlignment, inResourceData->GetString("textAlignment", "center"), sizeof(m_szTextAlignment));		

	m_bImageVisible = inResourceData->GetBool("imagevisible", false);	
	m_bBorderVisible = inResourceData->GetBool("bordervisible", false);

	//InvalidateLayout(false, true); // force ApplySchemeSettings to run
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButtonBase::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	
	pImage->SetImage(pDefaultImage);
	pImage->SetVisible(m_bImageVisible);
	pImage->SetEnabled(true);
	pImage->SetPos(0, 0);
	pImage->SetZPos(1);
	pImage->SetWide(GetWide());
	pImage->SetTall(GetTall());
	pImage->SetShouldScaleImage(true);

	SetDefaultColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetArmedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetDepressedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
	SetSelectedColor(Color(0, 0, 0, 0), Color(0, 0, 0, 0));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButtonBase::OnThink()
{
	BaseClass::OnThink();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButtonBase::SetDefaultAnimation()
{
	pImage->SetImage(pDefaultImage);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFMainMenuButtonBase::SendAnimation(MouseState flag)
{
	switch (flag)
	{
	//We can add additional stuff like animation here
	case MOUSE_DEFAULT:
		pImage->SetImage(pDefaultImage);
		break;
	case MOUSE_ENTERED:
		pImage->SetImage(pArmedImage);
		break;
	case MOUSE_EXITED:
		pImage->SetImage(pDefaultImage);
		break;
	case MOUSE_PRESSED:
		pImage->SetImage(pDepressedImage);
		break;
	default:
		pImage->SetImage(pDefaultImage);
		break;
	}
}

///
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFButtonBase::CTFButtonBase(vgui::Panel *parent, const char *panelName, const char *text) : CExButton(parent, panelName, text)
{
	iState = MOUSE_DEFAULT;
	vgui::ivgui()->AddTickSignal(GetVPanel());
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::OnCursorEntered()
{
	BaseClass::OnCursorEntered();
	if (iState != MOUSE_ENTERED)
	{
		SetMouseEnteredState(MOUSE_ENTERED);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::OnCursorExited()
{
	BaseClass::OnCursorExited();
	if (iState != MOUSE_EXITED)
	{
		SetMouseEnteredState(MOUSE_EXITED);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::OnMousePressed(vgui::MouseCode code)
{
	BaseClass::OnMousePressed(code);
	if (code == MOUSE_LEFT && iState != MOUSE_PRESSED)
	{
		SetMouseEnteredState(MOUSE_PRESSED);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::OnMouseReleased(vgui::MouseCode code)
{
	BaseClass::OnMouseReleased(code);
	if (code == MOUSE_LEFT && (iState == MOUSE_ENTERED || iState == MOUSE_PRESSED))
	{
		//Set this to do something
	}
	if (code == MOUSE_LEFT && iState == MOUSE_ENTERED)
	{
		SetMouseEnteredState(MOUSE_ENTERED);
	}
	else
	{
		SetMouseEnteredState(MOUSE_EXITED);
	}
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::SetMouseEnteredState(MouseState flag)
{
	iState = flag;
}


//-----------------------------------------------------------------------------
// Purpose: Set armed button border attributes.
//-----------------------------------------------------------------------------
void CTFButtonBase::SetArmedBorder(IBorder *border)
{
	_armedBorder = border;
	InvalidateLayout(false);
}

//-----------------------------------------------------------------------------
// Purpose: Set selected button border attributes.
//-----------------------------------------------------------------------------
void CTFButtonBase::SetSelectedBorder(IBorder *border)
{
	_selectedBorder = border;
	InvalidateLayout(false);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFButtonBase::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	_armedBorder = pScheme->GetBorder("ButtonArmedBorder");
	_selectedBorder = pScheme->GetBorder("ButtonSelectedBorder");
	InvalidateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: Get button border attributes.
//-----------------------------------------------------------------------------
IBorder *CTFButtonBase::GetBorder(bool depressed, bool armed, bool selected, bool keyfocus)
{
	if (depressed)
		return _depressedBorder;
	if (armed)
		return _armedBorder;
	if (selected)
		return _selectedBorder;

	return _defaultBorder;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
vgui::Label::Alignment CTFMainMenuButtonBase::GetAlignment(char* m_szAlignment)
{
	// text alignment
	const char *alignmentString = m_szAlignment;
	int align = -1;

	if (!stricmp(alignmentString, "north-west"))
	{
		align = a_northwest;
	}
	else if (!stricmp(alignmentString, "north"))
	{
		align = a_north;
	}
	else if (!stricmp(alignmentString, "north-east"))
	{
		align = a_northeast;
	}
	else if (!stricmp(alignmentString, "west"))
	{
		align = a_west;
	}
	else if (!stricmp(alignmentString, "center"))
	{
		align = a_center;
	}
	else if (!stricmp(alignmentString, "east"))
	{
		align = a_east;
	}
	else if (!stricmp(alignmentString, "south-west"))
	{
		align = a_southwest;
	}
	else if (!stricmp(alignmentString, "south"))
	{
		align = a_south;
	}
	else if (!stricmp(alignmentString, "south-east"))
	{
		align = a_southeast;
	}

	if (align != -1)
	{
		return (Alignment)align;
	}

	return a_center;
}
