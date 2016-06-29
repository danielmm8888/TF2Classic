#ifndef tf_advbuttonbase_H
#define tf_advbuttonbase_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_mainmenu.h"
#include <vgui_controls/EditablePanel.h>
#include "tf_imagepanel.h"
#include "tf_controls.h"

using namespace vgui;

class CTFButtonBase;
class CTFToolTipPanel;
class CTFDialogPanelBase;

enum MouseState
{
	MOUSE_DEFAULT,
	MOUSE_ENTERED,
	MOUSE_EXITED,
	MOUSE_PRESSED,
	MOUSE_RELEASED
};

#define DEFAULT_PATH		"../vgui/main_menu/"
#define DEFAULT_BG			"MainMenuAdvButtonDefault"
#define ARMED_BG			"MainMenuAdvButtonArmed"
#define DEPRESSED_BG		"MainMenuAdvButtonDepressed"
#define DEFAULT_BORDER		"AdvRoundedButtonDefault"
#define ARMED_BORDER		"AdvRoundedButtonArmed"
#define DEPRESSED_BORDER	"AdvRoundedButtonDepressed"
#define DEFAULT_COLOR		"AdvTextDefault"
#define ARMED_COLOR			"AdvTextArmed"
#define DEPRESSED_COLOR		"AdvTextDepressed"
#define DEFAULT_FONT		"MenuSmallFont"
#define DEFAULT_IMAGE		""
#define EMPTY_STRING		""
#define GETSCHEME()			scheme()->GetIScheme(GetScheme())
#define pSelectedBG			(!m_bSelected ? pDefaultBG : pArmedBG) 

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFAdvButtonBase : public vgui::EditablePanel
{
	friend class CTFButtonBase;
public:
	DECLARE_CLASS_SIMPLE(CTFAdvButtonBase, vgui::EditablePanel);

	CTFAdvButtonBase(vgui::Panel *parent, const char *panelName, const char *text);
	virtual ~CTFAdvButtonBase();
	virtual void Init();
	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PerformLayout();

	virtual void SendAnimation(MouseState flag);
	virtual void SetDefaultAnimation();
	virtual void SetCommandString(const char *sCommand);
	virtual const char* GetCommandString();
	virtual void SetAutoChange(bool bAutoChange) { m_bAutoChange = bAutoChange; };
	virtual bool IsAutoChange() { return m_bAutoChange; };
	virtual void SetBorderByString(const char *sBorderDefault, const char *sBorderArmed = NULL, const char *sBorderDepressed = NULL);
	virtual void SetBorderVisible(bool bVisible);

	virtual void SetImage(const char *sImage){ pButtonImage->SetImage(sImage); };
	virtual void SetImageInset(int iInsetX, int iInsetY);
	virtual void SetImageSize(int iWide, int iTall);
	virtual void SetToolTip(const char *sText);

	virtual void ShowInt( bool bShow ) { m_bShowInt = bShow; }

	virtual void OnThink();
	static	vgui::Label::Alignment GetAlignment(char* m_szAlignment);

protected:
	bool			m_bBorderVisible;
	bool			m_bShowInt;
	bool			m_bAutoChange;
	bool			m_bSelected;
	
	char			pDefaultBG[64];
	char			pArmedBG[64];
	char			pDepressedBG[64];
	char			pDefaultBorder[64];
	char			pArmedBorder[64];
	char			pDepressedBorder[64];
	char			pDefaultColor[64];
	char			pArmedColor[64];
	char			pDepressedColor[64];
	char			pSelectedColor[64];
	char			m_szCommand[64];
	char			pDefaultButtonImage[64];
	char			pImageColorDefault[64];
	char			pImageColorArmed[64];
	char			pImageColorDepressed[64];
	char			pToolTip[256];

	CTFButtonBase	*pButton;
	ImagePanel		*pButtonImage;
	float			m_fImageWidth;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFButtonBase : public Button
{
	friend class CTFAdvButtonBase;
public:
	DECLARE_CLASS_SIMPLE(CTFButtonBase, Button);

	CTFButtonBase(vgui::Panel *parent, const char *panelName, const char *text);

	virtual void ApplySchemeSettings(IScheme *pScheme);
	virtual void ApplySettings(KeyValues *inResourceData);

	// Set armed button border attributes.
	virtual void SetArmedBorder(vgui::IBorder *border);
	virtual void SetSelectedBorder(vgui::IBorder *border);
	// Get button border attributes.
	virtual IBorder *GetBorder(bool depressed, bool armed, bool selected, bool keyfocus);

	virtual void OnCursorExited();
	virtual void OnCursorEntered();
	virtual void OnMousePressed(vgui::MouseCode code);
	virtual void OnMouseReleased(vgui::MouseCode code);
	virtual MouseState GetState() { return iState; };
	//virtual void SetParent(CTFAdvButtonBase *m_pButton) { m_pParent = m_pButton; };
	//virtual char *GetCommandStr() { return m_pParent->m_szCommand; };
	void SetFontByString(const char *sFont);

protected:
	virtual void	SetMouseEnteredState(MouseState flag);
	IBorder			*_armedBorder;
	IBorder			*_selectedBorder;
	bool			m_bBorderVisible;
	MouseState		iState;
};


#endif // tf_advbuttonbase_H
