//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_MENU_WEAPONSET_H
#define TF_HUD_MENU_WEAPONSET_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/Label.h>
#include "tf_imagepanel.h"
#include "tf_inventory.h"


using namespace vgui;

#define ALL_BUILDINGS	-1

class CHudMenuWeaponSet : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE(CHudMenuWeaponSet, EditablePanel);

public:
	CHudMenuWeaponSet(const char *pElementName);

	virtual void	ApplySchemeSettings(IScheme *scheme);
	virtual bool	ShouldDraw(void);

	virtual void	FireGameEvent(IGameEvent *event);
	virtual void	Paint();

	virtual void	SetVisible(bool state);

	int	HudElementKeyInput(int down, ButtonCode_t keynum, const char *pszCurrentBinding);

	virtual int GetRenderGroupPriority(void) { return 50; }

private:
	void SetSelectedItem(int iSlot);

	void SelectWeapon(int iSlot, int iWeapon);
	void SelectSlot(int iSlot);

	C_BaseCombatWeapon* GetWeaponInSlot(int iSlot, int iSlotPos);
	void DrawString(wchar_t *text, int xpos, int ypos, Color col, bool bCenter = false);
	C_BaseCombatWeapon* GetFirstPos(int iSlot);
	void DrawBox(int x, int y, int wide, int tall, Color color, float normalizedAlpha, int number);

private:
	//EditablePanel *m_pWeaponSlot[3];
	//EditablePanel *m_pWeaponItem[3];
	EditablePanel *m_pSlotList;
	EditablePanel *m_pWeaponList;
	CTFInventory* Invenory;

	EditablePanel *m_pActiveSelection;
	int m_iSelectedSlot;

	int m_iShowingTeam;

	int m_iSelectedItem;

	bool m_bInConsoleMode;

	int m_iDemoModeSlot;
	CTFImagePanel *m_pActiveWeaponBG;
	int m_iBGImage_Inactive;
	int m_iBGImage_Blue;
	int m_iBGImage_Red;
	int m_iBGImage_Green;
	int m_iBGImage_Yellow;
	CPanelAnimationVar(vgui::HFont, m_hNumberFont, "NumberFont", "HudSelectionText");
	CPanelAnimationVar(vgui::HFont, m_hTextFont, "TextFont", "HudSelectionText");

	CPanelAnimationVarAliasType(float, m_flSmallBoxWide, "SmallBoxWide", "32", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flSmallBoxTall, "SmallBoxTall", "21", "proportional_float");

	CPanelAnimationVarAliasType(float, m_flPlusStyleBoxWide, "PlusStyleBoxWide", "120", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flPlusStyleBoxTall, "PlusStyleBoxTall", "84", "proportional_float");
	CPanelAnimationVar(float, m_flPlusStyleExpandPercent, "PlusStyleExpandSelected", "0.3")

		CPanelAnimationVarAliasType(float, m_flLargeBoxWide, "LargeBoxWide", "108", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flLargeBoxTall, "LargeBoxTall", "72", "proportional_float");

	CPanelAnimationVarAliasType(float, m_flBoxGap, "BoxGap", "12", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flRightMargin, "RightMargin", "0", "proportional_float");

	CPanelAnimationVarAliasType(float, m_flSelectionNumberXPos, "SelectionNumberXPos", "4", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flSelectionNumberYPos, "SelectionNumberYPos", "4", "proportional_float");

	CPanelAnimationVarAliasType(float, m_flIconXPos, "IconXPos", "16", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flIconYPos, "IconYPos", "8", "proportional_float");

	CPanelAnimationVarAliasType(float, m_flTextYPos, "TextYPos", "35", "proportional_float");
	CPanelAnimationVarAliasType(float, m_flErrorYPos, "ErrorYPos", "60", "proportional_float");

	CPanelAnimationVar(float, m_flAlphaOverride, "Alpha", "255");
	CPanelAnimationVar(float, m_flSelectionAlphaOverride, "SelectionAlpha", "255");

	CPanelAnimationVar(Color, m_TextColor, "TextColor", "SelectionTextFg");
	CPanelAnimationVar(Color, m_NumberColor, "NumberColor", "SelectionNumberFg");
	CPanelAnimationVar(Color, m_EmptyBoxColor, "EmptyBoxColor", "SelectionEmptyBoxBg");
	CPanelAnimationVar(Color, m_BoxColor, "BoxColor", "SelectionBoxBg");
	CPanelAnimationVar(Color, m_SelectedBoxColor, "SelectedBoxClor", "SelectionSelectedBoxBg");

	CPanelAnimationVar(float, m_flWeaponPickupGrowTime, "SelectionGrowTime", "0.1");

	CPanelAnimationVar(float, m_flTextScan, "TextScan", "1.0");

	CPanelAnimationVar(int, m_iMaxSlots, "MaxSlots", "6");
	CPanelAnimationVar(bool, m_bPlaySelectionSounds, "PlaySelectSounds", "1");


};

#endif	// TF_HUD_MENU_WEAPONSET_H