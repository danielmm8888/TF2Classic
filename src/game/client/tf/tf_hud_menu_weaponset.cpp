//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "c_tf_player.h"
#include "iclientmode.h"
#include "ienginevgui.h"
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include "c_baseobject.h"
#include "c_tf_weapon_builder.h"
#include "hud_macros.h"
#include "tf_inventory.h"
#include "tf_weaponbase.h"
#include "tf_gamerules.h"
#include "tf_mainmenu.h"

#include <string.h>

#include "tf_hud_menu_weaponset.h"

#include "history_resource.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include <vgui_controls/AnimationController.h>
#include <vgui_controls/Panel.h>
#include <vgui_controls/EditablePanel.h>
#include "baseobject_shared.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//======================================

DECLARE_HUDELEMENT(CHudMenuWeaponSet);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudMenuWeaponSet::CHudMenuWeaponSet(const char *pElementName) : CHudElement(pElementName), EditablePanel(NULL, "HudMenuSelectBase")
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	SetHiddenBits(HIDEHUD_MISCSTATUS);

	m_pWeaponIconBase = new EditablePanel(this, "WeaponIcon");
	m_pWeaponIconBase->SetVisible(false);

	for (int i = 0; i < INVENTORY_VECTOR_NUM; i++){
		m_pWeaponIcons.AddToTail(new EditablePanel(this, "WeaponIcon"));
		m_pWeaponIcons[i]->SetVisible(false);
	}
	
	SetPostChildPaintEnabled(true);
	m_iSelectedSlot = -1;

	vgui::ivgui()->AddTickSignal(GetVPanel());
	ListenForGameEvent("localplayer_changeclass");
	ListenForGameEvent("localplayer_changeteam");
	InvalidateLayout(false, true);
	RegisterForRenderGroup("mid");
}
ConVar tf2c_weaponset_show("tf2c_weaponset_show", "0", FCVAR_ARCHIVE, "Shows weapon selection menu. 1 = ON, 0 = OFF.");

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	
	// load control settings...
	LoadControlSettings("resource/UI/weaponselect_menu/HudMenuSelectBase.res");
	m_pWeaponIconBase->LoadControlSettings("resource/UI/weaponselect_menu/HudMenuWeaponIcon.res");
	m_pWeaponIconBase->SetPos(500, 500);
	m_pMainBackground = dynamic_cast<EditablePanel *>(FindChildByName("MainBG"));
	m_pStatusLabel = dynamic_cast<CExLabel *>(FindChildByName("StatusLabel"));
	m_pSelectedSlot = dynamic_cast<CTFImagePanel *>(FindChildByName("SelectedSlot"));
	m_pSelectedSlot->GetPos(m_iSelect_X, m_iSelect_Y);

	for (int i = 0; i < INVENTORY_VECTOR_NUM; i++)
	{
		m_pWeaponIcons[i]->LoadControlSettings("resource/UI/weaponselect_menu/HudMenuWeaponIcon.res");
		m_pWeaponIcons[i]->SetVisible(1);
		m_pWeaponIcons[i]->SetWide(m_pWeaponIconBase->GetWide());
		m_pWeaponIcons[i]->SetTall(m_pWeaponIconBase->GetTall());
		m_pWeaponIcons[i]->SetPos(0, 0);
		m_pWeaponIcons[i]->SetZPos(m_pWeaponIconBase->GetZPos());
	}

	SetPaintBackgroundEnabled(true);

	m_fWide = XRES(100);
	m_fTall = YRES(82);

	int screenWide, screenTall;
	int x, y;
	GetPos(x, y);
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
	DefaultLayout();
	UpdateLayout();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudMenuWeaponSet::ShouldDraw(void)
{
	CTFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (!pPlayer)
		return false;

	CTFWeaponBase *pWpn = pPlayer->GetActiveTFWeapon();
	if (!pWpn)
		return false;

	// Don't show the menu for first person spectator
	if (pPlayer != pWpn->GetOwner())
		return false;

	if (pPlayer->m_Shared.InCond(TF_COND_TAUNTING))
		return false;

	if ( TFGameRules()->IsDeathmatch() )
		return false;

	return (tf2c_weaponset_show.GetBool());
}

void CHudMenuWeaponSet::DefaultLayout()
{
	m_iSelectedSlot = -1;
	m_pStatusLabel->SetText("Select Slot");
	m_pSelectedSlot->SetVisible(false);
}

void CHudMenuWeaponSet::UpdateLayout()
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (!pPlayer)
		return;
	C_TFPlayerClass* pClass = pPlayer->GetPlayerClass();
	int iClass = pClass->GetClassIndex();
	C_BaseCombatWeapon *pSelectedWeapon = NULL;
	pSelectedWeapon = pPlayer->GetActiveWeapon();
	if (!pSelectedWeapon)
		return;

	int iColCount = 0;
	for (int iSlot = 0; iSlot < INVENTORY_ROWNUM; iSlot++)
	{
		int iCols = 0;
		for (int iPreset = 0; iPreset < INVENTORY_COLNUM; iPreset++)
		{
			EditablePanel* m_pWeaponIcon = m_pWeaponIcons[INVENTORY_COLNUM * iSlot + iPreset];

			int iWeapon = GetTFInventory()->GetWeapon(pClass->GetClassIndex(), iSlot, iPreset);
			if (iWeapon > 0)
			{
				iCols++;
				if (iCols > iColCount) 
					iColCount = iCols;

				m_pWeaponIcon->SetVisible(1);
				m_pWeaponIcon->SetPos((iPreset + 0.9) * m_fWide, (iSlot + 0.55) * m_fTall);
				m_pWeaponBucket = dynamic_cast<CTFImagePanel *>(m_pWeaponIcon->FindChildByName("WeaponBucket"));
				m_pWeaponBucket->SetVisible(true);
				char* cIcon = GetTFInventory()->GetWeaponBucket(iWeapon, pPlayer->GetTeamNumber());
				char szIcon[64];
				Q_snprintf(szIcon, sizeof(szIcon), "../%s", cIcon);
				if (szIcon)
					m_pWeaponBucket->SetImage(szIcon);

				m_pWeaponLabel = dynamic_cast<CExLabel *>(m_pWeaponIcon->FindChildByName("WeaponLabel"));
				const char *pszWeaponName = WeaponIdToAlias(iWeapon);
				char szWeaponName[64];
				Q_snprintf(szWeaponName, sizeof(szWeaponName), "#%s", pszWeaponName);
				wchar_t *pText = g_pVGuiLocalize->Find(szWeaponName);
				m_pWeaponLabel->SetText(pText);

				m_pActiveWeaponBG = dynamic_cast<CTFImagePanel *>(m_pWeaponIcon->FindChildByName("ActiveWeapon"));
				int iWeaponPreset = GetTFInventory()->GetWeaponPreset(filesystem, iClass, iSlot);
				m_pActiveWeaponBG->SetVisible((iPreset == iWeaponPreset));

				m_pWeaponLabel = dynamic_cast<CExLabel *>(m_pWeaponIcon->FindChildByName("WeaponNumber"));
				char szWeaponNumber[64];
				Q_snprintf(szWeaponNumber, sizeof(szWeaponNumber), "%d %d", iSlot + 1, iPreset + 1);
				m_pWeaponLabel->SetText(szWeaponNumber);
			}
			else
			{
				m_pWeaponIcon->SetVisible(0);
			}
		}
	}

	m_pSelectedSlot->SetWide((iColCount + 1) * m_fWide);
	m_pMainBackground->SetWide((iColCount + 1) * m_fWide);
}

//-----------------------------------------------------------------------------
// Purpose: Keyboard input hook. Return 0 if handled
//-----------------------------------------------------------------------------
int	CHudMenuWeaponSet::HudElementKeyInput(int down, ButtonCode_t keynum, const char *pszCurrentBinding)
{
	if (!ShouldDraw())
	{
		DefaultLayout();
		engine->ExecuteClientCmd("tf2c_weaponset_show 0");
		return 1;
	}
	if (!down)
	{
		return 1;
	}
	switch (keynum)
	{
	case KEY_1:
	case KEY_2:
	case KEY_3:
	case KEY_4:
	case KEY_5:
	case KEY_6:
	case KEY_7:
	case KEY_8:
	case KEY_9:
	{
		if (m_iSelectedSlot == -1)
		{
			int iSlot = keynum - KEY_1;
			SelectSlot(iSlot);
		}
		else
		{
			int iWeapon = keynum - KEY_1;
			SelectWeapon(m_iSelectedSlot, iWeapon);
		}
	}
	return 0;
	case KEY_0:
		// cancel, close the menu
		engine->ExecuteClientCmd("tf2c_weaponset_show 0");
		DefaultLayout();
		return 0;
	default:
		return 1;	// key not handled
	}
	return 1;	// key not handled
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::SelectSlot(int iSlot)
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	int iClass = pPlayer->m_Shared.GetDesiredPlayerClassIndex();
	if (pPlayer && GetTFInventory()->CheckValidSlot(iClass, iSlot, true))
	{
		m_iSelectedSlot = iSlot;
		m_pStatusLabel->SetText("Select Weapon");
		m_pSelectedSlot->SetVisible(true);
		m_pSelectedSlot->SetPos(0, 0 + (m_iSelectedSlot + 0.7) * m_fTall);
	}
	else
	{
		DefaultLayout();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::SelectWeapon(int iSlot, int iWeapon)
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	int iClass = pPlayer->m_Shared.GetDesiredPlayerClassIndex();
	if (pPlayer && GetTFInventory()->CheckValidWeapon(iClass, iSlot, iWeapon, true))
	{
		char szCmd[64];
		Q_snprintf(szCmd, sizeof(szCmd), "weaponpreset %d %d", iSlot, iWeapon); //; tf2c_weaponset_show 0
		engine->ExecuteClientCmd(szCmd);
		pPlayer->EditInventory(iSlot, iWeapon);
		UpdateLayout();
	}
	DefaultLayout();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::FireGameEvent(IGameEvent *event)
{
	if (!ShouldDraw())
	{
		engine->ExecuteClientCmd("tf2c_weaponset_show 0");
		DefaultLayout();
		SetVisible(0);
	}

	const char * type = event->GetName();
	if (Q_strcmp(type, "localplayer_changeclass") == 0)
	{
		DefaultLayout();
		UpdateLayout();
	}
	else if (Q_strcmp(type, "localplayer_changeteam") == 0)
	{
		engine->ExecuteClientCmd("tf2c_weaponset_show 0");
		DefaultLayout();
		UpdateLayout();
	}
	else
	{
		CHudElement::FireGameEvent(event);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::SetVisible(bool state)
{
	if (state == true)
	{
		HideLowerPriorityHudElementsInGroup("mid");
	}
	else
	{
		UnhideLowerPriorityHudElementsInGroup("mid");
	}

	BaseClass::SetVisible(state);
}