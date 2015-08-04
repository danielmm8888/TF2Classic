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
#include "tf_controls.h"


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

	virtual void	SetVisible(bool state);
	void			DefaultLayout();
	void			UpdateLayout();

	int	HudElementKeyInput(int down, ButtonCode_t keynum, const char *pszCurrentBinding);

	virtual int GetRenderGroupPriority(void) { return 50; }

private:
	void SetSelectedItem(int iSlot);

	void SelectWeapon(int iSlot, int iWeapon);
	void SelectSlot(int iSlot);


private:
	CTFImagePanel				*m_pWeaponBucket;
	CTFImagePanel				*m_pSelection;
	CExLabel					*m_pWeaponLabel;
	EditablePanel				*m_pWeaponIconBase;
	EditablePanel				*m_pMainBackground;
	CTFImagePanel				*m_pSelectedSlot;
	CExLabel					*m_pStatusLabel;
	CUtlVector<EditablePanel*>	m_pWeaponIcons;

	int m_iSelectedSlot;
	int m_iShowingTeam;

	float m_fWide;
	float m_fTall;
	int m_iSelect_X;
	int m_iSelect_Y;

	CTFImagePanel *m_pActiveWeaponBG;
};

#endif	// TF_HUD_MENU_WEAPONSET_H