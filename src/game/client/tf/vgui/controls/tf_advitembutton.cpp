#include "cbase.h"
#include "tf_advitembutton.h"
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
#include "panels/tf_dialogpanelbase.h"
#include "inputsystem/iinputsystem.h"
#include "tf_inventory.h"

using namespace vgui;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

DECLARE_BUILD_FACTORY_DEFAULT_TEXT(CTFAdvItemButton, CTFAdvItemButtonBase);

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFAdvItemButton::CTFAdvItemButton(vgui::Panel *parent, const char *panelName, const char *text) : CTFAdvButton(parent, panelName, text)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFAdvItemButton::~CTFAdvItemButton()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvItemButton::Init()
{
	BaseClass::Init();
	m_pItemDefinition = NULL;
	m_iLoadoutSlot = TF_LOADOUT_SLOT_PRIMARY;
	pButton->SetContentAlignment(CTFAdvButtonBase::GetAlignment("south"));
	pButton->SetTextInset(0, -10);
}

void CTFAdvItemButton::PerformLayout()
{
	int inset = YRES(45);
	int wide = GetWide() - inset;
	SetImageSize(wide, wide);
	SetImageInset(inset / 2, -1 * wide / 5);
	SetShouldScaleImage(true);

	BaseClass::PerformLayout();
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvItemButton::SendAnimation(MouseState flag)
{
	BaseClass::SendAnimation(flag);
	switch (flag)
	{
	case MOUSE_DEFAULT:
		if ( m_pItemDefinition )
			MAINMENU_ROOT->ShowItemToolTip( m_pItemDefinition );
		break;
	case MOUSE_ENTERED:
		if ( m_pItemDefinition )
			MAINMENU_ROOT->ShowItemToolTip( m_pItemDefinition );
		break;
	case MOUSE_EXITED:
		if ( m_pItemDefinition )
			MAINMENU_ROOT->HideItemToolTip();
		break;
	case MOUSE_PRESSED:
		break;
	default:
		break;
	}
}

void CTFAdvItemButton::SetItemDefinition(CEconItemDefinition *pItemData)
{
	m_pItemDefinition = pItemData;

	char szIcon[128];
	Q_snprintf( szIcon, sizeof( szIcon ), "../%s_large", pItemData->image_inventory );
	SetImage( szIcon );

	SetText( pItemData->item_name );

	pButton->SetDepressedSound( pItemData->mouse_pressed_sound );
	pButton->SetReleasedSound( NULL );
}

void CTFAdvItemButton::SetLoadoutSlot( int iSlot, int iPreset )
{
	m_iLoadoutSlot = iSlot;

	char szCommand[64];
	Q_snprintf( szCommand, sizeof( szCommand ), "loadout %d %d", iSlot, iPreset );
	SetCommandString( szCommand );
}
