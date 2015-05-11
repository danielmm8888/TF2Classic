#include "cbase.h"
#include "tf_mainmenupanelbase.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuPanelBase::CTFMainMenuPanelBase(vgui::Panel* parent) : EditablePanel(NULL, "MainMenuPanel")
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);
	SetVisible(true);

	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

void CTFMainMenuPanelBase::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMainMenuPanelBase::PerformLayout()
{
	BaseClass::PerformLayout();
};

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuPanelBase::~CTFMainMenuPanelBase()
{

}

void CTFMainMenuPanelBase::OnCommand(const char* command)
{
	engine->ExecuteClientCmd(command);
}

void CTFMainMenuPanelBase::OnTick()
{
	BaseClass::OnTick();
};

void CTFMainMenuPanelBase::OnThink()
{
	BaseClass::OnThink();
};

void CTFMainMenuPanelBase::DefaultLayout()
{

};

void CTFMainMenuPanelBase::GameLayout()
{

};

void CTFMainMenuPanelBase::SetMainMenu(Panel *m_pPanel)
{
	m_pMainMenu = dynamic_cast<CTFMainMenu*>(m_pPanel);
};

Panel* CTFMainMenuPanelBase::GetMainMenu()
{
	return dynamic_cast<CTFMainMenu*>(m_pMainMenu);
};

void CTFMainMenuPanelBase::PaintBackground()
{
	SetPaintBackgroundType(0);
	BaseClass::PaintBackground();
}

bool CTFMainMenuPanelBase::InGame()
{
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if (pPlayer && IsVisible())
	{
		return true;
	}
	else {
		return false;
	}
}