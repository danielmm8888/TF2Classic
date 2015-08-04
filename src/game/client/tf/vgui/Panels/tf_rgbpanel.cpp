#include "cbase.h"
#include "tf_rgbpanel.h"
#include "controls/tf_advslider.h"
#include "controls/tf_advslider.h"

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

	return true;
}

void CTFRGBPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/RGBPanel.res");

	m_pRedScrollBar = dynamic_cast<CTFAdvSlider *>(FindChildByName("RedScrollBar"));
	m_pGrnScrollBar = dynamic_cast<CTFAdvSlider *>(FindChildByName("GrnScrollBar"));
	m_pBluScrollBar = dynamic_cast<CTFAdvSlider *>(FindChildByName("BluScrollBar"));
	m_pRedScrollBar->SetMinMax(0.0, 255.0);
	m_pGrnScrollBar->SetMinMax(0.0, 255.0);
	m_pBluScrollBar->SetMinMax(0.0, 255.0);
	m_pRedScrollBar->SetValue(0.0);
	m_pGrnScrollBar->SetValue(0.0);
	m_pBluScrollBar->SetValue(0.0);
	m_pColorBG = dynamic_cast<ImagePanel *>(FindChildByName("ColorBG"));
}

void CTFRGBPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};

void CTFRGBPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		SetVisible(false);
	}
	if (!Q_strcmp(command, "scrolled"))
	{
		Color clr(m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue(), 255);
		m_pColorBG->SetFillColor(clr);
		char szCommand[MAX_PATH];
		Q_snprintf(szCommand, sizeof(szCommand), "tf2c_setmerccolor %i %i %i", (int)m_pRedScrollBar->GetValue(), (int)m_pGrnScrollBar->GetValue(), (int)m_pBluScrollBar->GetValue());
		engine->ExecuteClientCmd(szCommand);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

