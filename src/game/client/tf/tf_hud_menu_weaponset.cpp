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
//DECLARE_HUDELEMENT_DEPTH(CHudMenuWeaponSet, 40);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudMenuWeaponSet::CHudMenuWeaponSet(const char *pElementName) : CHudElement(pElementName), EditablePanel(NULL, "HudMenuSelectBase")
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	SetHiddenBits(HIDEHUD_MISCSTATUS);

	/*for (int i = 0; i<3; i++)
	{
	char buf[32];
	Q_snprintf(buf, sizeof(buf), "weaponslot_%d", i + 1);
	m_pWeaponSlot[i] = new EditablePanel(this, buf);

	Q_snprintf(buf, sizeof(buf), "weaponitem_%d", i + 1);
	m_pWeaponItem[i] = new EditablePanel(this, buf);

	Q_snprintf(buf, sizeof(buf), "class_item_blue_%d", i + 1);
	m_pClassItems_Blue[i] = new EditablePanel(this, buf);

	Q_snprintf(buf, sizeof(buf), "class_item_green_%d", i + 1);
	m_pClassItems_Green[i] = new EditablePanel(this, buf);

	Q_snprintf(buf, sizeof(buf), "class_item_yellow_%d", i + 1);
	m_pClassItems_Yellow[i] = new EditablePanel(this, buf);
	}
	*/

	char buf[32];
	Q_snprintf(buf, sizeof(buf), "slotitems_%d", 0);
	m_pSlotList = new EditablePanel(this, buf);
	m_pSlotList->SetVisible(false);

	Q_snprintf(buf, sizeof(buf), "weaponitems_%d", 0);
	m_pWeaponList = new EditablePanel(this, buf);
	m_pWeaponList->SetVisible(false);

	m_pActiveWeaponBG = new CTFImagePanel(this, "ActiveWeapon");
	m_pActiveWeaponBG->SetVisible(false);

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

	//m_iShowingTeam = TF_TEAM_RED;

	//ListenForGameEvent("spy_pda_reset");

	m_iSelectedItem = -1;

	m_pActiveSelection = NULL;

	InvalidateLayout(false, true);

	m_bInConsoleMode = false;

	RegisterForRenderGroup("mid");
}
ConVar tf_weaponset_show("tf_weaponset_show", "0", FCVAR_ARCHIVE, "Use console controller disguise menus. 1 = ON, 0 = OFF.");

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	
	// load control settings...
	LoadControlSettings("resource/UI/weaponselect_menu/HudMenuSelectBase.res");
	//LoadControlSettings("resource/UI/HudWeaponSelection.res");
	//m_pWeaponSlot[0]->LoadControlSettings("resource/UI/weaponselect_menu/primary.res");
	//m_pWeaponSlot[1]->LoadControlSettings("resource/UI/weaponselect_menu/secondary.res");
	//m_pWeaponSlot[2]->LoadControlSettings("resource/UI/weaponselect_menu/melee.res");
	//m_pSlotList->LoadControlSettings("resource/UI/weaponselect_menu/HudMenuSlotSelect.res");
	//m_pWeaponList->LoadControlSettings("resource/UI/weaponselect_menu/HudMenuWeaponSelect.res");
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

	/*if (pPlayer->m_Shared.InCond(TF_COND_TAUNTING))
	return false;*/
	if (pPlayer->m_Shared.InCond(TF_COND_TAUNTING))
		return false;

	return (tf_weaponset_show.GetBool());
	//return (pWpn->GetWeaponID() == TF_WEAPON_PDA_SPY);
}

//-----------------------------------------------------------------------------
// Purpose: draws a selection box
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::DrawBox(int x, int y, int wide, int tall, Color color, float normalizedAlpha, int number)
{
	BaseClass::DrawBox(x, y, wide, tall, color, normalizedAlpha / 255.0f);

	// draw the number
	if (IsPC() && number >= 0)
	{
		Color numberColor = m_NumberColor;
		numberColor[3] *= normalizedAlpha / 255.0f;
		surface()->DrawSetTextColor(numberColor);
		surface()->DrawSetTextFont(m_hNumberFont);
		wchar_t wch = '0' + number;
		surface()->DrawSetTextPos(x + wide - m_flSelectionNumberXPos, y + m_flSelectionNumberYPos);
		surface()->DrawUnicodeChar(wch);
	}
}


void CHudMenuWeaponSet::DrawString(wchar_t *text, int xpos, int ypos, Color col, bool bCenter)
{
	surface()->DrawSetTextColor(col);
	vgui::IScheme* pScheme = vgui::scheme()->GetIScheme(GetScheme());
	vgui::HFont hFont = pScheme->GetFont("Default");
	surface()->DrawSetTextFont(hFont); 
	// set the font	
	//surface()->DrawSetTextFont(m_hTextFont);

	// count the position
	int slen = 0, charCount = 0, maxslen = 0;
	{
		for (wchar_t *pch = text; *pch != 0; pch++)
		{
			if (*pch == '\n')
			{
				// newline character, drop to the next line
				if (slen > maxslen)
				{
					maxslen = slen;
				}
				slen = 0;
			}
			else if (*pch == '\r')
			{
				// do nothing
			}
			else
			{
				slen += surface()->GetCharacterWidth(hFont, *pch);
				charCount++;
			}
		}
	}
	if (slen > maxslen)
	{
		maxslen = slen;
	}

	int x = xpos;

	if (bCenter)
	{
		x = xpos - slen * 0.5;
	}

	surface()->DrawSetTextPos(x, ypos);
	// adjust the charCount by the scan amount
	charCount *= m_flTextScan;
	for (wchar_t *pch = text; charCount > 0; pch++)
	{
		if (*pch == '\n')
		{
			// newline character, move to the next line
			surface()->DrawSetTextPos(x + ((m_flLargeBoxWide - slen) / 2), ypos + (surface()->GetFontTall(hFont) * 1.1f));
		}
		else if (*pch == '\r')
		{
			// do nothing
		}
		else
		{
			surface()->DrawUnicodeChar(*pch);
			charCount--;
		}
	}
}

void  CHudMenuWeaponSet::Paint()
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (!pPlayer)
		return;
	//int nNumSlots = GetNumVisibleSlots();
	//if (nNumSlots <= 0)
	//	return;
	C_BaseCombatWeapon *pSelectedWeapon = NULL;
	pSelectedWeapon = pPlayer->GetActiveWeapon();
	if (!pSelectedWeapon)
		return;
	C_TFPlayerClass* pClass = pPlayer->GetPlayerClass();


	//C_BaseCombatWeapon *pSelectedWeapon;
	//TFPlayerClassData_t* pData = pClass->GetData();
	//int num = pData->m_aWeapons[0];
	//TFPlayerClassData_t *pData = GetPlayerClass()->GetData();

	vgui::IScheme* pScheme = vgui::scheme()->GetIScheme(GetScheme());
	vgui::HFont hFont = pScheme->GetFont("Default");
	surface()->DrawSetTextFont(hFont); // set the font	
	surface()->DrawSetTextPos(100, 100);
	wchar_t *pText = g_pVGuiLocalize->Find(pSelectedWeapon->GetWpnData().szPrintName);
	surface()->DrawUnicodeChar('Q');
	Color ammoColor = Color(236, 236, 180, 255);
	DrawString(pText, 200, 100, ammoColor, true);

	if (m_iSelectedSlot == -1)
		pText = L"Select Slot";
	else
		pText = L"Select Weapon";
	DrawString(pText, 100, 200, ammoColor, true);
	wchar buffer[33];

	//pText = _itow(num, buffer, 10);
	//DrawString(pText, 200, 200, ammoColor, true);

	//DrawAllWeapons
	for (int i = 0; i < 5; i++)
	{
		for (int j = 0; j < 5; j++)
		{
			Color col(255, 255, 255, 255);
			int Weapon = Invenory->GetWeapon(pClass->GetClassIndex() - 1, i, j);
			if (Weapon != 0)
			{	
				//CBaseCombatWeapon* tWeapon = pPlayer->GetWeapon(Weapon);
				//pPlayer->Weapon_OwnsThisID(Weapon);
				//int iWeaponID = Weapon;
				//const char *pszWeaponName = WeaponIdToAlias(iWeaponID);
				//CTFWeaponBase *pWeapon = (CTFWeaponBase *)GiveNamedItem(pszWeaponName);
				pText = _itow(Weapon, buffer, 10);
				DrawString(pText, j * 200 + 200, i * 200 + 200, ammoColor, true);
				CTFWeaponBase *pWeapon = pPlayer->Weapon_OwnsThisID(Weapon);
				if (pWeapon)
				{
					const CHudTexture *pTexture = pWeapon->GetSpriteInactive();
					pTexture->DrawSelf(j * 200 + 200, i * 200 + 200, m_flLargeBoxWide, m_flLargeBoxTall, col);
				}
				else
				{
					pText = L"ERROR";
					Color ammoColor = Color(225, 0, 0, 255);
					DrawString(pText, j * 200 + 200 + 100, i * 200 + 200 + 100, ammoColor, true);
				}
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
		return 1;
	}

	if (!down)
	{
		return 1;
	}
	if (m_iSelectedSlot == -1)
	{
		m_pSlotList->SetVisible(true);
		m_pWeaponList->SetVisible(false);
	}
	else
	{
		m_pSlotList->SetVisible(false);
		m_pWeaponList->SetVisible(true);
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
			//(m_iShowingTeam == TF_TEAM_BLUE) ? 1 : 0;
			//tf_weaponset_show 1
			//SelectWeapon(iSlot, iWeapon);
			SelectSlot(iSlot);
		}
		else
		{
			int iWeapon = keynum - KEY_1;
			//(m_iShowingTeam == TF_TEAM_BLUE) ? 1 : 0;
			//tf_weaponset_show 1
			//SelectSlot(iSlot);
			SelectWeapon(m_iSelectedSlot, iWeapon);
		}
	}
	return 0;
	case KEY_MINUS:
		//ToggleDisguiseTeam();
		return 0;

	case KEY_0:
		// cancel, close the menu
		engine->ExecuteClientCmd("lastinv");
		return 0;

	default:
		return 1;	// key not handled
	}
	//}


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
		m_pSlotList->SetVisible(false);
		m_pWeaponList->SetVisible(true);
		m_iSelectedSlot = iSlot;
		//char szCmd[64];
		//Q_snprintf(szCmd, sizeof(szCmd), "weaponpreset %d %d; tf_weaponset_show 0", iSlot); //; lastinv
		//engine->ExecuteClientCmd(szCmd);
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
		Q_snprintf(szCmd, sizeof(szCmd), "weaponpreset %d %d; tf_weaponset_show 0", iSlot, iWeapon); //; lastinv
		engine->ExecuteClientCmd(szCmd);
		//Q_snprintf(szCmd, sizeof(szCmd), "say \"weaponpreset %d %d\"", iSlot, iWeapon); //; lastinv
		//engine->ExecuteClientCmd(szCmd);
		m_pSlotList->SetVisible(true);
		m_pWeaponList->SetVisible(false);
		m_iSelectedSlot = -1;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::SetSelectedItem(int iSlot)
{
	if (m_iSelectedItem != iSlot)
	{
		m_iSelectedItem = iSlot;

		// move the selection item to the new position
		if (m_pActiveSelection)
		{
			// move the selection background
			//int x, y;
			//m_pClassItems_Blue[m_iSelectedItem - 1]->GetPos(x, y);
			//	m_pActiveSelection->SetPos(x, y);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::FireGameEvent(IGameEvent *event)
{
	//const char * type = event->GetName();

	//if (Q_strcmp(type, "spy_pda_reset") == 0)
	//{
	CTFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pPlayer)
	{
		//bool bShowBlue = (pPlayer->GetTeamNumber() == TF_TEAM_RED);
		if (m_iSelectedSlot == -1)
		{
			m_pSlotList->SetVisible(true);
			m_pWeaponList->SetVisible(false);
		}
		else
		{
			m_pSlotList->SetVisible(false);
			m_pWeaponList->SetVisible(true);
		}
		for (int i = 0; i<3; i++)
		{
			//m_pWeaponSlot[i]->SetVisible(true);
			//m_pWeaponSlot[i]->SetVisible(bShowBlue);
		}

		//m_iShowingTeam = (bShowBlue) ? TF_TEAM_BLUE : TF_TEAM_RED;
	}
	//}
	//else
	//{
	//	CHudElement::FireGameEvent(event);
	//}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudMenuWeaponSet::SetVisible(bool state)
{
	if (state == true)
	{
		// close the weapon selection menu
		engine->ClientCmd("cancelselect");

		/*bool bConsoleMode = (IsConsole() || tf_weaponset_menu_controller_mode.GetBool());

		if (bConsoleMode != m_bInConsoleMode)
		{
		InvalidateLayout(true, true);
		m_bInConsoleMode = bConsoleMode;
		}*/

		// set the %lastinv% dialog var to our binding
		const char *key = engine->Key_LookupBinding("lastinv");
		if (!key)
		{
			key = "< not bound >";
		}

		SetDialogVariable("lastinv", key);

		HideLowerPriorityHudElementsInGroup("mid");
	}
	else
	{
		UnhideLowerPriorityHudElementsInGroup("mid");
	}

	BaseClass::SetVisible(state);
}