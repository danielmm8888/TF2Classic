//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "tf_optionsmousepanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_scriptobject.h"
#include "controls/tf_cvartogglecheckbutton.h"
#include "controls/tf_cvarslider.h"
#include "controls/tf_advpanellistpanel.h"
#include "controls/tf_advbutton.h"
#include "vgui_controls/ComboBox.h"
#include <KeyValues.h>
#include <vgui/IScheme.h>
#include "tier1/convar.h"
#include <stdio.h>
#include <vgui_controls/TextEntry.h>
// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

CTFOptionsMousePanel::CTFOptionsMousePanel(vgui::Panel *parent, const char *panelName) : CTFDialogPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFOptionsMousePanel::~CTFOptionsMousePanel()
{
}

bool CTFOptionsMousePanel::Init()
{
	BaseClass::Init();

	m_pListPanel = new CPanelListPanel(this, "PanelListPanel");
	return true;
}

//-----------------------------------------------------------------------------
// Purpose: sets background color & border
//-----------------------------------------------------------------------------
void CTFOptionsMousePanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/OptionsMouse.res");
}

void CTFOptionsMousePanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

void CTFOptionsMousePanel::CreateControls()
{
	BaseClass::CreateControls();

	pTitleMouse = new CTFAdvButton(this, "DescTextTitle", "Mouse options");

	m_pReverseMouseCheckBox = new CCvarToggleCheckButton(this, "ReverseMouse", "#GameUI_ReverseMouse", "m_pitch");
	m_pRawInputCheckBox = new CCvarToggleCheckButton(this, "RawInput", "#GameUI_MouseRaw", "m_rawinput");
	m_pMouseFilterCheckBox = new CCvarToggleCheckButton(this, "MouseFilter", "#GameUI_MouseFilter", "m_filter");
	m_pMouseSensitivitySlider = new CCvarSlider(this, "MouseSensitivity", "#GameUI_MouseSensitivity", 1.0f, 20.0f, "sensitivity", true);

	m_pMouseAccelCheckBox = new CCvarToggleCheckButton(this, "MouseAccel", "#GameUI_MouseAcceleration_Hint", "m_customaccel");
	m_pMouseAccelSlider = new CCvarSlider(this, "MouseAccelSlider", "#GameUI_MouseAcceleration", 1.0f, 2.0f, "m_customaccel_exponent", true);

	///
	pTitleJoystick = new CTFAdvButton(this, "DescTextTitle", "Joystick options");

	m_pJoystickCheckBox = new CCvarToggleCheckButton(this, "Joystick", "#GameUI_Joystick", "joystick");
	m_pJoystickSouthpawCheckBox = new CCvarToggleCheckButton(this, "JoystickSouthpaw", "#GameUI_JoystickSouthpaw", "joy_movement_stick");
	m_pReverseJoystickCheckBox = new CCvarToggleCheckButton(this, "ReverseJoystick", "#GameUI_ReverseJoystick", "joy_inverty");

	m_pJoyYawSensitivitySlider = new CCvarSlider(this, "JoystickYawSlider", "#GameUI_JoystickLookSpeedYaw", -0.5f, -7.0f, "joy_yawsensitivity", true);
	m_pJoyPitchSensitivitySlider = new CCvarSlider(this, "JoystickPitchSlider", "#GameUI_JoystickLookSpeedPitch", 0.5f, 7.0f, "joy_pitchsensitivity", true);

	AddControl(pTitleMouse, O_CATEGORY);
	AddControl(m_pReverseMouseCheckBox, O_BOOL);
	AddControl(m_pRawInputCheckBox, O_BOOL);
	AddControl(m_pMouseFilterCheckBox, O_BOOL);
	AddControl(m_pMouseSensitivitySlider, O_SLIDER);
	AddControl(m_pMouseAccelCheckBox, O_BOOL);
	AddControl(m_pMouseAccelSlider, O_SLIDER);

	AddControl(pTitleJoystick, O_CATEGORY);
	AddControl(m_pJoystickCheckBox, O_BOOL);
	AddControl(m_pJoystickSouthpawCheckBox, O_BOOL);
	AddControl(m_pReverseJoystickCheckBox, O_BOOL);
	AddControl(m_pJoyYawSensitivitySlider, O_SLIDER);
	AddControl(m_pJoyPitchSensitivitySlider, O_SLIDER);

	UpdatePanels();
}

void CTFOptionsMousePanel::DestroyControls()
{
	BaseClass::DestroyControls();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsMousePanel::OnResetData()
{
	BaseClass::OnResetData();

	m_pReverseMouseCheckBox->Reset();
	m_pRawInputCheckBox->Reset();
	m_pMouseFilterCheckBox->Reset();
	m_pMouseSensitivitySlider->Reset();
	m_pMouseAccelCheckBox->Reset();
	m_pMouseAccelSlider->Reset();

	m_pJoystickCheckBox->Reset();
	m_pJoystickSouthpawCheckBox->Reset();
	m_pReverseJoystickCheckBox->Reset();
	m_pJoyYawSensitivitySlider->Reset();
	m_pJoyPitchSensitivitySlider->Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsMousePanel::OnApplyChanges()
{
	BaseClass::OnApplyChanges();

	m_pReverseMouseCheckBox->ApplyChanges();
	m_pRawInputCheckBox->ApplyChanges();
	m_pMouseFilterCheckBox->ApplyChanges();
	m_pMouseSensitivitySlider->ApplyChanges();
	m_pMouseAccelCheckBox->ApplyChanges();
	m_pMouseAccelSlider->ApplyChanges();

	m_pJoystickCheckBox->ApplyChanges();
	m_pJoystickSouthpawCheckBox->ApplyChanges();
	m_pReverseJoystickCheckBox->ApplyChanges();
	m_pJoyYawSensitivitySlider->ApplyChanges();
	m_pJoyPitchSensitivitySlider->ApplyChanges();

	engine->ClientCmd_Unrestricted("joyadvancedupdate");
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsMousePanel::OnControlModified(Panel *panel)
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
	//if (panel == m_pJoystickCheckBox || panel == m_pMouseAccelCheckBox)
	UpdatePanels();
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsMousePanel::UpdatePanels()
{
	bool bEnabled = m_pMouseAccelCheckBox->IsSelected();
	m_pMouseAccelSlider->SetEnabled(bEnabled);


	bEnabled = m_pJoystickCheckBox->IsSelected();
	m_pReverseJoystickCheckBox->SetEnabled(bEnabled);
	m_pJoystickSouthpawCheckBox->SetEnabled(bEnabled);
	m_pJoyYawSensitivitySlider->SetEnabled(bEnabled);
	m_pJoyPitchSensitivitySlider->SetEnabled(bEnabled);
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsMousePanel::OnTextChanged(Panel *panel)
{

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsMousePanel::UpdateSensitivityLabel()
{

}
