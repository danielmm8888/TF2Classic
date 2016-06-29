//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_HUD_WEAPONDESIRE_H
#define TF_HUD_WEAPONDESIRE_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_imagepanel.h"
#include "econ_item_schema.h"

class CTFWeaponInfo;


class CItemModelPanel : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CItemModelPanel, vgui::EditablePanel );

public:
	CItemModelPanel( Panel *parent, const char* name );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void PerformLayout( void );

	virtual void SetWeapon( C_BaseCombatWeapon *pWeapon, int iBorderStyle = -1, int ID = -1 );
	virtual void SetWeapon( CEconItemDefinition *pItemDefinition, int iBorderStyle = -1, int ID = -1 );

private:
	C_BaseCombatWeapon	*m_pWeapon;
	vgui::Label			*m_pWeaponName;
	vgui::Label			*m_pSlotID;
	vgui::ImagePanel	*m_pWeaponImage;
	vgui::HFont			 m_pDefaultFont;
	vgui::HFont			 m_pSelectedFont;
	vgui::HFont			 m_pNumberDefaultFont;
	vgui::HFont			 m_pNumberSelectedFont;
	vgui::IBorder		*m_pDefaultBorder;
	vgui::IBorder		*m_pSelectedRedBorder;
	vgui::IBorder		*m_pSelectedBlueBorder;
	int					 m_iBorderStyle;
	int					 m_ID;
};

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFHudWeaponSwitch : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudWeaponSwitch, vgui::EditablePanel );

public:

	CTFHudWeaponSwitch( const char *pElementName );

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual bool ShouldDraw( void );
	virtual void SetVisible( bool bVisible );
	void OnTick();

private:
	CEconItemDefinition *m_pItemDefFrom;
	CEconItemDefinition *m_pItemDefTo;

private:
	CItemModelPanel *m_pWeaponFrom;
	CItemModelPanel *m_pWeaponTo;
};

#endif	// TF_HUD_WEAPONDESIRE_H
