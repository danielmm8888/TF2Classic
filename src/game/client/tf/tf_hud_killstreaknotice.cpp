#include "cbase.h"
#include "iclientmode.h"
#include "hud.h"
#include "hudelement.h"
#include <vgui/IScheme.h>
#include <vgui_controls/EditablePanel.h>
#include "vgui/tf_controls.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

#if 0
class CTFKillstreakNoticePanel : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE(CTFKillstreakNoticePanel, EditablePanel);

public:
	CTFKillstreakNoticePanel(const char *pElementName);

	virtual void ApplySchemeSettings(IScheme *pScheme);

private:
	CExLabel	*m_pSplashLabel;
};

DECLARE_HUDELEMENT(CTFKillstreakNoticePanel);

CTFKillstreakNoticePanel::CTFKillstreakNoticePanel(const char *pElementName) : CHudElement(pElementName), BaseClass(NULL, "Background")
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent(pParent);

	m_pSplashLabel = NULL;
}

void CTFKillstreakNoticePanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/HudKillStreakNotice.res");

	m_pSplashLabel = dynamic_cast<CExLabel *>(FindChildByName("SplashLabel"));
}
#endif