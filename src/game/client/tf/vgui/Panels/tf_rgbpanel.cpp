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
	SetProportional(false);	
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFRGBPanel::~CTFRGBPanel()
{

}



void CTFRGBPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/RGBPanel.res");
	SetMainMenu(GetParent());

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
		//Msg("Got the values: %i %i %i\n", m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue());
		Color clr(m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue(), 255);
		m_pColorBG->SetFillColor(clr);
		char szCommand[MAX_PATH];
		Q_snprintf(szCommand, sizeof(szCommand), "tf2c_setmerccolor %i %i %i", m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue());
		engine->ExecuteClientCmd(szCommand);
		//GetParent()->OnCommand(szCommand);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

