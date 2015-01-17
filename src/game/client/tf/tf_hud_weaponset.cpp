//The following include files are necessary to allow your MyPanel.cpp to compile.
#include "cbase.h"
#include "tf_hud_weaponset.h"
using namespace vgui;
#include <vgui/IVGui.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>
#include "tf_controls.h"
#include "iconvar.h"
#include "ienginevgui.h"

//CMyPanel class: Tutorial example class
class CMyPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CMyPanel, vgui::Frame);
	//CMyPanel : This Class / vgui::Frame : BaseClass

	CMyPanel(vgui::VPANEL parent);    // Constructor
	~CMyPanel(){};            // Destructor
	//stb new input
	virtual void OnTick();
	virtual void OnCommand(const char *command);
	//stb new input end

private:
	//Other used VGUI control Elements:

};
// Constuctor: Initializes the Panel
CMyPanel::CMyPanel(vgui::VPANEL parent)
	: BaseClass(NULL, "MyPanel")
{
	SetParent(parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(true);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(true);
	SetSizeable(true);
	SetMoveable(true);
	SetVisible(false);
	SetSize(300,200);
	SetPos(ScreenWidth()/2, 500);
	SetTitle("Weapon Select Menu", 1);
	
	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));

	LoadControlSettings("resource/UI/HudCloakMeter.res");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);


	CExLabel*m_pCloakLabel = dynamic_cast<CExLabel *>(FindChildByName("CloakLabel"));
	m_pCloakLabel->SetSize(0, 0);

	DevMsg("MyPanel has been constructed\n");
}
//Class: CMyPanelInterface Class. Used for construction.
class CMyPanelInterface : public IMyPanel
{
private:
	CMyPanel *MyPanel;
public:
	CMyPanelInterface()
	{
		MyPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		MyPanel = new CMyPanel(parent);
	}
	void Destroy()
	{
		if (MyPanel)
		{
			MyPanel->SetParent((vgui::Panel *)NULL);
			delete MyPanel;
		}
	}
	void Activate()
	{
		MyPanel->Activate();
	}
	void Close()
	{
		MyPanel->Close();
	}
	bool IsVisible()
	{
		return MyPanel->IsVisible();
	}
};
static CMyPanelInterface g_MyPanel;
IMyPanel* mypanel = (IMyPanel*)&g_MyPanel;


//ConVar cl_showmypanel("cl_showmypanel", "0", FCVAR_CLIENTDLL, "Sets the state of myPanel <state>");


CON_COMMAND(ToggleMyPanel, "Toggles myPanel on or off")
{
	//cl_showmypanel.SetValue(!cl_showmypanel.GetBool());
	if (!g_MyPanel.IsVisible()){
		g_MyPanel.Activate();
	}
	else
	{
		g_MyPanel.Close();
	}
	//g_MyPanel.Activate();
};

void CMyPanel::OnTick()
{
	BaseClass::OnTick();
	//if (cl_showmypanel.GetBool() == 1){
	//	Activate();
	//	cl_showmypanel.SetValue(0);
//	}

	//SetVisible(cl_showmypanel.GetBool()); //CL_SHOWMYPANEL / 1 BY DEFAULT
}

void CMyPanel::OnCommand(const char *command)
{
//	BaseClass::OnCommand(command);
//	if (!Q_stricmp(command, "turnoff"))
		//if (cl_showmypanel.GetBool() == 1)
};


//from here you add the inventory code 