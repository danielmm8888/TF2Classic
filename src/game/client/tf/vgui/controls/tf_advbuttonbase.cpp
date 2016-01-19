#include "cbase.h"
#include "tf_advbuttonbase.h"
#include "vgui_controls/Frame.h"
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui/IInput.h>
#include "vgui_controls/Button.h"
#include "vgui_controls/ImagePanel.h"
#include "tf_controls.h"
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>
#include "panels/tf_tooltippanel.h"
#include "basemodelpanel.h"


using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFAdvButtonBase, CTFAdvButtonBase);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvButtonBase::CTFAdvButtonBase(vgui::Panel *parent, const char *panelName, const char *text) : vgui::EditablePanel(parent, panelName)
{
	pButtonImage = new ImagePanel(this, "SubImage");
	pButtonImage->SetParent(this);

	Init();
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}	

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvButtonBase::~CTFAdvButtonBase()
{
	delete pButtonImage;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::Init()
{ 
	SetTall(50);
	SetWide(100);
	Q_strncpy(pDefaultBG, DEFAULT_BG, sizeof(pDefaultBG));
	Q_strncpy(pArmedBG, ARMED_BG, sizeof(pArmedBG));
	Q_strncpy(pDepressedBG, DEPRESSED_BG, sizeof(pDepressedBG));
	Q_strncpy(pDefaultBorder, DEFAULT_BORDER, sizeof(pDefaultBorder));
	Q_strncpy(pArmedBorder, ARMED_BORDER, sizeof(pArmedBorder));
	Q_strncpy(pDepressedBorder, DEPRESSED_BORDER, sizeof(pDepressedBorder));
	Q_strncpy(pDefaultColor, DEFAULT_COLOR, sizeof(pDefaultColor));
	Q_strncpy(pArmedColor, ARMED_COLOR, sizeof(pArmedColor));
	Q_strncpy(pDepressedColor, DEPRESSED_COLOR, sizeof(pDepressedColor));
	Q_strncpy(pSelectedColor, ARMED_COLOR, sizeof(pSelectedColor));
	Q_strncpy(pDefaultButtonImage, DEFAULT_IMAGE, sizeof(pDefaultButtonImage));
	Q_strncpy(pImageColorDefault, DEFAULT_COLOR, sizeof(pImageColorDefault));
	Q_strncpy(pImageColorArmed, ARMED_COLOR, sizeof(pImageColorArmed));
	Q_strncpy(pImageColorDepressed, DEPRESSED_COLOR, sizeof(pImageColorDepressed));
	
	Q_strncpy(m_szCommand, EMPTY_STRING, sizeof(m_szCommand));
	Q_strncpy(pToolTip, EMPTY_STRING, sizeof(pToolTip));
	m_bBorderVisible = true;
	m_bAutoChange = false;
	m_bSelected = false;
	m_fImageWidth = 0.0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);

	Q_strncpy(m_szCommand, inResourceData->GetString("command", EMPTY_STRING), sizeof(m_szCommand));
	m_bBorderVisible = inResourceData->GetBool("bordervisible", true);

	for (KeyValues *pData = inResourceData->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey())
	{
		if (!Q_stricmp(pData->GetName(), "SubButton"))
		{
			Q_strncpy(pToolTip, pData->GetString("tooltip", ""), sizeof(pToolTip));
			Q_strncpy(pDefaultBorder, pData->GetString("button_default", DEFAULT_BORDER), sizeof(pDefaultBorder));
			Q_strncpy(pArmedBorder, pData->GetString("button_armed", ARMED_BORDER), sizeof(pArmedBorder));
			Q_strncpy(pDepressedBorder, pData->GetString("button_depressed", DEPRESSED_BORDER), sizeof(pDepressedBorder));

			Q_strncpy(pDefaultBG, pData->GetString("border_default", DEFAULT_BG), sizeof(pDefaultBG));
			Q_strncpy(pArmedBG, pData->GetString("border_armed", ARMED_BG), sizeof(pArmedBG));
			Q_strncpy(pDepressedBG, pData->GetString("border_depressed", DEPRESSED_BG), sizeof(pDepressedBG));

			Q_strncpy(pDefaultColor, pData->GetString("defaultFgColor_override", DEFAULT_COLOR), sizeof(pDefaultColor));
			Q_strncpy(pArmedColor, pData->GetString("armedFgColor_override", ARMED_COLOR), sizeof(pArmedColor));
			Q_strncpy(pDepressedColor, pData->GetString("depressedFgColor_override", DEPRESSED_COLOR), sizeof(pDepressedColor));
			Q_strncpy(pSelectedColor, pData->GetString("selectedFgColor_override", ARMED_COLOR), sizeof(pSelectedColor));

			m_bSelected = inResourceData->GetBool("selected", false);
		}
		if (!Q_stricmp(pData->GetName(), "SubImage"))
		{
			pButtonImage->ApplySettings(pData);
			m_fImageWidth = pData->GetFloat("imagewidth", 0.0);
			Q_strncpy(pDefaultButtonImage, pData->GetString("image", DEFAULT_IMAGE), sizeof(pDefaultButtonImage));

			Q_strncpy(pImageColorDefault, pData->GetString("imagecolor", pDefaultColor), sizeof(pImageColorDefault));
			Q_strncpy(pImageColorArmed, pData->GetString("imagecolorarmed", pArmedColor), sizeof(pImageColorArmed));
			Q_strncpy(pImageColorDepressed, pData->GetString("imagecolordepressed", pDepressedColor), sizeof(pImageColorDepressed));
		}
	}

	m_bAutoChange = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::PerformLayout()
{
	BaseClass::PerformLayout();

	if (m_bBorderVisible)
	{
		SetBorder(GETSCHEME()->GetBorder(pSelectedBG));
	}
	else
	{
		SetBorder(GETSCHEME()->GetBorder(EMPTY_STRING));
	}
}

void CTFAdvButtonBase::SetCommandString(const char *sCommand)
{
	Q_strncpy(m_szCommand, sCommand, sizeof(m_szCommand));
}

const char* CTFAdvButtonBase::GetCommandString()
{
	return m_szCommand;
}

void CTFAdvButtonBase::SetBorderByString(const char *sBorderDefault, const char *sBorderArmed, const char *sBorderDepressed)
{
	Q_strncpy(pDefaultBG, sBorderDefault, sizeof(pDefaultBG));
	if (IsEnabled())
	{
		Q_strncpy(pArmedBG, sBorderArmed, sizeof(pArmedBG));
		Q_strncpy(pDepressedBG, sBorderDepressed, sizeof(pDepressedBG));
	}
	else
	{
		Q_strncpy(pArmedBG, sBorderDefault, sizeof(pArmedBG));
		Q_strncpy(pDepressedBG, sBorderDefault, sizeof(pDepressedBG));
	}
	PerformLayout();
}

void CTFAdvButtonBase::SetBorderVisible(bool bVisible)
{
	m_bBorderVisible = bVisible;
	PerformLayout();
};

void CTFAdvButtonBase::SetImageInset(int iInsetX, int iInsetY)
{
	pButtonImage->SetPos(iInsetX, iInsetY);
}

void CTFAdvButtonBase::SetImageSize(int iWide, int iTall)
{
	pButtonImage->SetSize(iWide, iTall);
}

void CTFAdvButtonBase::SetToolTip(const char *sText)
{
	Q_strncpy(pToolTip, sText, sizeof(pToolTip));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::OnThink()
{
	BaseClass::OnThink();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::SetDefaultAnimation()
{
	if (m_bBorderVisible)
		SetBorder(GETSCHEME()->GetBorder(pSelectedBG));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvButtonBase::SendAnimation(MouseState flag)
{

	switch (flag)
	{
	case MOUSE_DEFAULT:
		if (pToolTip[0] != '\0')
			MAINMENU_ROOT->HideToolTip();
		if (m_bBorderVisible)
			SetBorder(GETSCHEME()->GetBorder(pSelectedBG));
		break;
	case MOUSE_ENTERED:
		if (pToolTip[0] != '\0')
			MAINMENU_ROOT->ShowToolTip(pToolTip);
		if (m_bBorderVisible)
			SetBorder(GETSCHEME()->GetBorder(pArmedBG));
		break;
	case MOUSE_EXITED:
		if (pToolTip[0] != '\0')
			MAINMENU_ROOT->HideToolTip();
		if (m_bBorderVisible)
			SetBorder(GETSCHEME()->GetBorder(pSelectedBG));
		break;
	case MOUSE_PRESSED:
		if (m_bBorderVisible)
			SetBorder(GETSCHEME()->GetBorder(pDepressedBG));
		break;
	default:
		if (m_bBorderVisible)
			SetBorder(GETSCHEME()->GetBorder(pDefaultBG));
		break;
	}
}

///
//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFButtonBase::CTFButtonBase(vgui::Panel *parent, const char *panelName, const char *text) : Button(parent, panelName, text)
{
	iState = MOUSE_DEFAULT;
	m_bBorderVisible = false;
	vgui::ivgui()->AddTickSignal(GetVPanel());
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

void CTFButtonBase::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);
	m_bBorderVisible = inResourceData->GetBool("bordervisible", false);
	_activationType = (ActivationType_t)inResourceData->GetInt("button_activation_type", ACTIVATE_ONPRESSEDANDRELEASED);
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

void CTFButtonBase::SetFontByString(const char *sFont)
{
	if (!GETSCHEME())
		return;
	SetFont(GETSCHEME()->GetFont(sFont, true));
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
vgui::Label::Alignment CTFAdvButtonBase::GetAlignment(char* m_szAlignment)
{
	// text alignment
	const char *alignmentString = m_szAlignment;
	int align = -1;

	if (!stricmp(alignmentString, "north-west"))
	{
		align = vgui::Label::a_northwest;
	}
	else if (!stricmp(alignmentString, "north"))
	{
		align = vgui::Label::a_north;
	}
	else if (!stricmp(alignmentString, "north-east"))
	{
		align = vgui::Label::a_northeast;
	}
	else if (!stricmp(alignmentString, "west"))
	{
		align = vgui::Label::a_west;
	}
	else if (!stricmp(alignmentString, "center"))
	{
		align = vgui::Label::a_center;
	}
	else if (!stricmp(alignmentString, "east"))
	{
		align = vgui::Label::a_east;
	}
	else if (!stricmp(alignmentString, "south-west"))
	{
		align = vgui::Label::a_southwest;
	}
	else if (!stricmp(alignmentString, "south"))
	{
		align = vgui::Label::a_south;
	}
	else if (!stricmp(alignmentString, "south-east"))
	{
		align = vgui::Label::a_southeast;
	}

	if (align != -1)
	{
		return (vgui::Label::Alignment)align;
	}

	return vgui::Label::a_center;
}

