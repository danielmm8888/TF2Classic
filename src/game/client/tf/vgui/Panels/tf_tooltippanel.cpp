#include "cbase.h"
#include "tf_tooltippanel.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbuttonbase.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFToolTipPanel::CTFToolTipPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFToolTipPanel::~CTFToolTipPanel()
{

}

bool CTFToolTipPanel::Init(void)
{
	BaseClass::Init();

	m_pText = NULL;

	return true;
}

void CTFToolTipPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/ToolTipPanel.res");

	m_pText = dynamic_cast<CExLabel *>(FindChildByName("TextLabel"));
}

void CTFToolTipPanel::PerformLayout()
{
	BaseClass::PerformLayout();
};



void CTFToolTipPanel::ShowToolTip(char *sText)
{
	Q_snprintf(m_sText, sizeof(m_sText), sText);
	if (m_pText)
	{
		m_pText->SetText(sText);
	}

	HFont pFont = m_pText->GetFont();	
	int iWidth = UTIL_ComputeStringWidth(pFont, sText);
	int iSpacing = UTIL_ComputeStringWidth(pFont, L" ");

	SetWide(iWidth + iSpacing * 2);
	m_pText->SetWide(iWidth + iSpacing * 2);

	Show();
}

void CTFToolTipPanel::HideToolTip()
{
	Hide();
}

void CTFToolTipPanel::Show()
{
	BaseClass::Show();
}

void CTFToolTipPanel::Hide()
{
	BaseClass::Hide();
}

void CTFToolTipPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "vguicancel"))
	{
		Hide();
	}
	else if (!stricmp(command, "Ok"))
	{
		Hide();
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFToolTipPanel::OnTick()
{
	BaseClass::OnTick();
};

void CTFToolTipPanel::OnThink()
{
	BaseClass::OnThink();
	int x, y;
	surface()->SurfaceGetCursorPos(x, y);
	SetPos(x + toProportionalWide(8), y + toProportionalTall(10));
};

void CTFToolTipPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
};

void CTFToolTipPanel::GameLayout()
{
	BaseClass::GameLayout();
};