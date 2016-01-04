#ifndef TF_ADVITEMBUTTON_H
#define TF_ADVITEMBUTTON_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_advbutton.h"

using namespace vgui;

class CTFButton;
#define pImageColorSelected		(!m_bSelected ? pImageColorDefault : pSelectedColor) 

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAdvItemButton : public CTFAdvButton
{
	friend class CTFButton;
public:
	DECLARE_CLASS_SIMPLE(CTFAdvItemButton, CTFAdvButton);

	CTFAdvItemButton(vgui::Panel *parent, const char *panelName, const char *text);
	~CTFAdvItemButton();
	void Init();
	void PerformLayout();
	void SendAnimation(MouseState flag);
	void SetItemDefinition(CEconItemDefinition *pItemData);
	void SetLoadoutSlot( int iSlot, int iPreset );

protected:
	CEconItemDefinition *m_pItemDefinition;
	int m_iLoadoutSlot;
};


#endif // TF_ADVITEMBUTTON_H
