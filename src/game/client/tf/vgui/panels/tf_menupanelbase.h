#ifndef TFMAINMENUPANELBASE_H
#define TFMAINMENUPANELBASE_H

#include "GameUI/IGameUI.h"
#include <vgui/ISurface.h>
#include "vgui_controls/Panel.h"
#include "tf_controls.h"
#include "tf_vgui_video.h"
#include <filesystem.h>
#include <vgui_controls/AnimationController.h>

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMenuPanelBase : public vgui::EditablePanel, public CAutoGameSystem
{
	DECLARE_CLASS_SIMPLE(CTFMenuPanelBase, vgui::EditablePanel);

public:
	CTFMenuPanelBase(vgui::Panel* parent, const char *panelName);
	virtual ~CTFMenuPanelBase();
	virtual bool Init();
	virtual void PerformLayout();
	virtual void ApplySchemeSettings(vgui::IScheme *pScheme);
	virtual void PaintBackground();
	virtual bool InGame();
	virtual void OnCommand(const char* command);
	virtual void OnThink();
	virtual void OnTick();
	virtual void SetShowSingle(bool ShowSingle);
	virtual void Show();
	virtual void Hide();
	virtual void DefaultLayout();
	virtual void GameLayout();
	static CTFMenuPanelBase* GetMenuPanel(int iPanel);

protected:
	bool				bInGame;
	bool				bInMenu;
	bool				bInGameLayout;
	bool				bFirstTime;
	bool				bShowSingle;
};

#endif // TFMAINMENUPANELBASE_H