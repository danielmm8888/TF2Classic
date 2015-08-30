#include "cbase.h"
#include "tf_menupanelbase.h"
#include "tf_mainmenu.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMenuPanelBase::CTFMenuPanelBase(vgui::Panel* parent, const char *panelName) : EditablePanel(NULL, panelName)
{
	SetParent(parent);
	SetScheme("ClientScheme");
	SetProportional(true);
	SetVisible(true);
	Init();
	
	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMenuPanelBase::~CTFMenuPanelBase()
{

}

bool CTFMenuPanelBase::Init()
{
	int x, y;
	int width, height;
	//surface()->GetScreenSize(width, height);
	GetParent()->GetBounds(x, y, width, height);
	SetSize(width, height);
	SetPos(0, 0);
	bInGame = false;
	bInMenu = false;
	bInGameLayout = false;
	bShowSingle = false;
	return true;
}

void CTFMenuPanelBase::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
}

void CTFMenuPanelBase::PerformLayout()
{
	BaseClass::PerformLayout();
};

void CTFMenuPanelBase::OnCommand(const char* command)
{
	engine->ExecuteClientCmd(command);
}

void CTFMenuPanelBase::OnTick()
{
	BaseClass::OnTick();
};

void CTFMenuPanelBase::OnThink()
{
	BaseClass::OnThink();
};

void CTFMenuPanelBase::SetShowSingle(bool ShowSingle)
{
	bShowSingle = ShowSingle;
}

void CTFMenuPanelBase::Show()
{
	SetVisible(true);
};

void CTFMenuPanelBase::Hide()
{
	SetVisible(false);
};

void CTFMenuPanelBase::DefaultLayout()
{
	bInGameLayout = false;
	if (bInMenu || bInGame)
		(bInMenu ? Show() : Hide());
	//SetVisible(bInMenu);
};


void CTFMenuPanelBase::GameLayout()
{
	bInGameLayout = true;
	if (bInMenu || bInGame)
		(bInGame ? Show() : Hide());
	//SetVisible(bInGame);
};

void CTFMenuPanelBase::PaintBackground()
{
	SetPaintBackgroundType(0);
	BaseClass::PaintBackground();
}

bool CTFMenuPanelBase::InGame()
{
	return MAINMENU_ROOT->InGame();
}


CTFMenuPanelBase* CTFMenuPanelBase::GetMenuPanel(int iPanel)
{
	return MAINMENU_ROOT->GetMenuPanel(iPanel);
}