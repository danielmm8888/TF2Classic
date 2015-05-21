#include "cbase.h"
#include "tf_mainmenurgbpanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuRGBPanel::CTFMainMenuRGBPanel(vgui::Panel* parent) : CTFMainMenuPanelBase(parent)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);	

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuRGBPanel::~CTFMainMenuRGBPanel()
{

}

void CTFMainMenuRGBPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		SetVisible(false);
	}
	if (!Q_strcmp(command, "scrolled"))
	{
		//Msg("Got the values: %f %f %f\n", m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue());
		Color clr(m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue(), 255);
		m_pColorBG->SetFillColor(clr);
		char szCommand[MAX_PATH];
		Q_snprintf(szCommand, sizeof(szCommand), "tf2c_setmerccolor %f %f %f", m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue());
		GetParent()->OnCommand(szCommand);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}


void CTFMainMenuRGBPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/RGBMenu.res");
	SetMainMenu(GetParent());
	EditablePanel *m_pPanel = dynamic_cast<EditablePanel *>(FindChildByName("RGBMenu"));
	int xpos, ypos, width, height;
	m_pPanel->GetBounds(xpos, ypos, width, height);
	SetSize(width, height);
	SetPos(xpos, ypos);

	m_pRedScrollBar = dynamic_cast<CTFMainMenuScrollBar *>(FindChildByName("RedScrollBar"));
	m_pGrnScrollBar = dynamic_cast<CTFMainMenuScrollBar *>(FindChildByName("GrnScrollBar"));
	m_pBluScrollBar = dynamic_cast<CTFMainMenuScrollBar *>(FindChildByName("BluScrollBar"));
	m_pRedScrollBar->SetMinMax(0, 255);
	m_pGrnScrollBar->SetMinMax(0, 255);
	m_pBluScrollBar->SetMinMax(0, 255);
	m_pColorBG = dynamic_cast<ImagePanel *>(FindChildByName("ColorBG"));
}

void CTFMainMenuRGBPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};
