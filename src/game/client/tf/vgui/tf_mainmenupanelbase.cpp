#include "cbase.h"
#include "tf_mainmenupanelbase.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuPanelBase::CTFMainMenuPanelBase(vgui::Panel* parent, const char *panelName) : EditablePanel(NULL, panelName)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(false);
	SetVisible(true);
	Init();
	
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuPanelBase::~CTFMainMenuPanelBase()
{

}

bool CTFMainMenuPanelBase::Init()
{
	int width, height;
	surface()->GetScreenSize(width, height);
	SetSize(width, height);
	SetPos(0, 0);
	bInGameLayout = false;
	return true;
}

void CTFMainMenuPanelBase::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMainMenuPanelBase::PerformLayout()
{
	BaseClass::PerformLayout();
};

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

void CTFMainMenuPanelBase::Show()
{
	SetVisible(true);
};

void CTFMainMenuPanelBase::Hide()
{
	SetVisible(false);
};

void CTFMainMenuPanelBase::DefaultLayout()
{
	bInGameLayout = false;
	if (bInMenu || bInGame)
		SetVisible(bInMenu);
};

void CTFMainMenuPanelBase::GameLayout()
{
	bInGameLayout = true;
	if (bInMenu || bInGame)
		SetVisible(bInGame);
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
	return dynamic_cast<CTFMainMenu*>(m_pMainMenu)->InGame();
}