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

	m_pItemDefFrom = NULL;
	m_pItemDefTo = NULL;

	m_pImageFrom = NULL;
	m_pImageTo = NULL;

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
	if (iWeaponTo != -1)
	{
		m_pItemDefTo = GetItemSchema()->GetItemDefinition( iWeaponTo );

		if ( !m_pItemDefTo )
			return false;

		C_TFWeaponBase *pWeaponFrom = (C_TFWeaponBase *)pLocalTFPlayer->Weapon_GetSlot( m_pItemDefTo->item_slot );
		if (!pWeaponFrom)
			return false;

		m_pItemDefFrom = pWeaponFrom->GetItem()->GetStaticData();
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

	m_pImageFrom = dynamic_cast<CTFImagePanel *>( FindChildByName( "WeaponBucketFrom" ) );
	m_pImageTo = dynamic_cast<CTFImagePanel *>( FindChildByName( "WeaponBucketTo" ) );
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
	if ( !m_pItemDefFrom || !m_pItemDefTo )
		return;

	SetDialogVariable( "weapontoalias", g_pVGuiLocalize->Find( m_pItemDefTo->item_name ) );
	SetDialogVariable( "weaponfromalias", g_pVGuiLocalize->Find( m_pItemDefFrom->item_name ) );


	const char *key = engine->Key_LookupBinding("+use");
	if (!key)
	{
		key = "< not bound >";
	}
	char hint[64];
	Q_snprintf(hint, sizeof(hint), "Press '%s' to switch", key);
	SetDialogVariable("hint", hint);

	const char *pszTextureFrom = m_pItemDefFrom->image_inventory;

	if ( m_pImageFrom )
	{
		char szImage[128];
		Q_snprintf( szImage, sizeof( szImage ), "../%s", pszTextureFrom );
		m_pImageFrom->SetImage( szImage );
	}

	const char *pszTextureTo = m_pItemDefTo->image_inventory;

	if ( m_pImageTo )
	{
		char szImage[128];
		Q_snprintf( szImage, sizeof( szImage ), "../%s", pszTextureTo );
		m_pImageTo->SetImage( szImage );
	}
}