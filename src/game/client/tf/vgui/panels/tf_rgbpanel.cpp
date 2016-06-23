#include "cbase.h"
#include "tf_rgbpanel.h"
#include "controls/tf_cvarcombobox.h"
#include "controls/tf_cvarslider.h"
#include <vgui/ILocalize.h>
#include "controls/tf_advmodelpanel.h"
#include "c_tf_player.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFRGBPanel::CTFRGBPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFRGBPanel::~CTFRGBPanel()
{

}

bool CTFRGBPanel::Init()
{
	BaseClass::Init();

	m_pRedScrollBar = NULL;
	m_pGrnScrollBar = NULL;
	m_pBluScrollBar = NULL;
	m_pColorBG = NULL;
	m_pCombo = NULL;
	m_pParticlesPanel = new CTFAdvModelPanel(this, "particlesmodelpanel");

	return true;
}

void CTFRGBPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/RGBPanel.res");

	m_pRedScrollBar = dynamic_cast<CCvarSlider *>(FindChildByName("RedScrollBar"));
	m_pGrnScrollBar = dynamic_cast<CCvarSlider *>(FindChildByName("GrnScrollBar"));
	m_pBluScrollBar = dynamic_cast<CCvarSlider *>(FindChildByName("BluScrollBar"));
	m_pColorBG = dynamic_cast<ImagePanel *>(FindChildByName("ColorBG"));
	m_pCombo = dynamic_cast<CCvarComboBox*>(FindChildByName("ParticleComboBox"));

	for (int i = 0; i < 100; i++)
	{
		char pszParticleName[64];
		if (i < 9)
			Q_snprintf(pszParticleName, sizeof(pszParticleName), "#TF_DM_PARTICLE0%d", i + 1);
		else
			Q_snprintf(pszParticleName, sizeof(pszParticleName), "#TF_DM_PARTICLE%d", i + 1);
		wchar_t *pText = g_pVGuiLocalize->Find(pszParticleName);
		if (pText != NULL)
		{
			char pszParticleNameLocal[64];
			wcstombs(pszParticleNameLocal, pText, sizeof(pszParticleNameLocal));
			m_pCombo->AddItem(pszParticleNameLocal, NULL);
		}
	}

	if ( tf2c_setmerccolor_r.GetFloat() == 0.0f && tf2c_setmerccolor_g.GetFloat() == 0.0f && tf2c_setmerccolor_b.GetFloat() == 0.0f )
	{
		// 0 0 0 is default value which disables the proxy. So that means this is the first game launch.
		// Let's generate a random color...
		tf2c_setmerccolor_r.SetValue( RandomInt( 0, 255 ) );
		tf2c_setmerccolor_g.SetValue( RandomInt( 0, 255 ) );
		tf2c_setmerccolor_b.SetValue( RandomInt( 0, 255 ) );
		InvalidateLayout( true, true );
	}
}

void CTFRGBPanel::PerformLayout()
{
	BaseClass::PerformLayout();
	OnDataChanged();
};

void CTFRGBPanel::OnDataChanged()
{
	Color clr(m_pRedScrollBar->GetValue(), m_pGrnScrollBar->GetValue(), m_pBluScrollBar->GetValue(), 255);
	m_pColorBG->SetFillColor(clr);
	PostActionSignal(new KeyValues("ControlModified"));

	m_pParticlesPanel->Update();
	char chParticleName[64];
	ConVar *tf2c_setmercparticle = cvar->FindVar("tf2c_setmercparticle");
	if (!tf2c_setmercparticle)
		return;
	int iParticleID = tf2c_setmercparticle->GetInt();
	if ( !iParticleID )
		return;
	Q_snprintf(chParticleName, sizeof(chParticleName), "dm_respawn_%02d", iParticleID);
	m_pParticlesPanel->SetParticleName(chParticleName);
};

void CTFRGBPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		SetVisible(false);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

