//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <vgui/ILocalize.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/Label.h>

#include "tf_hud_weaponswitch.h"
#include "c_tf_player.h"
#include "tf_hud_freezepanel.h"
#include "tf_imagepanel.h"

#include "tf_gamerules.h"
#include "tf_weapon_parse.h"
#include "c_tf_playerresource.h"

using namespace vgui;

DECLARE_HUDELEMENT(CTFHudWeaponSwitch);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudWeaponSwitch::CTFHudWeaponSwitch(const char *pElementName) : CHudElement(pElementName), BaseClass(NULL, "HudWeaponSwitch")
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	pWeaponFromInfo = NULL;
	pWeaponToInfo = NULL;

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudWeaponSwitch::ShouldDraw(void)
{
	if( IsTakingAFreezecamScreenshot() )
		return false;

	if (!TFGameRules() || !TFGameRules()->IsDeathmatch())
		return false;

	C_TFPlayer *pLocalTFPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (!pLocalTFPlayer)
		return false;

	int iWeaponTo = pLocalTFPlayer->m_Shared.GetDesiredWeaponIndex();
	if (iWeaponTo != TF_WEAPON_NONE)
	{
		pWeaponToInfo = GetTFWeaponInfo(iWeaponTo);

		C_TFWeaponBase *pWeaponFrom = pLocalTFPlayer->Weapon_GetWeaponByBucket(pWeaponToInfo->iSlot);		
		if (!pWeaponFrom)
			return false;

		pWeaponFromInfo = GetTFWeaponInfo(pWeaponFrom->GetWeaponID());
		UpdateStatus();

		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponSwitch::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/HudWeaponSwitch.res" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponSwitch::OnTick()
{
	///
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudWeaponSwitch::UpdateStatus( void )
{
	if (!pWeaponFromInfo || !pWeaponToInfo)
		return;

	SetDialogVariable("weapontoalias", g_pVGuiLocalize->Find(pWeaponToInfo->szPrintName));
	SetDialogVariable("weaponfromalias", g_pVGuiLocalize->Find(pWeaponFromInfo->szPrintName));


	const char *key = engine->Key_LookupBinding("+use");
	if (!key)
	{
		key = "< not bound >";
	}
	char hint[64];
	Q_snprintf(hint, sizeof(hint), "Press '%s' to switch", key);
	SetDialogVariable("hint", hint);

	const CHudTexture *pTextureFrom = pWeaponFromInfo->iconInactive;
	if (pTextureFrom)
	{
		char szImage[64];
		Q_snprintf(szImage, sizeof(szImage), "../%s", pTextureFrom->szTextureFile);
		dynamic_cast<CTFImagePanel *>(FindChildByName("WeaponBucketFrom"))->SetImage(szImage);
	}

	const CHudTexture *pTextureTo = pWeaponToInfo->iconInactive;
	if (pTextureTo)
	{
		char szImage[64];
		Q_snprintf(szImage, sizeof(szImage), "../%s", pTextureTo->szTextureFile);
		dynamic_cast<CTFImagePanel *>(FindChildByName("WeaponBucketTo"))->SetImage(szImage);
	}
}