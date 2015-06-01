#ifndef TFMAINMENUPANEL_H
#define TFMAINMENUPANEL_H

#include "tf_mainmenupanelbase.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFMainMenuPanel : public CTFMainMenuPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFMainMenuPanel, CTFMainMenuPanelBase);

public:
	CTFMainMenuPanel(vgui::Panel* parent, const char *panelName);
	virtual ~CTFMainMenuPanel();
	bool Init();
	void PerformLayout();
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void OnThink();
	void OnTick();
	void OnCommand(const char* command);
	void DefaultLayout();
	void GameLayout();
	void SetVersionLabel();
	void PlayMusic();

private:
	CExLabel			*m_pVersionLabel;
	char				m_pzMusicLink[64];
	float				m_flActionThink;
	float				m_flMusicThink;
	float				m_flAnimationThink;
	bool				m_bAnimationIn;
	bool				m_bMusicPlay;
	char*				GetRandomMusic();
};


//-----------------------------------------------------------------------------
// Purpose:  Draws the rotated arrow panels
//-----------------------------------------------------------------------------
class CTFRotationPanel : public CTFImagePanel
{
public:
	DECLARE_CLASS_SIMPLE(CTFRotationPanel, CTFImagePanel);

	CTFRotationPanel(vgui::Panel *parent, const char *name);
	virtual void Paint();
	virtual void ApplySettings(KeyValues *inResourceData);
	float GetAngleRotation(void);

private:
	float				flRetVal;
	char				pImage[64];
	CMaterialReference	m_Material;
};


#endif // TFMAINMENUPANEL_H