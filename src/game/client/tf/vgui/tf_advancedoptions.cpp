//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "tf_advancedoptions.h"
#include <vgui/IVGui.h>
#include "vgui/IScheme.h"
#include <vgui_controls/Frame.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/TextEntry.h>
#include <vgui_controls/CheckButton.h>
#include "tf_controls.h"
#include "iconvar.h"
#include "ienginevgui.h"

using namespace vgui;

ConVar cl_showmypanel("tf2c_showadvpanel", "0", FCVAR_CLIENTDLL, "Sets the state of the panel <state>");
extern ConVar hud_fastswitch;
extern ConVar tf2c_model_muzzleflash;
extern ConVar tf2c_ammobucket;

//CAdvSettingsPanel class: Tutorial example class
class CAdvSettingsPanel : public vgui::Frame
{
	DECLARE_CLASS_SIMPLE(CAdvSettingsPanel, vgui::Frame);

	CAdvSettingsPanel(vgui::VPANEL* parent);    // Constructor
	~CAdvSettingsPanel(){};            // Destructor
	//stb new input
	virtual void OnTick();
	virtual void OnCommand(const char *command);
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);

private:
	//Other used VGUI control Elements:
	vgui::CheckButton* m_pMuzzleCheck;
	vgui::CheckButton* m_pBucketCheck;
	vgui::CheckButton* m_pFastCheck;
};

// Constuctor: Initializes the Panel
CAdvSettingsPanel::CAdvSettingsPanel(vgui::VPANEL* parent) : BaseClass(NULL, "AdvSettingsPanel")
{
	SetParent(*parent);

	SetKeyBoardInputEnabled(true);
	SetMouseInputEnabled(true);

	SetProportional(false);
	SetTitleBarVisible(true);
	SetMinimizeButtonVisible(false);
	SetMaximizeButtonVisible(false);
	SetCloseButtonVisible(false);
	SetSizeable(false);
	SetMoveable(true);
	SetVisible(false);

	vgui::HScheme scheme = vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme");
	LoadControlSettings("resource/UI/AdditionalOptions.res");

	vgui::ivgui()->AddTickSignal(GetVPanel(), 100);

	SetScheme(scheme);
}

void CAdvSettingsPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);
};


//Class: CAdvSettingsPanelInterface Class. Used for construction.
class CAdvSettingsPanelInterface : public IAdvPanel
{
private:
	CAdvSettingsPanel *MyPanel;
public:
	CAdvSettingsPanelInterface()
	{
		MyPanel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		MyPanel = new CAdvSettingsPanel(&parent);
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
static CAdvSettingsPanelInterface g_AdvPanel;
IAdvPanel* advpanel = (IAdvPanel*)&g_AdvPanel;

CON_COMMAND(tf2c_advsettings, "Toggles AdvSettings panel on or off")
{
	//cl_showmypanel.SetValue(!cl_showmypanel.GetBool());
	if (!g_AdvPanel.IsVisible()){
		g_AdvPanel.Activate();
	}
	else
	{
		g_AdvPanel.Close();
	}
};

void CAdvSettingsPanel::OnTick()
{
	BaseClass::OnTick();

	m_pMuzzleCheck = FindControl<CheckButton>("MuzzleCheckButton", true);
	m_pMuzzleCheck->SetSelected(tf2c_model_muzzleflash.GetBool());
	m_pBucketCheck = FindControl<CheckButton>("BucketCheckButton", true);
	m_pBucketCheck->SetSelected(tf2c_ammobucket.GetBool());
	m_pFastCheck = FindControl<CheckButton>("FastCheckButton", true);
	m_pFastCheck->SetSelected(hud_fastswitch.GetBool());

	if (cl_showmypanel.GetBool() == 1){
		Activate();
		cl_showmypanel.SetValue(0);
	}
}

void CAdvSettingsPanel::OnCommand(const char *command)
{
	BaseClass::OnCommand(command);
	if (!Q_stricmp(command, "turnoff"))
	{
		cl_showmypanel.SetValue(0);
		Close();
	}
	if (!Q_stricmp(command, "checkmuzzle"))
	{
		tf2c_model_muzzleflash.SetValue(!tf2c_model_muzzleflash.GetBool());
	}
	if (!Q_stricmp(command, "checkbucket"))
	{
		tf2c_ammobucket.SetValue(!tf2c_ammobucket.GetBool());
	}
	if (!Q_stricmp(command, "checkfast"))
	{
		hud_fastswitch.SetValue(!hud_fastswitch.GetBool());
	}
};