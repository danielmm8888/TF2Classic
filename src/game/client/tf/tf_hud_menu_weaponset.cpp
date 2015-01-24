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
#include <vgui/IVGUI.h>
#include "c_baseobject.h"
#include "c_tf_weapon_builder.h"
#include "hud_macros.h"
#include "tf_inventory.h"
#include "tf_weaponbase.h"

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

	char buf[32];
	Q_snprintf(buf, sizeof(buf), "WeaponIcon");

	m_pWeaponIcon = new EditablePanel(this, buf);
	m_pWeaponIcon->SetVisible(false);

	for (int i = 0; i < VECTOR_NUM; i++){
		m_pWeaponIcons.AddToTail(new EditablePanel(this, buf));
		m_pWeaponIcons[i]->SetVisible(false);
	}

	SetPostChildPaintEnabled(true);

	m_iSelectedSlot = -1;
	m_iDemoModeSlot = -1;

	m_iBGImage_Inactive = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_iBGImage_Inactive, "hud/weapon_selection_unselected", true, false);

	m_iBGImage_Blue = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_iBGImage_Blue, "hud/weapon_selection_blue", true, false);

	m_iBGImage_Red = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_iBGImage_Red, "hud/weapon_selection_red", true, false);

	m_iBGImage_Green = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_iBGImage_Red, "hud/weapon_selection_green", true, false);

	m_iBGImage_Yellow = vgui::surface()->CreateNewTextureID();
	vgui::surface()->DrawSetTextureFile(m_iBGImage_Red, "hud/weapon_selection_yellow", true, false);

	vgui::ivgui()->AddTickSignal(GetVPanel());

	m_iSelectedItem = -1;

	m_pActiveSelection = NULL;

	InvalidateLayout(false, true);

	m_bInConsoleMode = false;

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
	m_pWeaponIcon->LoadControlSettings("resource/UI/weaponselect_menu/HudMenuWeaponIcon.res");
	m_pWeaponIcon->SetPos(500, 500);
	m_pWeaponBucket = dynamic_cast<CTFImagePanel *>(FindChildByName("SelectedSlot"));
	m_pWeaponBucket->GetPos(m_iSelect_X, m_iSelect_Y);

	for (int i = 0; i < VECTOR_NUM; i++)
	{
		m_pWeaponIcons[i]->LoadControlSettings("resource/UI/weaponselect_menu/HudMenuWeaponIcon.res");
		m_pWeaponIcons[i]->SetEnabled(1);
		m_pWeaponIcons[i]->SetVisible(1);
		m_pWeaponIcons[i]->SetWide(m_pWeaponIcon->GetWide());
		m_pWeaponIcons[i]->SetTall(m_pWeaponIcon->GetTall());
		m_pWeaponIcons[i]->SetPos(0, 0);
		m_pWeaponIcons[i]->SetZPos(m_pWeaponIcon->GetZPos());
	}

	m_pActiveSelection = NULL;

	SetPaintBackgroundEnabled(true);

	// set our size
	int screenWide, screenTall;
	int x, y;
	GetPos(x, y);
	GetHudSize(screenWide, screenTall);
	SetBounds(0, 0, screenWide, screenTall);
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

	return (tf2c_weaponset_show.GetBool());
}

void CHudMenuWeaponSet::OnThink()
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (!pPlayer)
		return;
	C_TFPlayerClass* pClass = pPlayer->GetPlayerClass();
	C_BaseCombatWeapon *pSelectedWeapon = NULL;
	pSelectedWeapon = pPlayer->GetActiveWeapon();
	if (!pSelectedWeapon)
		return;

	m_pWeaponLabel = dynamic_cast<CExLabel *>(FindChildByName("StatusLabel"));
	m_pWeaponBucket = dynamic_cast<CTFImagePanel *>(FindChildByName("SelectedSlot"));
	if (m_iSelectedSlot == -1)
	{
		m_pWeaponLabel->SetText("Select Slot");
		m_pWeaponBucket->SetVisible(false);
	}
	else
	{
		m_pWeaponLabel->SetText("Select Weapon");
		m_pWeaponBucket->SetVisible(true);
		m_pWeaponBucket->SetPos(m_iSelect_X, m_iSelect_Y + m_iSelectedSlot * 185);
	}


	for (int i = 0; i < ROWNUM; i++)
	{
		for (int j = 0; j < COLNUM; j++)
		{
				int iWeapon = Invenory->GetWeapon(pClass->GetClassIndex() - 1, i, j);
				if (iWeapon > 0)
				{
					m_pWeaponIcons[COLNUM * i + j]->SetEnabled(1);
					m_pWeaponIcons[COLNUM * i + j]->SetVisible(1);
					m_pWeaponIcons[COLNUM * i + j]->SetPos(j * 300 + 200, i * 185 + 100);
					m_pWeaponBucket = dynamic_cast<CTFImagePanel *>(m_pWeaponIcons[COLNUM * i + j]->FindChildByName("WeaponBucket"));
					m_pWeaponBucket->SetVisible(1);
					char* cIcon = Invenory->GetWeaponBucket(iWeapon, pPlayer->GetTeamNumber());
					char szIcon[64];
					Q_snprintf(szIcon, sizeof(szIcon), "../%s", cIcon);
					if (szIcon)
						m_pWeaponBucket->SetImage(szIcon);

					m_pWeaponLabel = dynamic_cast<CExLabel *>(m_pWeaponIcons[COLNUM * i + j]->FindChildByName("WeaponLabel"));
					const char *pszWeaponName = WeaponIdToAlias(iWeapon);
					char szWeaponName[64];
					Q_snprintf(szWeaponName, sizeof(szWeaponName), "#%s", pszWeaponName);
					wchar_t *pText = g_pVGuiLocalize->Find(szWeaponName);
					m_pWeaponLabel->SetText(pText);

					m_pActiveWeaponBG = dynamic_cast<CTFImagePanel *>(m_pWeaponIcons[COLNUM * i + j]->FindChildByName("ActiveWeapon"));
					if (pPlayer->Weapon_OwnsThisID(iWeapon))
					{
						m_pActiveWeaponBG->SetVisible(true);
					}
					else
					{
						m_pActiveWeaponBG->SetVisible(false);
					}

					m_pWeaponLabel = dynamic_cast<CExLabel *>(m_pWeaponIcons[COLNUM * i + j]->FindChildByName("WeaponNumber"));
					char szWeaponNumber[64];
					Q_snprintf(szWeaponNumber, sizeof(szWeaponNumber), "%d %d", i+1, j+1);
					m_pWeaponLabel->SetText(szWeaponNumber);
	
				}
				else
				{
					m_pWeaponIcons[COLNUM * i + j]->SetEnabled(0);
					m_pWeaponIcons[COLNUM * i + j]->SetVisible(0);
				}
			}
		}
}

//-----------------------------------------------------------------------------
// Purpose: Keyboard input hook. Return 0 if handled
//-----------------------------------------------------------------------------
int	CHudMenuWeaponSet::HudElementKeyInput(int down, ButtonCode_t keynum, const char *pszCurrentBinding)
{
	if (!ShouldDraw())
	{
		m_iSelectedSlot = -1;
		tf2c_weaponset_show.SetValue(0);
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
		tf2c_weaponset_show.SetValue(0);
		m_iSelectedSlot = -1;
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
	CTFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pPlayer)
	{
		m_iSelectedSlot = iSlot;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::SelectWeapon(int iSlot, int iWeapon)
{
	CTFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pPlayer)
	{
		char szCmd[64];
		Q_snprintf(szCmd, sizeof(szCmd), "weaponpreset %d %d; tf2c_weaponset_show 0", iSlot, iWeapon); //; lastinv
		engine->ExecuteClientCmd(szCmd);
		m_iSelectedSlot = -1;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::FireGameEvent(IGameEvent *event)
{
	if (!ShouldDraw())
	{
		tf2c_weaponset_show.SetValue(0);
		m_iSelectedSlot = -1;
	}
	CHudElement::FireGameEvent(event);
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