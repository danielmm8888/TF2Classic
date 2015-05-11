#ifndef TFMAINMENUPANELBASE_H
#define TFMAINMENUPANELBASE_H

#include "GameUI/IGameUI.h"
#include "vgui_controls/Frame.h"
#include "tf_mainmenubutton.h"
#include "tf_controls.h"

#include <vgui/ISurface.h>
#include "tf_vgui_video.h"

#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuPanelBase : public vgui::EditablePanel, public CAutoGameSystem
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuPanelBase, vgui::EditablePanel);

public:
	CTFMainMenuPanelBase(vgui::Panel* parent);
	virtual ~CTFMainMenuPanelBase();

	void PerformLayout();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PaintBackground();
	virtual bool InGame();
	virtual void OnCommand(const char* command);
	virtual void OnThink();
	virtual void OnTick();
	virtual void SetMainMenu(Panel *m_pPanel);
	virtual Panel* GetMainMenu();
	virtual void DefaultLayout();
	virtual void GameLayout();

protected:
	bool				InGameLayout;
	Panel				*m_pMainMenu;
};

#endif // TFMAINMENUPANELBASE_H