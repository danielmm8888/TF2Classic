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
	Show();

	Q_snprintf(m_sText, sizeof(m_sText), sText);

	if (m_pText)
	{
		m_pText->SetText(sText);
	}

	HFont pFont = m_pText->GetFont();	
	int iWidth = UTIL_ComputeStringWidth(pFont, sText);
	int iSpacing = UTIL_ComputeStringWidth(pFont, L" ");
	int iFontWide = iWidth + iSpacing * 2;

	int iWide = XRES(200);
	int iTall = YRES(25);
	int iOffsetX = XRES(10);
	int iOffsetY = (iFontWide / iWide) + 1;

	if (iFontWide > iWide)
	{
		m_pText->SetWrap(true);
		SetSize(iWide + iOffsetX * 2, iTall * iOffsetY);
		m_pText->SetPos(iOffsetX, 0);
		m_pText->SetSize(iWide, iTall * iOffsetY);
	}
	else 
	{
		m_pText->SetWrap(false);
		SetSize(iFontWide, iTall);
		m_pText->SetPos(0, 0);
		m_pText->SetSize(iFontWide, iTall);
	}
}

void CTFToolTipPanel::HideToolTip()
{
	Hide();
}

void CTFToolTipPanel::Show()
{
	BaseClass::Show();
	MakePopup();
}

void CTFToolTipPanel::Hide()
{
	BaseClass::Hide();
}

void CTFToolTipPanel::OnThink()
{
	BaseClass::OnThink();
	int cursorX, cursorY;
	surface()->SurfaceGetCursorPos(cursorX, cursorY);
	//SetPos(cursorX + toProportionalWide(8), cursorY + toProportionalTall(10));
	
	int iTipW, iTipH;
	GetSize(iTipW, iTipH);

	int wide, tall;
	surface()->GetScreenSize(wide, tall);

	if (wide - iTipW > cursorX)
	{
		cursorY += YRES(10);
		cursorX += XRES(8);

		// menu hanging right
		if (tall - iTipH > cursorY)
		{
			// menu hanging down
			SetPos(cursorX, cursorY);
		}
		else
		{
			// menu hanging up
			SetPos(cursorX, cursorY - iTipH - YRES(20));
		}
	}
	else
	{
		cursorY += YRES(10);
		cursorX += XRES(8);
		// menu hanging left
		if (tall - iTipH > cursorY)
		{
			// menu hanging down
			SetPos(cursorX - iTipW, cursorY);
		}
		else
		{
			// menu hanging up
			SetPos(cursorX - iTipW, cursorY - iTipH - YRES(20));
		}
	}
};