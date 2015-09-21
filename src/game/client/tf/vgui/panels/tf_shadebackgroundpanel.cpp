#include "cbase.h"
#include "tf_shadebackgroundpanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFShadeBackgroundPanel::CTFShadeBackgroundPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{

}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFShadeBackgroundPanel::~CTFShadeBackgroundPanel()
{

}


void CTFShadeBackgroundPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	LoadControlSettings("resource/UI/main_menu/ShadeBackgroundPanel.res");
	Show();
}

void CTFShadeBackgroundPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};


void CTFShadeBackgroundPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

void CTFShadeBackgroundPanel::Show()
{
	BaseClass::Show();
	ImagePanel *m_bShadedBG = dynamic_cast<ImagePanel *>(FindChildByName("ShadedBG"));
	vgui::GetAnimationController()->RunAnimationCommand(m_bShadedBG, "Alpha", 220, 0.0f, 0.3f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};

void CTFShadeBackgroundPanel::Hide()
{
	BaseClass::Hide();
	ImagePanel *m_bShadedBG = dynamic_cast<ImagePanel *>(FindChildByName("ShadedBG"));
	vgui::GetAnimationController()->RunAnimationCommand(m_bShadedBG, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};
