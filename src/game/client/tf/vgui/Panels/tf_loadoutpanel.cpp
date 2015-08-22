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
	iCurrentSlot = TF_WPN_TYPE_PRIMARY;
	iCurrentPreset = 0;
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

	float wide = CTFAdvButtonBase::GetProportionalWideScale();
	float tall = CTFAdvButtonBase::GetProportionalWideScale();

	for (int i = 0; i < INVENTORY_VECTOR_NUM; i++){
		m_pWeaponIcons[i]->SetBounds(0, 0, 120 * wide, 55 * tall);
		m_pWeaponIcons[i]->SetBorderVisible(false);
		m_pWeaponIcons[i]->GetButton()->SetContentAlignment(CTFAdvButtonBase::GetAlignment("south"));
		m_pWeaponIcons[i]->SetShouldScaleImage(false);
		m_pWeaponIcons[i]->SetBorderByString("AdvRoundedButtonDefault", "AdvRoundedButtonArmed", "AdvRoundedButtonDepressed");
	}
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
	CTFWeaponInfo *pWeapon = GetTFWeaponInfo(iWeapon);
	if (pWeapon)
	{
		m_pClassModelPanel->SetAnimationIndex(pWeapon->m_iWeaponType);
		m_pClassModelPanel->ClearMergeMDLs();	
		m_pClassModelPanel->SetMergeMDL(pWeapon->szWorldModel);
		m_pClassModelPanel->Update();
	}
}

void CTFLoadoutPanel::Show()
{
	BaseClass::Show();
	MAINMENU_ROOT->ShowPanel(SHADEBACKGROUND_MENU);
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
}

void CTFLoadoutPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
	
	int iClassIndex = iCurrentClass;
	SetModelClass(iClassIndex);
	int iWeaponPreset = GetTFInventory()->GetWeaponPreset(filesystem, iClassIndex, iCurrentSlot);
	SetModelWeapon(iClassIndex, iCurrentSlot, iWeaponPreset);

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
				m_pWeaponButton->SetVisible(1);

				float wide = CTFAdvButtonBase::GetProportionalWideScale();
				float tall = CTFAdvButtonBase::GetProportionalWideScale();
				m_pWeaponButton->SetPos(iPreset * 125 * wide, iSlot * 60 * tall);

				int iWeapon = GetTFInventory()->GetWeapon(iCurrentClass, iSlot, iPreset);
				CTFWeaponInfo *pWeapon = GetTFWeaponInfo(iWeapon);
				if (pWeapon)
				{
					char szIcon[64];
					Q_snprintf(szIcon, sizeof(szIcon), "../%s", pWeapon->iconInactive->szTextureFile);
					m_pWeaponButton->SetImage(szIcon);
				}
				else 
				{
					m_pWeaponButton->SetImage("class_sel_sm_soldier_blu");
				}


				const char *pszWeaponName = WeaponIdToAlias(iWeapon);
				char szWeaponName[32];
				Q_snprintf(szWeaponName, sizeof(szWeaponName), "#%s", pszWeaponName);
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