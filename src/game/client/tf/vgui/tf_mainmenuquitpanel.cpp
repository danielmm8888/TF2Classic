#include "cbase.h"
#include "tf_mainmenuquitpanel.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuQuitPanel::CTFMainMenuQuitPanel(vgui::Panel* parent) : CTFMainMenuPanelBase(parent)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);	
	SetMainMenu(GetParent());

	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);
	LoadControlSettings("resource/UI/main_menu/QuitMenu.res");
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuQuitPanel::~CTFMainMenuQuitPanel()
{

}

void CTFMainMenuQuitPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		Hide();
	}
	else if (!Q_strcmp(command, "quitconfirm"))
	{
		engine->ClientCmd("quit");
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}


void CTFMainMenuQuitPanel::Show()
{
	ImagePanel *m_bShadedBG = dynamic_cast<ImagePanel *>(FindChildByName("ShadedBG"));
	vgui::GetAnimationController()->RunAnimationCommand(m_bShadedBG, "Alpha", 220, 0.0f, 0.3f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	BaseClass::Show();
};

void CTFMainMenuQuitPanel::Hide()
{
	ImagePanel *m_bShadedBG = dynamic_cast<ImagePanel *>(FindChildByName("ShadedBG"));
	vgui::GetAnimationController()->RunAnimationCommand(m_bShadedBG, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
	BaseClass::Hide();
};


void CTFMainMenuQuitPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMainMenuQuitPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};
