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

class CTFWeaponInfo;

//-----------------------------------------------------------------------------
// Purpose:  
//-----------------------------------------------------------------------------
class CTFHudWeaponSwitch : public CHudElement, public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE( CTFHudWeaponSwitch, vgui::EditablePanel );

public:

	CTFHudWeaponSwitch(const char *pElementName);

	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual bool ShouldDraw(void);
	void OnTick();

private:
	
	void UpdateStatus( void );
	CTFWeaponInfo *pWeaponFromInfo;
	CTFWeaponInfo *pWeaponToInfo;

private:

};

#endif	// TF_HUD_WEAPONDESIRE_H
