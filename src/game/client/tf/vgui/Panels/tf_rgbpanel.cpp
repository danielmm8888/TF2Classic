#include "cbase.h"
#include "tf_rgbpanel.h"
#include "controls/tf_cvarcombobox.h"
#include "controls/tf_cvarslider.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFRGBPanel::CTFRGBPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFRGBPanel::~CTFRGBPanel()
{

}

bool CTFRGBPanel::Init()
{
	BaseClass::Init();

	m_pRedScrollBar = NULL;
	m_pGrnScrollBar = NULL;
	m_pBluScrollBar = NULL;
	m_pColorBG = NULL;
	m_pCombo = NULL;

	return true;
}

void CTFRGBPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/RGBPanel.res");

	m_pRedScrollBar = dynamic_cast<CCvarSlider *>(FindChildByName("RedScrollBar"));
	m_pGrnScrollBar = dynamic_cast<CCvarSlider *>(FindChildByName("GrnScrollBar"));
	m_pBluScrollBar = dynamic_cast<CCvarSlider *>(FindChildByName("BluScrollBar"));
	m_pColorBG = dynamic_cast<ImagePanel *>(FindChildByName("ColorBG"));
	m_pCombo = dynamic_cast<CCvarComboBox*>(FindChildByName("ParticleComboBox"));

	for (int i = 0; i < 31; i++)
	{
		char pszParticleName[64];
		Q_snprintf(pszParticleName, sizeof(pszParticleName), "ParticleNum%d", i + 1);
		m_pCombo->AddItem(pszParticleName, NULL);
	}

}

void CTFRGBPanel::PerformLayout()
{
	BaseClass::PerformLayout();
	OnDataChanged();
};

void CTFRGBPanel::OnDataChanged()
{
	Color clr(m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue(), 255);
	m_pColorBG->SetFillColor(clr);
};

void CTFRGBPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		SetVisible(false);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

