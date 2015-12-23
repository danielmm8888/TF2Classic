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
#include "econ_itemschema.h"

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
	EconItemDefinition *m_pItemDefFrom;
	EconItemDefinition *m_pItemDefTo;

private:
	CTFImagePanel *m_pImageFrom;
	CTFImagePanel *m_pImageTo;
};

#endif	// TF_HUD_WEAPONDESIRE_H
