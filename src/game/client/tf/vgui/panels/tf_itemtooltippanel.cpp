#include "cbase.h"
#include "tf_itemtooltippanel.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbuttonbase.h"
#include "controls/tf_advmodelpanel.h"
#include <vgui/ILocalize.h>

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFItemToolTipPanel::CTFItemToolTipPanel(vgui::Panel* parent, const char *panelName) : CTFToolTipPanel(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFItemToolTipPanel::~CTFItemToolTipPanel()
{

}

bool CTFItemToolTipPanel::Init(void)
{
	BaseClass::Init();

	m_pClassModelPanel = new CTFAdvModelPanel(this, "classmodelpanel");
	m_pTitle = new CExLabel(this, "TitleLabel", "Title");
	m_pClassName = new CExLabel(this, "ClassNameLabel", "ClassName");
	m_pAttributeText = new CExLabel(this, "AttributeLabel", "Attribute");
	for (int i = 0; i < 20; i++){
		m_pAttributes.AddToTail(new CExLabel(this, "AttributeLabel", "Attribute"));
	}

	return true;
}

void CTFItemToolTipPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/ItemToolTipPanel.res");
}

void CTFItemToolTipPanel::PerformLayout()
{
	BaseClass::PerformLayout();
	int xpos, ypos;
	int wide, tall;
	m_pAttributeText->GetPos(xpos, ypos);
	m_pAttributeText->GetSize(wide, tall);

	for (int i = 0; i < 20; i++)
	{
		m_pAttributes[i]->SetBounds(xpos, ypos + i * tall / 2, wide, tall);
		m_pAttributes[i]->SetFont(m_pAttributeText->GetFont());
		m_pAttributes[i]->SetContentAlignment(vgui::Label::Alignment::a_center);
	}
};


void CTFItemToolTipPanel::ShowToolTip(CEconItemDefinition *pItemData)
{
	Show();

	/*
	char pModel[64];
	Q_snprintf(pModel, sizeof(pModel), pItemData->model_world);
	if (!Q_strcmp(pModel, ""))
		Q_snprintf(pModel, sizeof(pModel), pItemData->model_player);
	m_pClassModelPanel->SetModelName(strdup(pModel), 0);
	if (Q_strcmp(pModel, ""))
	{
		m_pClassModelPanel->SetVisible(true);
		m_pClassModelPanel->Update();
	}
	*/

	if (m_pTitle)
	{
		m_pTitle->SetText(pItemData->item_name);
	}
	if (m_pClassName)
	{
		m_pClassName->SetText(pItemData->item_type_name);
	}
	for (int i = 0; i < 20; i++){
		m_pAttributes[i]->SetVisible(false);
	}
	int index = 0;
	if (m_pAttributeText)
	{
		for (int i = 0; i < pItemData->attributes.Count(); i++)
		{
			CEconItemAttribute *pAttribute = &pItemData->attributes[i];
			EconAttributeDefinition *pStatic = pAttribute->GetStaticData();
			if (pStatic)
			{
				float value = pAttribute->value;
			
				switch ( pStatic->description_format )
				{
				case ATTRIB_FORMAT_PERCENTAGE:
					value = value - 1.0f;
					value *= 100.0f;
					break;
				case ATTRIB_FORMAT_INVERTED_PERCENTAGE:
					value = 1.0f - value;
					value *= 100.0f;
					break;
				case ATTRIB_FORMAT_ADDITIVE_PERCENTAGE:
					value *= 100.0f;
					break;
				}

				wchar_t floatstr[32];
				_snwprintf(floatstr, ARRAYSIZE(floatstr) - 1, L"%.0f", value);

				wchar_t attrib[128];
				g_pVGuiLocalize->ConstructString(attrib, sizeof(attrib), g_pVGuiLocalize->Find(pStatic->description_string), 1, floatstr);

				if (attrib[0] == '\0' || pStatic->hidden)
					continue;
				
				char attributename[128];
				g_pVGuiLocalize->ConvertUnicodeToANSI(attrib, attributename, sizeof(attributename));
				Q_strncat(attributename, "\n", sizeof(attributename));

				m_pAttributes[index]->SetText(attributename);

				Color attrcolor;
				switch (pStatic->effect_type)
				{
				case ATTRIB_EFFECT_NEUTRAL: 
					attrcolor = GETSCHEME()->GetColor("ItemAttribNeutral", Color(255, 255, 255));
					break;
				case ATTRIB_EFFECT_POSITIVE:
					attrcolor = GETSCHEME()->GetColor("ItemAttribPositive", Color(255, 255, 255));
					break;
				case ATTRIB_EFFECT_NEGATIVE:
					attrcolor = GETSCHEME()->GetColor("ItemAttribNegative", Color(255, 255, 255));
					break;
				}
				m_pAttributes[index]->SetFgColor(attrcolor);
				m_pAttributes[index]->SetVisible(true);
				index++;
			}
		}
	}
	int xpos, ypos;
	m_pAttributeText->GetPos(xpos, ypos);
	SetSize(GetWide(), YRES(10) + ypos + index * YRES(10));
}

void CTFItemToolTipPanel::HideToolTip()
{
	Hide();
}

void CTFItemToolTipPanel::Show()
{
	BaseClass::Show();
	MakePopup();
}

void CTFItemToolTipPanel::Hide()
{
	BaseClass::Hide();
}