#include "cbase.h"
#include "tf_loadoutpanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbutton.h"
#include "controls/tf_advmodelpanel.h"
#include <vgui/ILocalize.h>

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//"modelname"		

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
	m_pClassModelPanel = new CTFAdvModelPanel(this, "classmodelpanel");
	m_pWeaponSetPanel = new CTFWeaponSetPanel(this, "weaponsetpanel");

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
		m_pWeaponIcons[i]->SetBounds(0, 0, 300, 150);
		m_pWeaponIcons[i]->SetBGVisible(false);
		m_pWeaponIcons[i]->SetBorderVisible(false);
		m_pWeaponIcons[i]->SetContentAlignment(CTFAdvButtonBase::GetAlignment("south"));
	}
	AutoLayout();
};


void CTFLoadoutPanel::OnCommand(const char* command)
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
	if (!Q_strcmp(strPrimary, szPrimary))
	{
		Q_snprintf(buffer, sizeof(buffer), command + Q_strlen(szPrimary));
		m_pClassModelPanel->SetAnimationIndex(TF_WPN_TYPE_PRIMARY);
		SetWeaponPreset(iCurrentClass, TF_WPN_TYPE_PRIMARY, atoi(buffer));
	}
	else if (!Q_strcmp(strSecondary, szSecondary))
	{
		Q_snprintf(buffer, sizeof(buffer), command + Q_strlen(szSecondary));
		m_pClassModelPanel->SetAnimationIndex(TF_WPN_TYPE_SECONDARY);
		SetWeaponPreset(iCurrentClass, TF_WPN_TYPE_SECONDARY, atoi(buffer));
	}
	else if (!Q_strcmp(strMelee, szMelee))
	{
		Q_snprintf(buffer, sizeof(buffer), command + Q_strlen(szMelee));
		m_pClassModelPanel->SetAnimationIndex(TF_WPN_TYPE_MELEE);
		SetWeaponPreset(iCurrentClass, TF_WPN_TYPE_MELEE, atoi(buffer));
	}
	else if (!Q_strcmp(command, "back") || (!Q_strcmp(command, "vguicancel")))
	{
		Hide();
	}
	else if (!Q_strcmp(command, "select_scout"))
	{
		iCurrentClass = TF_CLASS_SCOUT;
		DefaultLayout();
	}
	else if (!Q_strcmp(command, "select_soldier"))
	{
		iCurrentClass = TF_CLASS_SOLDIER;
		DefaultLayout();
	}
	else if (!Q_strcmp(command, "select_pyro"))
	{
		iCurrentClass = TF_CLASS_PYRO;
		DefaultLayout();
	}
	else if (!Q_strcmp(command, "select_demoman"))
	{
		iCurrentClass = TF_CLASS_DEMOMAN;
		DefaultLayout();
	}
	else if (!Q_strcmp(command, "select_heavyweapons"))
	{
		iCurrentClass = TF_CLASS_HEAVYWEAPONS;
		DefaultLayout();
	}
	else if (!Q_strcmp(command, "select_engineer"))
	{
		iCurrentClass = TF_CLASS_ENGINEER;
		DefaultLayout();
	}
	else if (!Q_strcmp(command, "select_medic"))
	{
		iCurrentClass = TF_CLASS_MEDIC;
		DefaultLayout();
	}
	else if (!Q_strcmp(command, "select_sniper"))
	{
		iCurrentClass = TF_CLASS_SNIPER;
		DefaultLayout();
	}
	else if (!Q_strcmp(command, "select_spy"))
	{
		iCurrentClass = TF_CLASS_SPY;
		DefaultLayout();
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFLoadoutPanel::Show()
{
	BaseClass::Show();
	MAINMENU_ROOT->ShowPanel(SHADEBACKGROUND_MENU);
	//AutoLayout();
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

void CTFLoadoutPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();


	float m_fWide = GetWide() / 5.0;
	float m_fTall = GetTall() / 6.0;


	int iClassIndex = iCurrentClass;
	//int iTeamNumber = TF_TEAM_RED;

	const char *pValue = pszClassModels[iCurrentClass];
	int len = Q_strlen(pValue) + 1;
	char *pAlloced = new char[len];
	Q_strncpy(pAlloced, pValue, len);
	m_pClassModelPanel->m_BMPResData.m_pszModelName = pAlloced;

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
				m_pWeaponButton->SetEnabled(1);
				m_pWeaponButton->SetVisible(1);
				m_pWeaponButton->SetPos(iPreset * m_fWide + m_fWide - 100, iSlot * m_fTall + m_fTall - 80);

				/*
				char* cIcon = GetTFInventory()->GetWeaponBucket(iWeapon, iTeamNumber);
				char szIcon[64];
				Q_snprintf(szIcon, sizeof(szIcon), "../%s", cIcon);
				if (szIcon)
				*/
				m_pWeaponButton->SetImage("class_sel_sm_soldier_blu");

				const char *pszWeaponName = WeaponIdToAlias(iWeapon);
				char szWeaponName[32];
				Q_snprintf(szWeaponName, sizeof(szWeaponName), "#%s", pszWeaponName);
				m_pWeaponButton->SetText(szWeaponName);

				int iWeaponPreset = GetTFInventory()->GetWeaponPreset(filesystem, iClassIndex, iSlot);
				m_pWeaponButton->SetBorderVisible((iPreset == iWeaponPreset));
				m_pWeaponButton->SetSelected((iPreset == iWeaponPreset));

				char szCommand[64];
				Q_snprintf(szCommand, sizeof(szCommand), "%s%i", GetTFInventory()->GetSlotName(iSlot), iPreset);
				m_pWeaponButton->SetCommand(szCommand);
			}
			else
			{
				m_pWeaponButton->SetEnabled(0);
				m_pWeaponButton->SetVisible(0);
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
	Msg("%i %i %i\n", iClass, iSlot, iPreset);
	GetTFInventory()->SetInventory(filesystem, pInventoryKeys);
	DefaultLayout();
}