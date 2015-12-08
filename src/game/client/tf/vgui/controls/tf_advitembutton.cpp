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
	pItemDefinition = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFAdvItemButton::SendAnimation(MouseState flag)
{
	BaseClass::SendAnimation(flag);
	switch (flag)
	{
	case MOUSE_DEFAULT:
		if (pItemDefinition)
			MAINMENU_ROOT->ShowItemToolTip(pItemDefinition);
		break;
	case MOUSE_ENTERED:
		if (pItemDefinition)
			MAINMENU_ROOT->ShowItemToolTip(pItemDefinition);
		break;
	case MOUSE_EXITED:
		if (pItemDefinition)
			MAINMENU_ROOT->HideItemToolTip();
		break;
	case MOUSE_PRESSED:
		break;
	default:
		break;
	}
}

void CTFAdvItemButton::SetItemDefinition(EconItemDefinition *pItemData)
{
	pItemDefinition = pItemData;

	char szIcon[128];
	Q_snprintf(szIcon, sizeof(szIcon), "../%s_large", pItemData->image_inventory);
	SetImage(szIcon);

	char szWeaponName[32];
	Q_snprintf(szWeaponName, sizeof(szWeaponName), "%s", pItemData->item_name);
	SetText(szWeaponName);
}