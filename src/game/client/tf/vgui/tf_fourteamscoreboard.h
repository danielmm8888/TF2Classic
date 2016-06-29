//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_FOURTEAMSCOREBOARD_H
#define TF_FOURTEAMSCOREBOARD_H
#ifdef _WIN32
#pragma once
#endif

#include "hud.h"
#include "hudelement.h"
#include "tf_hud_playerstatus.h"
#include "clientscoreboarddialog.h"
#include "tf_clientscoreboard.h"

//-----------------------------------------------------------------------------
// Purpose: displays the MapInfo menu
//-----------------------------------------------------------------------------

class CTFFourTeamScoreBoardDialog : public CClientScoreBoardDialog
{
private:
	DECLARE_CLASS_SIMPLE(CTFFourTeamScoreBoardDialog, CClientScoreBoardDialog);

public:
	CTFFourTeamScoreBoardDialog(IViewPort *pViewPort);
	virtual ~CTFFourTeamScoreBoardDialog();

	virtual const char *GetName(void) { return PANEL_FOURTEAMSCOREBOARD; }
	virtual void Reset();
	virtual void Update();
	virtual void ShowPanel( bool bShow );

protected:
	virtual void PerformLayout();
	virtual void ApplySchemeSettings( vgui::IScheme *pScheme );

	virtual void PostApplySchemeSettings( vgui::IScheme *pScheme ) {};

private:
	void InitPlayerList( vgui::SectionedListPanel *pPlayerList );
	void SetPlayerListImages( vgui::SectionedListPanel *pPlayerList );
	void UpdateTeamInfo();
	void UpdatePlayerList();
	void UpdateSpectatorList();
	void UpdatePlayerDetails();
	void ClearPlayerDetails();
	bool ShouldShowAsSpectator( int iPlayerIndex );
	
	virtual void FireGameEvent( IGameEvent *event );

	static bool TFPlayerSortFunc( vgui::SectionedListPanel *list, int itemID1, int itemID2 );

	vgui::SectionedListPanel *GetSelectedPlayerList( void );

	vgui::SectionedListPanel	*m_pPlayerListRed;
	vgui::SectionedListPanel	*m_pPlayerListBlue;
	vgui::SectionedListPanel	*m_pPlayerListGreen;
	vgui::SectionedListPanel	*m_pPlayerListYellow;
	CExLabel					*m_pLabelPlayerName;
	vgui::ImagePanel			*m_pImagePanelHorizLine;
	CTFClassImage				*m_pClassImage;

	int							m_iImageDead;
	int							m_iImageDominated;
	int							m_iImageNemesis;
	int							m_iClassEmblem[TF_CLASS_COUNT_ALL];
	int							m_iClassEmblemDead[TF_CLASS_COUNT_ALL];
	
	CPanelAnimationVarAliasType( int, m_iStatusWidth, "status_width", "12", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iNemesisWidth, "nemesis_width", "20", "proportional_int" );
};

const wchar_t *GetPointsString( int iPoints );

#endif // TF_FOURTEAMSCOREBOARD_H
