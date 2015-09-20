//===== Copyright © 1996-2005, Valve Corporation, All rights reserved. ======//
//
// Purpose: 
//
// $NoKeywords: $
//===========================================================================//

#ifndef OPTIONS_SUB_KEYBOARD_H
#define OPTIONS_SUB_KEYBOARD_H
#ifdef _WIN32
#pragma once
#endif

#include "utlvector.h"
#include "utlsymbol.h"

#include "tf_dialogpanelbase.h"
class vcontrolslistpanel;
class CTFAdvButton;

//-----------------------------------------------------------------------------
// Purpose: Keyboard Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class CTFOptionsKeyboardPanel : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFOptionsKeyboardPanel, CTFDialogPanelBase);

public:
	CTFOptionsKeyboardPanel(vgui::Panel* parent, const char *panelName);
	~CTFOptionsKeyboardPanel();
	virtual bool Init();
	virtual void OnResetData();
	virtual void OnApplyChanges();
	virtual void OnSetDefaults();
	virtual void OnKeyCodePressed( vgui::KeyCode code );
	virtual void OnCommand(const char *command);
	virtual void OnThink();

	// Trap row selection message
	MESSAGE_FUNC_INT( ItemSelected, "ItemSelected", itemID );

protected:
	void ApplySchemeSettings(vgui::IScheme *pScheme);

private:
	void Finish( ButtonCode_t code );

	//-----------------------------------------------------------------------------
	// Purpose: Used for saving engine keybindings in case user hits cancel button
	//-----------------------------------------------------------------------------
	struct KeyBinding
	{
		char *binding;
	};

	// Create the key binding list control
	void			CreateKeyBindingList( void );

	// Tell engine to bind/unbind a key
	void			BindKey( const char *key, const char *binding );
	void			UnbindKey( const char *key );

	// Save/restore/cleanup engine's current bindings ( for handling cancel button )
	void			SaveCurrentBindings( void );
	void			DeleteSavedBindings( void );

	// Get column 0 action descriptions for all keys
	void			ParseActionDescriptions( void );

	// Populate list of actions with current engine keybindings
	void			FillInCurrentBindings( void );
	// Remove all current bindings from list of bindings
	void			ClearBindItems( void );
	// Fill in bindings with mod-specified defaults
	void			FillInDefaultBindings( void );
	// Copy bindings out of list and set them in the engine
	void			ApplyAllBindings( void );

	// Bind a key to the item
	void			AddBinding( KeyValues *item, const char *keyname );

	// Remove all instances of a key from all bindings
	void			RemoveKeyFromBindItems( KeyValues *org_item, const char *key );

	// Find item by binding name
	KeyValues *GetItemForBinding( const char *binding );

private:
	void OpenKeyboardAdvancedDialog();
	vgui::DHANDLE<class CTFOptionsKeyboardPanelAdvancedDlg> m_OptionsSubKeyboardAdvancedDlg;
	virtual void OnKeyCodeTyped(vgui::KeyCode code);

	vcontrolslistpanel	*m_pKeyBindList;

	CTFAdvButton *m_pSetBindingButton;
	CTFAdvButton *m_pClearBindingButton;

	// List of saved bindings for the keys
	KeyBinding m_Bindings[ BUTTON_CODE_LAST ];

	// List of all the keys that need to have their binding removed
	CUtlVector<CUtlSymbol> m_KeysToUnbind;
	bool bParsed;
};

#endif // OPTIONS_SUB_KEYBOARD_H