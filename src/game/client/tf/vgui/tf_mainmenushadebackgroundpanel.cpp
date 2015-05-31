#include "cbase.h"
#include "tf_mainmenushadebackgroundpanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuShadeBackgroundPanel::CTFMainMenuShadeBackgroundPanel(vgui::Panel* parent, const char *panelName) : CTFMainMenuPanelBase(parent, panelName)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);	
	SetMainMenu(GetParent());
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuShadeBackgroundPanel::~CTFMainMenuShadeBackgroundPanel()
{

}


void CTFMainMenuShadeBackgroundPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
	LoadControlSettings("resource/UI/main_menu/ShadeBackgroundMenu.res");
}

void CTFMainMenuShadeBackgroundPanel::PerformLayout()
{
	BaseClass::PerformLayout();
	Show();
};


void CTFMainMenuShadeBackgroundPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

void CTFMainMenuShadeBackgroundPanel::Show()
{
	BaseClass::Show();
	ImagePanel *m_bShadedBG = dynamic_cast<ImagePanel *>(FindChildByName("ShadedBG"));
	vgui::GetAnimationController()->RunAnimationCommand(m_bShadedBG, "Alpha", 220, 0.0f, 0.3f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};

void CTFMainMenuShadeBackgroundPanel::Hide()
{
	BaseClass::Hide();
	ImagePanel *m_bShadedBG = dynamic_cast<ImagePanel *>(FindChildByName("ShadedBG"));
	vgui::GetAnimationController()->RunAnimationCommand(m_bShadedBG, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};
