#ifndef TFMAINMENULOADOUTPANEL_H
#define TFMAINMENULOADOUTPANEL_H

#include "tf_dialogpanelbase.h"
#include "tf_inventory.h"

class CTFAdvModelPanel;
class CTFWeaponSetPanel;
class CTFAdvButton;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFLoadoutPanel : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFLoadoutPanel, CTFDialogPanelBase);

public:
	CTFLoadoutPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFLoadoutPanel();
	bool Init();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void OnCommand(const char* command);
	void Hide();
	void Show();
	void DefaultLayout();
	void GameLayout();
	void SetWeaponPreset(int iClass, int iSlot, int iPreset);
	void SetCurrentClass(int iClass) { iCurrentClass = iClass; DefaultLayout(); };
	void SetCurrentSlot(int iSlot) { iCurrentSlot = iSlot; };
	void SetCurrentPreset(int iPreset) { iCurrentPreset = iPreset; };
	void SetModelWeapon(int iClass, int iSlot, int iPreset);
	void SetModelClass(int iClass);

private:
	CTFAdvModelPanel *m_pClassModelPanel;
	CTFWeaponSetPanel *m_pWeaponSetPanel;
	CUtlVector<CTFAdvButton*>	m_pWeaponIcons;
	int	iCurrentClass;
	int	iCurrentSlot;
	int	iCurrentPreset;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFWeaponSetPanel : public vgui::EditablePanel
{
	DECLARE_CLASS_SIMPLE(CTFWeaponSetPanel, vgui::EditablePanel);

public:
	CTFWeaponSetPanel(vgui::Panel* parent, const char *panelName);
	void OnCommand(const char* command);
};

#endif // TFMAINMENULOADOUTPANEL_H