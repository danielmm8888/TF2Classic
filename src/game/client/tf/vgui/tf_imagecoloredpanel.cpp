//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//


#include "cbase.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui/ISurface.h>
#include <vgui/ISystem.h>
#include <vgui/IScheme.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui/ISurface.h>
#include <vgui/IImage.h>
#include <vgui_controls/Label.h>

#include "tf_imagecoloredpanel.h"
#include "c_tf_player.h"
#include "tf_gamerules.h"
#include "c_playerresource.h"
#include "c_tf_playerresource.h"

using namespace vgui;

DECLARE_BUILD_FACTORY( CTFImageColoredPanel );

extern ConVar tf2c_coloredhud;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFImageColoredPanel::CTFImageColoredPanel(Panel *parent, const char *name) : ScalableImagePanel(parent, name)
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFImageColoredPanel::PaintBackground()
{
	C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>(g_PR);

	//disable for deathmatch for now
	if (!tf_PR || !tf2c_coloredhud.GetBool() || !TFGameRules() || !TFGameRules()->IsDeathmatch())
	{
		SetDrawColor(Color(0, 0, 0, 0));
	}
	else
	{
		int iIndex = GetLocalPlayerIndex();
		SetDrawColor(tf_PR->GetPlayerColor(iIndex));
	}
	BaseClass::PaintBackground();
}
