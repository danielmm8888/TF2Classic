#include "cbase.h"
#include "tf_loadoutpanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advitembutton.h"
#include "controls/tf_advmodelpanel.h"
#include "tf_rgbpanel.h"
#include "basemodelpanel.h"
#include <vgui/ILocalize.h>
#include "script_parser.h"
#include "econ_itemview.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define PANEL_WIDE 110
#define PANEL_TALL 70
#define TF_WEAPON_SAPPER "TF_WEAPON_SAPPER"

static char* pszClassModels[TF_CLASS_COUNT_ALL] =
{
	"",
	"models/player/scout.mdl",
	"models/player/sniper.mdl",
	"models/player/soldier.mdl",
	"models/player/demo.mdl",
	"models/player/medic.mdl",
	"models/player/heavy.mdl",
	"models/player/pyro.mdl",
	"models/player/spy.mdl",
	"models/player/engineer.mdl",
	"",
	"",
};

struct _WeaponData
{
	char szWorldModel[64];
	char iconActive[64];
	char iconInactive[64];
	char szPrintName[64];
	int m_iWeaponType;
};

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFWeaponSetPanel::CTFWeaponSetPanel(vgui::Panel* parent, const char *panelName) : vgui::EditablePanel(parent, panelName)
{
}

void CTFWeaponSetPanel::OnCommand(const char* command)
{
	GetParent()->OnCommand(command);
}

class CTFWeaponScriptParser : public C_ScriptParser
{
public:
	DECLARE_CLASS_GAMEROOT(CTFWeaponScriptParser, C_ScriptParser);

	void Parse(KeyValues *pKeyValuesData, bool bWildcard, const char *szFileWithoutEXT)
	{
		_WeaponData sTemp;
		Q_strncpy(sTemp.szWorldModel, pKeyValuesData->GetString("playermodel", ""), sizeof(sTemp.szWorldModel));
		Q_strncpy(sTemp.szPrintName, pKeyValuesData->GetString("printname", ""), sizeof(sTemp.szPrintName));
		const char *pszWeaponType = pKeyValuesData->GetString("WeaponType");
		sTemp.m_iWeaponType = 0;
		if (!Q_strcmp(pszWeaponType, "primary"))
		{
			sTemp.m_iWeaponType = TF_WPN_TYPE_PRIMARY;
		}
		else if (!Q_strcmp(pszWeaponType, "secondary"))
		{
			sTemp.m_iWeaponType = TF_WPN_TYPE_SECONDARY;
		}
		else if (!Q_strcmp(pszWeaponType, "melee"))
		{
			sTemp.m_iWeaponType = TF_WPN_TYPE_MELEE;
		}
		else if (!Q_strcmp(pszWeaponType, "grenade"))
		{
			sTemp.m_iWeaponType = TF_WPN_TYPE_GRENADE;
		}
		else if (!Q_strcmp(pszWeaponType, "building"))
		{
			sTemp.m_iWeaponType = TF_WPN_TYPE_BUILDING;
		}
		else if (!Q_strcmp(pszWeaponType, "pda"))
		{
			sTemp.m_iWeaponType = TF_WPN_TYPE_PDA;
		}
		for (KeyValues *pData = pKeyValuesData->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey())
		{
			if (!Q_stricmp(pData->GetName(), "TextureData"))
			{
				for (KeyValues *pTextureData = pData->GetFirstSubKey(); pTextureData != NULL; pTextureData = pTextureData->GetNextKey())
				{
					if (!Q_stricmp(pTextureData->GetName(), "weapon"))
					{
						Q_strncpy(sTemp.iconInactive, pTextureData->GetString("file", ""), sizeof(sTemp.iconInactive));
					}
					if (!Q_stricmp(pTextureData->GetName(), "weapon_s"))
					{
						Q_strncpy(sTemp.iconActive, pTextureData->GetString("file", ""), sizeof(sTemp.iconActive));
					}
				}
			}
		}
		m_WeaponInfoDatabase.Insert(szFileWithoutEXT, sTemp);
	};

	_WeaponData GetTFWeaponInfo(const char *name)
	{
		return m_WeaponInfoDatabase[m_WeaponInfoDatabase.Find(name)];
	}

private:
	CUtlDict< _WeaponData, unsigned short > m_WeaponInfoDatabase;
};
CTFWeaponScriptParser g_TFWeaponScriptParser;

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFLoadoutPanel::CTFLoadoutPanel(vgui::Panel* parent, const char *panelName) : CTFDialogPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFLoadoutPanel::~CTFLoadoutPanel()
{
	m_pWeaponIcons.RemoveAll();
	m_pSlideButtons.RemoveAll();
}

bool CTFLoadoutPanel::Init()
{
	BaseClass::Init();

	iCurrentClass = TF_CLASS_SCOUT;
	iCurrentSlot = TF_WPN_TYPE_PRIMARY;
	iCurrentPreset = 0;
	m_pClassModelPanel = new CTFAdvModelPanel(this, "classmodelpanel");
	m_pGameModelPanel = new CModelPanel(this, "gamemodelpanel");
	m_pWeaponSetPanel = new CTFWeaponSetPanel(this, "weaponsetpanel");
	m_pRGBPanel = new CTFRGBPanel(this, "rgbpanel");
	g_TFWeaponScriptParser.InitParser("scripts/tf_weapon_*.txt", true, false);

	for (int i = 0; i < INVENTORY_VECTOR_NUM; i++){
		m_pWeaponIcons.AddToTail(new CTFAdvItemButton(m_pWeaponSetPanel, "WeaponIcons", "DUK"));
	}
	for (int i = 0; i < INVENTORY_ROWNUM * 2; i++){
		m_pSlideButtons.AddToTail(new CTFAdvItemButton(m_pWeaponSetPanel, "SlideButton", "DUK"));
	}
	for (int i = 0; i < INVENTORY_ROWNUM; i++){
		m_RawIDPos.AddToTail(0);
	}


	for (int iClassIndex = 0; iClassIndex < TF_CLASS_COUNT_ALL; iClassIndex++)
	{
		if (pszClassModels[iClassIndex][0] != '\0')
			modelinfo->FindOrLoadModel(pszClassModels[iClassIndex]);
		for (int iSlot = 0; iSlot < INVENTORY_ROWNUM; iSlot++)
			for (int iPreset = 0; iPreset < INVENTORY_COLNUM; iPreset++)
			{
				int iWeapon = GetTFInventory()->GetItem(iClassIndex, iSlot, iPreset);
				EconItemDefinition *pItemData = GetItemSchema()->GetItemDefinition(iWeapon);
				if (pItemData && (iWeapon > 0 || (iClassIndex == TF_CLASS_SCOUT && iSlot == TF_WPN_TYPE_MELEE && iPreset == 0)))
				{
					char pModel[64];
					Q_snprintf(pModel, sizeof(pModel), pItemData->model_world);
					if (!Q_strcmp(pModel, ""))
						Q_snprintf(pModel, sizeof(pModel), pItemData->model_player);
					if (pModel[0] != '\0')
						modelinfo->FindOrLoadModel(pModel);
					//if (pItemData->image_inventory != '\0')
					//	PrecacheMaterial(pItemData->image_inventory);
				}
			}
	}

	return true;
}

void CTFLoadoutPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/LoadoutPanel.res");
}

void CTFLoadoutPanel::PerformLayout()
{
	BaseClass::PerformLayout();
	for (int iSlot = 0; iSlot < INVENTORY_ROWNUM; iSlot++)
	{
		for (int iPreset = 0; iPreset < INVENTORY_COLNUM; iPreset++)
		{
			CTFAdvItemButton *m_pWeaponButton = m_pWeaponIcons[INVENTORY_COLNUM * iSlot + iPreset];
			m_pWeaponButton->SetSize(XRES(PANEL_WIDE), YRES(PANEL_TALL));
			m_pWeaponButton->SetPos(iPreset * XRES((PANEL_WIDE + 10)), iSlot * YRES((PANEL_TALL + 5)));
			m_pWeaponButton->SetBorderVisible(false);
			m_pWeaponButton->SetBorderByString("AdvRoundedButtonDefault", "AdvRoundedButtonArmed", "AdvRoundedButtonDepressed");
			char szCommand[64];
			Q_snprintf(szCommand, sizeof(szCommand), "%s%i", GetTFInventory()->GetSlotName(iSlot), iPreset);
			m_pWeaponButton->SetCommandString(szCommand);
		}

		CTFAdvItemButton *m_pSlideButtonL = m_pSlideButtons[iSlot * 2];
		CTFAdvItemButton *m_pSlideButtonR = m_pSlideButtons[(iSlot * 2) + 1];

		m_pSlideButtonL->SetSize(XRES(10), YRES(PANEL_TALL));
		m_pSlideButtonL->SetPos(0, iSlot * YRES((PANEL_TALL + 5)));
		m_pSlideButtonL->SetText("<");
		m_pSlideButtonL->SetBorderVisible(true);
		m_pSlideButtonL->SetBorderByString("AdvLeftButtonDefault", "AdvLeftButtonArmed", "AdvLeftButtonDepressed");
		char szCommand[64];
		Q_snprintf(szCommand, sizeof(szCommand), "SlideL%i", iSlot);
		m_pSlideButtonL->SetCommandString(szCommand);
		
		m_pSlideButtonR->SetSize(XRES(10), YRES(PANEL_TALL));
		m_pSlideButtonR->SetPos(m_pWeaponSetPanel->GetWide() - XRES(10), iSlot * YRES((PANEL_TALL + 5)));
		m_pSlideButtonR->SetText(">");
		m_pSlideButtonR->SetBorderVisible(true);
		m_pSlideButtonR->SetBorderByString("AdvRightButtonDefault", "AdvRightButtonArmed", "AdvRightButtonDepressed");
		Q_snprintf(szCommand, sizeof(szCommand), "SlideR%i", iSlot);
		m_pSlideButtonR->SetCommandString(szCommand);
	}
};


void CTFLoadoutPanel::SetCurrentClass(int iClass)
{
	if (iCurrentClass == iClass)
		return;

	iCurrentClass = iClass; 	
	DefaultLayout(); 
};


void CTFLoadoutPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "back") || (!Q_strcmp(command, "vguicancel")))
	{
		Hide();
	}
	else if (!Q_strcmp(command, "select_scout"))
	{
		SetCurrentClass(TF_CLASS_SCOUT);
	}
	else if (!Q_strcmp(command, "select_soldier"))
	{
		SetCurrentClass(TF_CLASS_SOLDIER);
	}
	else if (!Q_strcmp(command, "select_pyro"))
	{
		SetCurrentClass(TF_CLASS_PYRO);
	}
	else if (!Q_strcmp(command, "select_demoman"))
	{
		SetCurrentClass(TF_CLASS_DEMOMAN);
	}
	else if (!Q_strcmp(command, "select_heavyweapons"))
	{
		SetCurrentClass(TF_CLASS_HEAVYWEAPONS);
	}
	else if (!Q_strcmp(command, "select_engineer"))
	{
		SetCurrentClass(TF_CLASS_ENGINEER);
	}
	else if (!Q_strcmp(command, "select_medic"))
	{
		SetCurrentClass(TF_CLASS_MEDIC);
	}
	else if (!Q_strcmp(command, "select_sniper"))
	{
		SetCurrentClass(TF_CLASS_SNIPER);
	}
	else if (!Q_strcmp(command, "select_spy"))
	{
		SetCurrentClass(TF_CLASS_SPY);
	}
	else if (!Q_strcmp(command, "select_merc"))
	{
		SetCurrentClass(TF_CLASS_MERCENARY);
	}
	else
	{
		char buffer[64];
		const char* szText;
		char strText[40];

		for (int iType = TF_WPN_TYPE_PRIMARY; iType <= TF_WPN_TYPE_MELEE; iType++)
		{
			szText = GetTFInventory()->GetSlotName(iType);
			Q_strncpy(strText, command, Q_strlen(szText) + 1);
			if (!Q_strcmp(strText, szText))
			{
				Q_snprintf(buffer, sizeof(buffer), command + Q_strlen(szText));
				SetSlotAndPreset(iType, atoi(buffer));
				return;
			}
		}

		for (int i = 0; i < 2; i++)
		{
			szText = (i == 0 ? "SlideL" : "SlideR");
			Q_strncpy(strText, command, Q_strlen(szText) + 1);
			if (!Q_strcmp(strText, szText))
			{
				Q_snprintf(buffer, sizeof(buffer), command + Q_strlen(szText));
				SideRow(atoi(buffer), (i == 0 ? -1 : 1));
				return;
			}
		}

		BaseClass::OnCommand(command);
	}
}

void CTFLoadoutPanel::SetSlotAndPreset(int iSlot, int iPreset)
{
	SetCurrentSlot(iSlot);
	SetCurrentPreset(iPreset);
	SetWeaponPreset(iCurrentClass, iCurrentSlot, iCurrentPreset);
}

void CTFLoadoutPanel::SideRow(int iRow, int iDir)
{
	m_RawIDPos[iRow] += iDir;

	for (int iPreset = 0; iPreset < INVENTORY_COLNUM; iPreset++)
	{
		CTFAdvItemButton *m_pWeaponButton = m_pWeaponIcons[INVENTORY_COLNUM * iRow + iPreset]; 
		int _x, _y;
		m_pWeaponButton->GetPos(_x, _y);
		int x = (iPreset - m_RawIDPos[iRow]) * XRES((PANEL_WIDE + 10));
		AnimationController::PublicValue_t p_AnimHover(x, _y);
		vgui::GetAnimationController()->RunAnimationCommand(m_pWeaponButton, "Position", p_AnimHover, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR, NULL);
	}

	DefaultLayout();
}

void CTFLoadoutPanel::SetModelWeapon(int iClass, int iSlot, int iPreset)
{
	int iWeapon = GetTFInventory()->GetItem(iClass, iSlot, iPreset);
	EconItemDefinition *pItemData = GetItemSchema()->GetItemDefinition(iWeapon);
	if (pItemData)
	{
		char pModel[64];
		Q_snprintf(pModel, sizeof(pModel), pItemData->model_world);
		if (!Q_strcmp(pModel, ""))
			Q_snprintf(pModel, sizeof(pModel), pItemData->model_player);
		int iSlot = pItemData->anim_slot;
		if (iSlot == -1)
			iSlot = pItemData->item_slot;
		m_pClassModelPanel->SetAnimationIndex(iSlot);
		m_pClassModelPanel->ClearMergeMDLs();
		if (pModel[0] != '\0')
			m_pClassModelPanel->SetMergeMDL(pModel, NULL, iCurrentSkin);
	}
	else
	{
		m_pClassModelPanel->SetAnimationIndex(iSlot);
		m_pClassModelPanel->ClearMergeMDLs();
	}
	m_pClassModelPanel->Update();
}

void CTFLoadoutPanel::Show()
{
	BaseClass::Show();
	MAINMENU_ROOT->ShowPanel(SHADEBACKGROUND_MENU);

	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pPlayer)
	{
		int iClass = pPlayer->m_Shared.GetDesiredPlayerClassIndex();
		if (iClass >= TF_CLASS_SCOUT)
			SetCurrentClass(pPlayer->m_Shared.GetDesiredPlayerClassIndex());
	}
	DefaultLayout();
};

void CTFLoadoutPanel::Hide()
{
	BaseClass::Hide();
	MAINMENU_ROOT->HidePanel(SHADEBACKGROUND_MENU);
};


void CTFLoadoutPanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFLoadoutPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFLoadoutPanel::SetModelClass(int iClass)
{
	m_pClassModelPanel->SetModelName(strdup(pszClassModels[iClass]), iCurrentSkin);
}

void CTFLoadoutPanel::UpdateModelPanels()
{
	int iClassIndex = iCurrentClass;

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();

	if (iClassIndex == TF_CLASS_MERCENARY)
	{
		m_pClassModelPanel->SetVisible(false);
		m_pGameModelPanel->SetVisible(true);
		m_pWeaponSetPanel->SetVisible(false);
		m_pRGBPanel->SetVisible(true);

		if (pLocalPlayer && m_pGameModelPanel)
		{
			CModelPanelModel *pPanelModel = m_pGameModelPanel->m_hModel.Get();
			if (pPanelModel)
			{
				int iRed = 0, iGreen = 0, iBlue = 0;
				ConVar *pColorRed = cvar->FindVar("tf2c_setmerccolor_r");
				ConVar *pColorGreen = cvar->FindVar("tf2c_setmerccolor_g");
				ConVar *pColorBlue = cvar->FindVar("tf2c_setmerccolor_b");
				if (pColorRed) iRed = pColorRed->GetInt();
				if (pColorGreen) iGreen = pColorGreen->GetInt();
				if (pColorBlue) iBlue = pColorBlue->GetInt();
				Vector vec = Vector(iRed / 255.0f, iGreen / 255.0f, iBlue / 255.0f);
				pPanelModel->m_nSkin = 8;
				pPanelModel->m_vecModelColor = vec;
			}
		}
	}
	else
	{
		m_pClassModelPanel->SetVisible(true);
		m_pGameModelPanel->SetVisible(false);
		m_pWeaponSetPanel->SetVisible(true);
		m_pRGBPanel->SetVisible(false);

		SetModelClass(iClassIndex);
		int iWeaponPreset = GetTFInventory()->GetWeaponPreset(filesystem, iClassIndex, iCurrentSlot);
		SetModelWeapon(iClassIndex, iCurrentSlot, iWeaponPreset);
	}
}

void CTFLoadoutPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pLocalPlayer && pLocalPlayer->GetTeamNumber() >= TF_TEAM_RED)
	{
		iCurrentSkin = pLocalPlayer->GetTeamNumber() - 2;
	}
	else
	{
		iCurrentSkin = 0;
	}

	UpdateModelPanels();

	int iClassIndex = iCurrentClass;
	SetDialogVariable("classname", g_pVGuiLocalize->Find(g_aPlayerClassNames[iClassIndex]));

	if (iClassIndex != TF_CLASS_MERCENARY)
	{
		for (int iSlot = 0; iSlot < INVENTORY_ROWNUM; iSlot++)
		{
			int iColumnCount = 0;
			int iPresetID = 0;
			int iPos = m_RawIDPos[iSlot];
			CTFAdvItemButton *m_pSlideButtonL = m_pSlideButtons[iSlot * 2];
			CTFAdvItemButton *m_pSlideButtonR = m_pSlideButtons[(iSlot * 2) + 1];
			for (int iPreset = 0; iPreset < INVENTORY_COLNUM; iPreset++)
			{
				int iWeapon = GetTFInventory()->GetItem(iClassIndex, iSlot, iPreset);
				EconItemDefinition *pItemData = GetItemSchema()->GetItemDefinition(iWeapon);
				CTFAdvItemButton *m_pWeaponButton = m_pWeaponIcons[INVENTORY_COLNUM * iSlot + iPreset];
				if (pItemData && (iWeapon > 0 || (iClassIndex == TF_CLASS_SCOUT && iSlot == TF_WPN_TYPE_MELEE && iPreset == 0)))
				{
					m_pWeaponButton->SetVisible(true);
					m_pWeaponButton->SetItemDefinition(pItemData);
					
					int iWeaponPreset = GetTFInventory()->GetWeaponPreset(filesystem, iClassIndex, iSlot);
					m_pWeaponButton->SetBorderVisible((iPreset == iWeaponPreset));
					m_pWeaponButton->GetButton()->SetSelected((iPreset == iWeaponPreset));

					if (iPreset == iWeaponPreset)
						iPresetID = iPreset;
					iColumnCount++;
				}
				else
				{
					m_pWeaponButton->SetVisible(false);
				}
			}
			if (iColumnCount > 2)
			{
				if (iPos == 0)	//left
				{
					m_pSlideButtonL->SetVisible(false);
					m_pSlideButtonR->SetVisible(true);
				}
				else if (iPos == iColumnCount - 2)	//right
				{
					m_pSlideButtonL->SetVisible(true);
					m_pSlideButtonR->SetVisible(false);
				}
				else  //middle
				{
					m_pSlideButtonL->SetVisible(true);
					m_pSlideButtonR->SetVisible(true);
				}
			}
			else
			{
				m_pSlideButtonL->SetVisible(false);
				m_pSlideButtonR->SetVisible(false);
			}
		}
	}
};

void CTFLoadoutPanel::GameLayout()
{
	BaseClass::GameLayout();

};

void CTFLoadoutPanel::SetWeaponPreset(int iClass, int iSlot, int iPreset)
{
	KeyValues* pInventoryKeys = GetTFInventory()->GetInventory(filesystem);
	KeyValues* pClass = pInventoryKeys->FindKey(g_aPlayerClassNames_NonLocalized[iClass]);
	pClass->SetInt(GetTFInventory()->GetSlotName(iSlot), iPreset);
	GetTFInventory()->SetInventory(filesystem, pInventoryKeys);

	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (pPlayer)
	{
		char szCmd[64];
		Q_snprintf(szCmd, sizeof(szCmd), "weaponpreset %d %d", iSlot, iPreset); //; tf2c_weaponset_show 0
		engine->ExecuteClientCmd(szCmd);
	}

	DefaultLayout();
}