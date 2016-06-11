//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include <time.h>

#include "tf_optionsadvancedpanel.h"
#include "tf_mainmenu.h"
#include "tf_menupanelbase.h"
#include "controls/tf_advslider.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advpanellistpanel.h"
#include "controls/tf_advcheckbutton.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_scriptobject.h"
#include "filesystem.h"

#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui_controls/ListPanel.h>
#include <KeyValues.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/MessageBox.h>
#include <vgui_controls/CheckButton.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/TextEntry.h>
#include <vgui/IInput.h>

#include <tier0/vcrmode.h>

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

#define OPTIONS_DIR "cfg"
#define DEFAULT_OPTIONS_FILE OPTIONS_DIR "/user_default.scr"
#define OPTIONS_FILE OPTIONS_DIR "/user.scr"
#define TEST_FILE OPTIONS_DIR "/user_test.scr"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFOptionsAdvancedPanel::CTFOptionsAdvancedPanel(vgui::Panel *parent, const char *panelName) : CTFDialogPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFOptionsAdvancedPanel::~CTFOptionsAdvancedPanel()
{
	delete m_pDescription;
}


bool CTFOptionsAdvancedPanel::Init()
{
	BaseClass::Init();

	m_pListPanel = new CPanelListPanel(this, "PanelListPanel");
	m_pList = NULL;

	m_pDescription = new CInfoDescription(m_pListPanel);
	m_pDescription->InitFromFile(DEFAULT_OPTIONS_FILE);
	m_pDescription->TransferCurrentValues(NULL);

	return true;
}

void CTFOptionsAdvancedPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/OptionsAdvPanel.res");
}

void CTFOptionsAdvancedPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

void CTFOptionsAdvancedPanel::OnKeyCodeTyped(KeyCode code)
{
	// force ourselves to be closed if the escape key it pressed
	if (code == KEY_ESCAPE)
	{
		Hide();
	}
	else
	{
		BaseClass::OnKeyCodeTyped(code);
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsAdvancedPanel::GatherCurrentValues()
{
	if (!m_pDescription)
		return;

	// OK
	CTFAdvCheckButton *pBox;
	TextEntry *pEdit;
	ComboBox *pCombo;
	CTFAdvSlider *pScroll;

	mpcontrol_t *pList;

	CScriptObject *pObj;
	CScriptListItem *pItem;

	char szValue[256];
	char strValue[256];
	float flValue;

	pList = m_pList;
	while (pList)
	{
		pObj = pList->pScrObj;

		if (!pList->pControl)
		{
			pObj->SetCurValue(pObj->curValue);
			pList = pList->next;
			continue;
		}

		switch (pObj->type)
		{
		case O_BOOL:
			pBox = (CTFAdvCheckButton *)pList->pControl;
			sprintf(szValue, "%s", pBox->IsSelected() ? "1" : "0");
			break;
		case O_NUMBER:
			pEdit = (TextEntry *)pList->pControl;
			pEdit->GetText(strValue, sizeof(strValue));
			sprintf(szValue, "%s", strValue);
			break;
		case O_SLIDER:
			pScroll = (CTFAdvSlider *)pList->pControl;
			flValue = pScroll->GetValue();
			sprintf(szValue, "%f", flValue);
			break;
		case O_STRING:
			pEdit = (TextEntry *)pList->pControl;
			pEdit->GetText(strValue, sizeof(strValue));
			sprintf(szValue, "%s", strValue);
			break;
		case O_CATEGORY:
			break;
		case O_LIST:
			pCombo = (ComboBox *)pList->pControl;
			pCombo->GetText( strValue, sizeof( strValue ) );
			int activeItem = pCombo->GetActiveItem();

			pItem = pObj->pListItems;
			//			int n = (int)pObj->fcurValue;

			while (pItem)
			{
				if (!activeItem--)
					break;

				pItem = pItem->pNext;
			}

			if (pItem)
			{
				sprintf(szValue, "%s", pItem->szValue);
			}
			else  // Couln't find index
			{
				//assert(!("Couldn't find string in list, using default value"));
				sprintf(szValue, "%s", pObj->curValue);
			}
			break;
		}

		// Remove double quotes and % characters
		UTIL_StripInvalidCharacters(szValue, sizeof(szValue));

		strcpy(strValue, szValue);

		pObj->SetCurValue(strValue);

		pList = pList->next;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsAdvancedPanel::CreateControls()
{
	BaseClass::CreateControls();

	// Go through desciption creating controls
	CScriptObject *pObj;
	pObj = m_pDescription->pObjList;
	
	mpcontrol_t	*pCtrl;
	CTFAdvCheckButton *pBox;
	TextEntry *pEdit;
	ComboBox *pCombo;
	CTFAdvSlider *pScroll;
	CTFAdvButton *pTitle;
	CScriptListItem *pListItem;
	

	Panel *objParent = m_pListPanel;
	while (pObj)
	{
		//Msg("\nAdded: %s %s %f %f %i\n", pObj->prompt, pObj->cvarname, pObj->fcurValue, pObj->fcurValue, pObj->type);
		
		if (pObj->type == O_OBSOLETE)
		{
			pObj = pObj->pNext;
			continue;
		}

		pCtrl = new mpcontrol_t(objParent, "mpcontrol_t");
		pCtrl->type = pObj->type;

	
		switch (pCtrl->type)
		{
		case O_BOOL:
			pBox = new CTFAdvCheckButton(pCtrl, "DescCheckButton", pObj->prompt);
			pBox->SetSelected(pObj->fcurValue != 0.0f ? true : false);
			pBox->SetCommandString(pObj->cvarname);
			pBox->GetButton()->SetFontByString(m_pListPanel->GetFontString());
			if (pObj->tooltip[0] != '\0')
			{
				wchar_t *pText = g_pVGuiLocalize->Find(pObj->tooltip);
				if (pText != NULL)
				{
					char pszToolTipLocal[256];
					wcstombs(pszToolTipLocal, pText, sizeof(pszToolTipLocal));
					pBox->SetToolTip(pszToolTipLocal);
				}
				else
				{
					pBox->SetToolTip(pObj->tooltip);
				}
			}
			pCtrl->pControl = (Panel *)pBox;
			break;
		case O_STRING:
		case O_NUMBER:
			pEdit = new TextEntry(pCtrl, "DescTextEntry");
			pEdit->InsertString(pObj->curValue);
			pCtrl->pControl = (Panel *)pEdit;
			break;
		case O_SLIDER:
			pScroll = new CTFAdvSlider(pCtrl, "DescScrollEntry", pObj->prompt);
			pScroll->ShowInt( false );
			pScroll->SetValue(pObj->fcurValue);
			pScroll->SetCommandString(pObj->cvarname);
			pScroll->SetMinMax(pObj->fMin, pObj->fMax);
			pScroll->GetButton()->SetFontByString(m_pListPanel->GetFontString());
			pCtrl->pControl = (Panel *)pScroll;
			break;
		case O_LIST:
			pCombo = new ComboBox(pCtrl, "DescComboBox", 5, false);

			pListItem = pObj->pListItems;
			while (pListItem)
			{
				pCombo->AddItem(pListItem->szItemText, NULL);
				pListItem = pListItem->pNext;
			}

			pCombo->ActivateItemByRow((int)pObj->fcurValue);

			pCtrl->pControl = (Panel *)pCombo;
			break;
		case O_CATEGORY:
			pTitle = new CTFAdvButton(pCtrl, "DescTextTitle", pObj->prompt);
			pTitle->SetEnabled(false);
			pTitle->SetBorderByString("AdvSettingsTitleBorder");
			pTitle->SetBorderVisible(true);
			pTitle->GetButton()->SetFontByString("MenuSmallFont");
			pCtrl->pControl = (Panel *)pTitle;
			break;
		default:
			break;
		}

		if (pCtrl->type != O_BOOL && pCtrl->type != O_SLIDER && pCtrl->type != O_CATEGORY)
		{
			pCtrl->pPrompt = new vgui::Label(pCtrl, "DescLabel", "");
			pCtrl->pPrompt->SetFont(m_pListPanel->GetFont());
			pCtrl->pPrompt->SetContentAlignment(vgui::Label::a_west);
			pCtrl->pPrompt->SetTextInset(5, 0);
			pCtrl->pPrompt->SetText(pObj->prompt);
		}

		pCtrl->pScrObj = pObj;
		int h = m_pListPanel->GetTall() / 13.0; //(float)GetParent()->GetTall() / 15.0;
		pCtrl->SetSize(800, h);
		//pCtrl->SetBorder( scheme()->GetBorder(1, "DepressedButtonBorder") );
		m_pListPanel->AddItem(pCtrl);

		// Link it in
		if (!m_pList)
		{
			m_pList = pCtrl;
			pCtrl->next = NULL;
		}
		else
		{
			mpcontrol_t *p;
			p = m_pList;
			while (p)
			{
				if (!p->next)
				{
					p->next = pCtrl;
					pCtrl->next = NULL;
					break;
				}
				p = p->next;
			}
		}
		
		pObj = pObj->pNext;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsAdvancedPanel::DestroyControls()
{
	BaseClass::DestroyControls();
	m_pList = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsAdvancedPanel::SaveValues()
{
	// Get the values from the controls:
	GatherCurrentValues();

	// Create the game.cfg file
	if (m_pDescription)
	{
		FileHandle_t fp;

		// Add settings to config.cfg
		m_pDescription->WriteToConfig();

		g_pFullFileSystem->CreateDirHierarchy(OPTIONS_DIR);
		fp = g_pFullFileSystem->Open(OPTIONS_FILE, "wb");
		if (fp)
		{
			m_pDescription->WriteToScriptFile(fp);
			g_pFullFileSystem->Close(fp);
		}
	}
}

void CTFOptionsAdvancedPanel::OnApplyChanges()
{
	BaseClass::OnApplyChanges();
	SaveValues();
}


//-----------------------------------------------------------------------------
// Purpose: Constructor, load/save client settings object
//-----------------------------------------------------------------------------
CInfoDescription::CInfoDescription(CPanelListPanel *panel)
	: CDescription(panel)
{
	setHint("// NOTE:  THIS FILE IS AUTOMATICALLY REGENERATED, \r\n\
			//DO NOT EDIT THIS HEADER, YOUR COMMENTS WILL BE LOST IF YOU DO\r\n\
			// User options script\r\n\
			//\r\n\
			// Format:\r\n\
			//  Version [float]\r\n\
			//  Options description followed by \r\n\
			//  Options defaults\r\n\
			//\r\n\
			// Option description syntax:\r\n\
			//\r\n\
			//  \"cvar\" { \"Prompt\" { type [ type info ] } { default } }\r\n\
			//\r\n\
			//  type = \r\n\
			//   BOOL   (a yes/no toggle)\r\n\
			//   STRING\r\n\
			//   NUMBER\r\n\
			//   LIST\r\n\
			//\r\n\
			// type info:\r\n\
			// BOOL                 no type info\r\n\
			// NUMBER       min max range, use -1 -1 for no limits\r\n\
			// STRING       no type info\r\n\
			// LIST         "" delimited list of options value pairs\r\n\
			//\r\n\
			//\r\n\
			// default depends on type\r\n\
			// BOOL is \"0\" or \"1\"\r\n\
			// NUMBER is \"value\"\r\n\
			// STRING is \"value\"\r\n\
			// LIST is \"index\", where index \"0\" is the first element of the list\r\n\r\n\r\n");

	setDescription("INFO_OPTIONS");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CInfoDescription::WriteScriptHeader(FileHandle_t fp)
{
	char am_pm[] = "AM";
	tm newtime;
	VCRHook_LocalTime(&newtime);

	g_pFullFileSystem->FPrintf(fp, (char *)getHint());

	// Write out the comment and Cvar Info:
	g_pFullFileSystem->FPrintf(fp, "// Half-Life User Info Configuration Layout Script (stores last settings chosen, too)\r\n");
	g_pFullFileSystem->FPrintf(fp, "// File generated:  %.19s %s\r\n", asctime(&newtime), am_pm);
	g_pFullFileSystem->FPrintf(fp, "//\r\n//\r\n// Cvar\t-\tSetting\r\n\r\n");
	g_pFullFileSystem->FPrintf(fp, "VERSION %.1f\r\n\r\n", SCRIPT_VERSION);
	g_pFullFileSystem->FPrintf(fp, "DESCRIPTION INFO_OPTIONS\r\n{\r\n");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CInfoDescription::WriteFileHeader(FileHandle_t fp)
{
	char am_pm[] = "AM";
	tm newtime;
	VCRHook_LocalTime(&newtime);

	g_pFullFileSystem->FPrintf(fp, "// Half-Life User Info Configuration Settings\r\n");
	g_pFullFileSystem->FPrintf(fp, "// DO NOT EDIT, GENERATED BY HALF-LIFE\r\n");
	g_pFullFileSystem->FPrintf(fp, "// File generated:  %.19s %s\r\n", asctime(&newtime), am_pm);
	g_pFullFileSystem->FPrintf(fp, "//\r\n//\r\n// Cvar\t-\tSetting\r\n\r\n");
}

//-----------------------------------------------------------------------------