//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#if !defined( PANELLISTPANEL_H )
#define PANELLISTPANEL_H
#ifdef _WIN32
#pragma once
#endif

#include <vgui/VGUI.h>
#include <vgui_controls/Panel.h>
#include "panels/tf_menupanelbase.h"

class KeyValues;

//-----------------------------------------------------------------------------
// Purpose: A list of variable height child panels
//-----------------------------------------------------------------------------
class CPanelListPanel : public CTFMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CPanelListPanel, CTFMenuPanelBase);

public:
	typedef struct dataitem_s
	{
		// Always store a panel pointer
		vgui::Panel *panel;

	} DATAITEM;

	CPanelListPanel(vgui::Panel *parent, char const *panelName, bool inverseButtons = false);
	~CPanelListPanel();

	// DATA & ROW HANDLING
	// The list now owns the panel
	virtual int	computeVPixelsNeeded(void);
	virtual int AddItem(vgui::Panel *panel);
	virtual int	GetItemCount(void);
	virtual vgui::Panel *GetItem(int itemIndex); // returns pointer to data the row holds
	virtual void RemoveItem(int itemIndex); // removes an item from the table (changing the indices of all following items)
	virtual void DeleteAllItems(); // clears and deletes all the memory used by the data items
	virtual vgui::HFont GetFont(){ return pFont; };
	virtual const char* GetFontString(){ return m_szFont; };

	// career-mode UI wants to nudge sub-controls around
	void SetSliderYOffset(int pixels);

	// PAINTING
	virtual vgui::Panel *GetCellRenderer(int row);

	MESSAGE_FUNC_INT(OnSliderMoved, "ScrollBarSliderMoved", position);

	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void ApplySettings(KeyValues *inResourceData);

	vgui::Panel *GetEmbedded()
	{
		return _embedded;
	}

protected:

	DATAITEM	*GetDataItem(int itemIndex);

	virtual void PerformLayout();
	virtual void PaintBackground();
	virtual void OnMouseWheeled(int delta);

private:
	// list of the column headers
	vgui::Dar<DATAITEM *>	_dataItems;
	vgui::ScrollBar		*_vbar;
	vgui::Panel			*_embedded;

	int					_tableStartX;
	int					_tableStartY;
	int					_sliderYOffset;
	char				m_szFont[64];
	char				m_szBorder[64];
	vgui::HFont				pFont;
	//vgui::			pBorder;
	MESSAGE_FUNC_PTR_CHARPTR(OnTextChanged, "TextChanged", panel, text);
	MESSAGE_FUNC_PTR(OnControlModified, "ControlModified", panel);
	MESSAGE_FUNC_PTR(OnCheckButtonChecked, "CheckButtonChecked", panel);
};

#endif // PANELLISTPANEL_H