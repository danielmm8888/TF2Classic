#ifndef TF_MAINMENU_TABS_H
#define TF_MAINMENU_TABS_H
#ifdef _WIN32
#pragma once
#endif

using namespace vgui;

class CTFAdvButton;

class CAdvTabs : public vgui::EditablePanel
{
public:
	DECLARE_CLASS_SIMPLE(CAdvTabs, vgui::EditablePanel);

	CAdvTabs(vgui::Panel *parent, char const *panelName);
	~CAdvTabs();

	virtual void PerformLayout();
	virtual void PaintBackground();

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void ApplySettings(KeyValues *inResourceData);

	void OnCommand(const char* command);

	virtual void DeleteAllItems(); // clears and deletes all the memory used by the data items

protected:
	int	iOffset;
	CUtlVector<CTFAdvButton*>	m_pButtons;
	CTFAdvButton*	m_pCurrentButton;
	MESSAGE_FUNC_PTR(OnButtonPressed, "ButtonPressed", panel);
};

#endif // TF_MAINMENU_TABS_H