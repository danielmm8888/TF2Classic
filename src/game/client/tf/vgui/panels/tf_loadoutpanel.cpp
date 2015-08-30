#include "cbase.h"
#include "tf_loadoutpanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advmodelpanel.h"
#include "tf_rgbpanel.h"
#include "basemodelpanel.h"
#include <vgui/ILocalize.h>
#include "c_script_parser.h"

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
		m_pWeaponIcons.AddToTail(new CTFAdvButton(m_pWeaponSetPanel, "WeaponIcons", "DUK"));
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

	for (int i = 0; i < INVENTORY_VECTOR_NUM; i++){
		m_pWeaponIcons[i]->SetBounds(0, 0, toProportionalWide(PANEL_WIDE), toProportionalTall(PANEL_TALL));
		m_pWeaponIcons[i]->SetBorderVisible(false);
		m_pWeaponIcons[i]->SetImageInset(25, -5);
		m_pWeaponIcons[i]->GetButton()->SetContentAlignment(CTFAdvButtonBase::GetAlignment("south"));
		m_pWeaponIcons[i]->GetButton()->SetTextInset(0, -10);
		m_pWeaponIcons[i]->SetBorderByString("AdvRoundedButtonDefault", "AdvRoundedButtonArmed", "AdvRoundedButtonDepressed");
	}
	//DefaultLayout();
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
		const char* szPrimary = GetTFInventory()->GetSlotName(TF_WPN_TYPE_PRIMARY);
		const char* szSecondary = GetTFInventory()->GetSlotName(TF_WPN_TYPE_SECONDARY);
		const char* szMelee = GetTFInventory()->GetSlotName(TF_WPN_TYPE_MELEE);
		char strPrimary[40];
		Q_strncpy(strPrimary, command, Q_strlen(szPrimary) + 1);
		char strSecondary[40];
		Q_strncpy(strSecondary, command, Q_strlen(szSecondary) + 1);
		char strMelee[40];
		Q_strncpy(strMelee, command, Q_strlen(szMelee) + 1);
		char buffer[64];
		bool bValid = false;

		if (!Q_strcmp(strPrimary, szPrimary))
		{
			SetCurrentSlot(TF_WPN_TYPE_PRIMARY);
			Q_snprintf(buffer, sizeof(buffer), command + Q_strlen(szPrimary));
			bValid = true;
		}
		else if (!Q_strcmp(strSecondary, szSecondary))
		{
			SetCurrentSlot(TF_WPN_TYPE_SECONDARY);
			Q_snprintf(buffer, sizeof(buffer), command + Q_strlen(szSecondary));
			bValid = true;
		}
		else if (!Q_strcmp(strMelee, szMelee))
		{
			SetCurrentSlot(TF_WPN_TYPE_MELEE);
			Q_snprintf(buffer, sizeof(buffer), command + Q_strlen(szMelee));
			bValid = true;
		}

		if (bValid)
		{
			SetCurrentPreset(atoi(buffer));
			SetWeaponPreset(iCurrentClass, iCurrentSlot, iCurrentPreset);
		}
		else
		{
			BaseClass::OnCommand(command);
		}
	}
}

void CTFLoadoutPanel::SetModelWeapon(int iClass, int iSlot, int iPreset)
{
	int iWeapon = GetTFInventory()->GetWeapon(iClass, iSlot, iPreset);
	if (iWeapon)
	{
		_WeaponData pData;
		if (iWeapon == TF_WEAPON_BUILDER && iClass == TF_CLASS_SPY)
		{
			pData = g_TFWeaponScriptParser.GetTFWeaponInfo(TF_WEAPON_SAPPER);
		}
		else
		{
			pData = g_TFWeaponScriptParser.GetTFWeaponInfo(WeaponIdToAlias(iWeapon));
		}
		m_pClassModelPanel->SetAnimationIndex(pData.m_iWeaponType);
		m_pClassModelPanel->ClearMergeMDLs();
		if (pData.szWorldModel[0] != '\0')
			m_pClassModelPanel->SetMergeMDL(pData.szWorldModel, NULL, iCurrentSkin);
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
	int len = Q_strlen(pszClassModels[iClass]) + 1;
	char *pAlloced = new char[len];
	Assert(pAlloced);
	Q_strncpy(pAlloced, pszClassModels[iClass], len);
	m_pClassModelPanel->m_BMPResData.m_pszModelName = pAlloced;
	m_pClassModelPanel->m_BMPResData.m_nSkin = iCurrentSkin;
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
		int iColCount = 0;
		for (int iSlot = 0; iSlot < INVENTORY_ROWNUM; iSlot++)
		{
			int iCols = 0;
			for (int iPreset = 0; iPreset < INVENTORY_COLNUM; iPreset++)
			{
				int iWeapon = GetTFInventory()->GetWeapon(iClassIndex, iSlot, iPreset);
				CTFAdvButton *m_pWeaponButton = m_pWeaponIcons[INVENTORY_COLNUM * iSlot + iPreset];
				if (iWeapon > 0)
				{
					iCols++;
					if (iCols > iColCount) iColCount = iCols;
					m_pWeaponButton->SetVisible(true);
					m_pWeaponButton->SetPos(iPreset * toProportionalWide(PANEL_WIDE + 10), iSlot * toProportionalTall(PANEL_TALL + 5));

					int iWeapon = GetTFInventory()->GetWeapon(iCurrentClass, iSlot, iPreset);

					_WeaponData pData;
					if (iWeapon == TF_WEAPON_BUILDER && iCurrentClass == TF_CLASS_SPY)
					{
						pData = g_TFWeaponScriptParser.GetTFWeaponInfo(TF_WEAPON_SAPPER);
					}
					else
					{
						pData = g_TFWeaponScriptParser.GetTFWeaponInfo(WeaponIdToAlias(iWeapon));
					}

					char szIcon[64];
					Q_snprintf(szIcon, sizeof(szIcon), "../%s", (iCurrentSkin == 1 ? pData.iconActive : pData.iconInactive));
					m_pWeaponButton->SetImage(szIcon);

					char szWeaponName[32];
					Q_snprintf(szWeaponName, sizeof(szWeaponName), "#%s", pData.szPrintName);
					m_pWeaponButton->SetText(szWeaponName);

					int iWeaponPreset = GetTFInventory()->GetWeaponPreset(filesystem, iClassIndex, iSlot);
					m_pWeaponButton->SetBorderVisible((iPreset == iWeaponPreset));
					m_pWeaponButton->GetButton()->SetSelected((iPreset == iWeaponPreset));

					char szCommand[64];
					Q_snprintf(szCommand, sizeof(szCommand), "%s%i", GetTFInventory()->GetSlotName(iSlot), iPreset);
					m_pWeaponButton->SetCommandString(szCommand);
				}
				else
				{
					m_pWeaponButton->SetVisible(0);
				}
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