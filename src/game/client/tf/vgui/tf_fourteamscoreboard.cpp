//========= Copyright © 1996-2006, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"

#include <tier1/fmtstr.h>
#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Frame.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/SectionedListPanel.h>
#include <vgui_controls/ImageList.h>
#include <game/client/iviewport.h>
#include <KeyValues.h>
#include <filesystem.h>
#include "IGameUIFuncs.h" // for key bindings

#include "tf_controls.h"
#include "tf_shareddefs.h"
#include "tf_fourteamscoreboard.h"
#include "tf_gamestats_shared.h"
#include "tf_hud_statpanel.h"
#include "c_playerresource.h"
#include "c_tf_playerresource.h"
#include "c_tf_team.h"
#include "c_tf_player.h"
#include "vgui_avatarimage.h"
#include "tf_gamerules.h"

#if defined ( _X360 )
#include "engine/imatchmaking.h"
#endif

using namespace vgui;

#define SCOREBOARD_MAX_LIST_ENTRIES 12

extern bool IsInCommentaryMode( void );
extern const char *GetMapDisplayName( const char *mapName );

vgui::IImage* GetDefaultAvatarImage( C_BasePlayer *pPlayer );

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFFourTeamScoreBoardDialog::CTFFourTeamScoreBoardDialog(IViewPort *pViewPort) : CClientScoreBoardDialog(pViewPort)
{
	SetProportional(true);
	SetKeyBoardInputEnabled(false);
	SetMouseInputEnabled(false);
	SetScheme( "ClientScheme" );

	m_pPlayerListRed = new SectionedListPanel( this, "RedPlayerList" );
	m_pPlayerListBlue = new SectionedListPanel( this, "BluePlayerList" );
	m_pPlayerListGreen = new SectionedListPanel( this, "GreenPlayerList" );
	m_pPlayerListYellow = new SectionedListPanel( this, "YellowPlayerList" );
	m_pLabelPlayerName = new CExLabel(this, "PlayerNameLabel", "");
	m_pImagePanelHorizLine = new ImagePanel( this, "HorizontalLine" );
	m_pClassImage = new CTFClassImage( this, "ClassImage" );
	m_iImageDead = 0;
	m_iImageDominated = 0;
	m_iImageNemesis = 0;
	
	ListenForGameEvent( "server_spawn" );

	SetDialogVariable( "server", "" );

	SetVisible( false );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFFourTeamScoreBoardDialog::~CTFFourTeamScoreBoardDialog()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::PerformLayout()
{
	BaseClass::PerformLayout();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings("Resource/UI/FourTeamScoreBoard.res");

	if ( m_pImageList )
	{
		m_iImageDead = m_pImageList->AddImage( scheme()->GetImage( "../hud/leaderboard_dead", true ) );
		m_iImageDominated = m_pImageList->AddImage( scheme()->GetImage( "../hud/leaderboard_dominated", true ) );
		m_iImageNemesis = m_pImageList->AddImage( scheme()->GetImage( "../hud/leaderboard_nemesis", true ) );

		// We're skipping the mercenary, as he shouldn't have a visible class emblem during regular gameplay
		for (int i = TF_CLASS_SCOUT; i < TF_CLASS_MERCENARY; i++)
		{
			m_iClassEmblem[i] = m_pImageList->AddImage(scheme()->GetImage(g_aPlayerClassEmblems[i - 1], true));
			m_iClassEmblemDead[i] = m_pImageList->AddImage(scheme()->GetImage(g_aPlayerClassEmblemsDead[i - 1], true));
		}

		// resize the images to our resolution
		for (int i = 1; i < m_pImageList->GetImageCount(); i++ )
		{
			int wide = 13, tall = 13;
			m_pImageList->GetImage(i)->SetSize(scheme()->GetProportionalScaledValueEx( GetScheme(), wide ), scheme()->GetProportionalScaledValueEx( GetScheme(),tall ) );
		}
	}

	SetPlayerListImages( m_pPlayerListBlue );
	SetPlayerListImages( m_pPlayerListRed );
	SetPlayerListImages( m_pPlayerListGreen );
	SetPlayerListImages( m_pPlayerListYellow );


	SetBgColor( Color( 0, 0, 0, 0) );
	SetBorder( NULL );
	SetVisible( false );

	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::ShowPanel(bool bShow)
{
	// Catch the case where we call ShowPanel before ApplySchemeSettings, eg when
	// going from windowed <-> fullscreen
	if ( m_pImageList == NULL )
	{
		InvalidateLayout( true, true );
	}

	// Don't show in commentary mode
	if ( IsInCommentaryMode() )
	{
		bShow = false;
	}
	
	if ( IsVisible() == bShow )
	{
		return;
	}

	int iRenderGroup = gHUD.LookupRenderGroupIndexByName( "global" );

	if ( bShow )
	{		
		SetVisible(true);
		MoveToFront();

		gHUD.LockRenderGroup(iRenderGroup);

		// Clear the selected item, this forces the default to the local player
		SectionedListPanel *pList = GetSelectedPlayerList();
		if (pList)
		{
			pList->ClearSelection();
		}
	}
	else
	{
		SetVisible( false );

		gHUD.UnlockRenderGroup( iRenderGroup );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Resets the scoreboard panel
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::Reset()
{
	InitPlayerList( m_pPlayerListBlue );
	InitPlayerList( m_pPlayerListRed );
	InitPlayerList( m_pPlayerListGreen );
	InitPlayerList( m_pPlayerListYellow );
}

//-----------------------------------------------------------------------------
// Purpose: Inits the player list in a list panel
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::InitPlayerList(SectionedListPanel *pPlayerList)
{
	pPlayerList->SetVerticalScrollbar( false );
	pPlayerList->RemoveAll();
	pPlayerList->RemoveAllSections();
	pPlayerList->AddSection( 0, "Players", TFPlayerSortFunc );
	pPlayerList->SetSectionAlwaysVisible( 0, true );
	pPlayerList->SetSectionFgColor( 0, Color( 255, 255, 255, 255 ) );
	pPlayerList->SetBgColor( Color( 0, 0, 0, 0 ) );
	pPlayerList->SetBorder( NULL );

	// Avatars are always displayed at 32x32 regardless of resolution
	if ( ShowAvatars() )
	{
		pPlayerList->AddColumnToSection( 0, "avatar", "", SectionedListPanel::COLUMN_IMAGE/* | SectionedListPanel::COLUMN_CENTER*/, m_iAvatarWidth );
	}

	pPlayerList->AddColumnToSection(0, "name", "", 0, m_iNameWidth);
	//pPlayerList->AddColumnToSection(0, "status", "", SectionedListPanel::COLUMN_IMAGE , m_iStatusWidth);
	pPlayerList->AddColumnToSection(0, "nemesis", "", SectionedListPanel::COLUMN_IMAGE, m_iNemesisWidth);
	pPlayerList->AddColumnToSection(0, "class", "", SectionedListPanel::COLUMN_IMAGE, m_iClassWidth);
	pPlayerList->AddColumnToSection(0, "score", "", 0, m_iScoreWidth);
	pPlayerList->AddColumnToSection(0, "ping", "", 0, m_iPingWidth);



//	pPlayerList->AddColumnToSection( 0, "name", "#TF_Scoreboard_Name", 0, m_iNameWidth );
//	pPlayerList->AddColumnToSection( 0, "status", "", SectionedListPanel::COLUMN_IMAGE | SectionedListPanel::COLUMN_CENTER, m_iStatusWidth );
//	pPlayerList->AddColumnToSection( 0, "nemesis", "", SectionedListPanel::COLUMN_IMAGE, m_iNemesisWidth );
//	pPlayerList->AddColumnToSection( 0, "class", "", 0, m_iClassWidth );
//	pPlayerList->AddColumnToSection( 0, "score", "#TF_Scoreboard_Score", SectionedListPanel::COLUMN_RIGHT, m_iScoreWidth );
//	pPlayerList->AddColumnToSection( 0, "ping", "#TF_Scoreboard_Ping", SectionedListPanel::COLUMN_RIGHT, m_iPingWidth );
}

//-----------------------------------------------------------------------------
// Purpose: Builds the image list to use in the player list
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::SetPlayerListImages(vgui::SectionedListPanel *pPlayerList)
{
	pPlayerList->SetImageList( m_pImageList, false );
	pPlayerList->SetVisible( true );
}

//-----------------------------------------------------------------------------
// Purpose: Updates the dialog
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::Update()
{
	UpdateTeamInfo();
	UpdatePlayerList();
	UpdateSpectatorList();
	UpdatePlayerDetails();
	MoveToCenterOfScreen();

	// update every second
	m_fNextUpdateTime = gpGlobals->curtime + 1.0f; 
}

//-----------------------------------------------------------------------------
// Purpose: Updates information about teams
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::UpdateTeamInfo()
{
	// update the team sections in the scoreboard
	for (int teamIndex = TF_TEAM_RED; teamIndex <= TF_TEAM_YELLOW; teamIndex++)
	{
		wchar_t *teamName = NULL;
		C_Team *team = GetGlobalTeam( teamIndex );
		if ( team )
		{
			// choose dialog variables to set depending on team
			const char *pDialogVarTeamScore = NULL;
			const char *pDialogVarTeamPlayerCount = NULL;
			const char *pDialogVarTeamName = NULL;
			switch ( teamIndex ) 
			{
				case TF_TEAM_RED:
					pDialogVarTeamScore = "redteamscore";
					pDialogVarTeamPlayerCount = "redteamplayercount";
					pDialogVarTeamName = "redteamname";
					break;
				case TF_TEAM_BLUE:
					pDialogVarTeamScore = "blueteamscore";
					pDialogVarTeamPlayerCount = "blueteamplayercount";
					pDialogVarTeamName = "blueteamname";
					break;
				case TF_TEAM_GREEN:
					pDialogVarTeamScore = "greenteamscore";
					pDialogVarTeamPlayerCount = "greenteamplayercount";
					pDialogVarTeamName = "greenteamname";
					break;
				case TF_TEAM_YELLOW:
					pDialogVarTeamScore = "yellowteamscore";
					pDialogVarTeamPlayerCount = "yellowteamplayercount";
					pDialogVarTeamName = "yellowteamname";
					break;
				default:
					Assert( false );
					break;
			}

			// update # of players on each team
			wchar_t name[64];
			wchar_t string1[1024];
			wchar_t wNumPlayers[6];
			_snwprintf( wNumPlayers, ARRAYSIZE( wNumPlayers ), L"%i", team->Get_Number_Players() );
			if ( !teamName && team )
			{
				g_pVGuiLocalize->ConvertANSIToUnicode( team->Get_Name(), name, sizeof( name ) );
				teamName = name;
			}
			if ( team->Get_Number_Players() == 1 )
			{
				g_pVGuiLocalize->ConstructString( string1, sizeof(string1), g_pVGuiLocalize->Find( "#TF_ScoreBoard_Player" ), 1, wNumPlayers );
			}
			else
			{
				g_pVGuiLocalize->ConstructString( string1, sizeof(string1), g_pVGuiLocalize->Find( "#TF_ScoreBoard_Players" ), 1, wNumPlayers );
			}

			// set # of players for team in dialog
			SetDialogVariable( pDialogVarTeamPlayerCount, string1 );

			// set team score in dialog
			SetDialogVariable( pDialogVarTeamScore, team->Get_Score() );		

			// set team name
			SetDialogVariable( pDialogVarTeamName, team->Get_Name() );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the player list
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::UpdatePlayerList()
{
	int iSelectedPlayerIndex = GetLocalPlayerIndex();

	// Save off which player we had selected
	SectionedListPanel *pList = GetSelectedPlayerList();

	if ( pList )
	{
		int itemID = pList->GetSelectedItem();

		if ( itemID >= 0 )
		{
			KeyValues *pInfo = pList->GetItemData( itemID );
			if ( pInfo )
			{
				iSelectedPlayerIndex = pInfo->GetInt( "playerIndex" );
			}
		}
	}	

	m_pPlayerListRed->RemoveAll();
	m_pPlayerListBlue->RemoveAll();
	m_pPlayerListGreen->RemoveAll();
	m_pPlayerListYellow->RemoveAll();

	C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>( g_PR );
	if ( !tf_PR )
		return;
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return;

	int localteam = pLocalPlayer->GetTeamNumber();

	bool bMadeSelection = false;

	for( int playerIndex = 1 ; playerIndex <= MAX_PLAYERS; playerIndex++ )
	{
		if( g_PR->IsConnected( playerIndex ) )
		{
			SectionedListPanel *pPlayerList = NULL;
			switch ( g_PR->GetTeam( playerIndex ) )
			{
			case TF_TEAM_RED:
				pPlayerList = m_pPlayerListRed;
				break;
			case TF_TEAM_BLUE:
				pPlayerList = m_pPlayerListBlue;
				break;
			case TF_TEAM_GREEN:
				pPlayerList = m_pPlayerListGreen;
				break;
			case TF_TEAM_YELLOW:
				pPlayerList = m_pPlayerListYellow;
				break;
			}
			if ( null == pPlayerList )
				continue;			

			const char *szName = tf_PR->GetPlayerName( playerIndex );
			int score = tf_PR->GetTotalScore( playerIndex );

			KeyValues *pKeyValues = new KeyValues( "data" );

			pKeyValues->SetInt( "playerIndex", playerIndex );
			pKeyValues->SetString( "name", szName );
			pKeyValues->SetInt( "score", score );

			// can only see class information if we're on the same team
			if ( !AreEnemyTeams( g_PR->GetTeam( playerIndex ), localteam ) && !( localteam == TEAM_UNASSIGNED ) )
			{
				// class name
				if( g_PR->IsConnected( playerIndex ) )
				{
					int iClass = tf_PR->GetPlayerClass( playerIndex );
					if ( GetLocalPlayerIndex() == playerIndex && !tf_PR->IsAlive( playerIndex ) ) 
					{
						// If this is local player and he is dead, show desired class (which he will spawn as) rather than current class.
						C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
						int iDesiredClass = pPlayer->m_Shared.GetDesiredPlayerClassIndex();
						// use desired class unless it's random -- if random, his future class is not decided until moment of spawn
						if ( TF_CLASS_RANDOM != iDesiredClass )
						{
							iClass = iDesiredClass;
						}
					} 
					else 
					{
						// for non-local players, show the current class
						iClass = tf_PR->GetPlayerClass( playerIndex );
					}

					if( iClass >= TF_FIRST_NORMAL_CLASS && iClass < TF_CLASS_MERCENARY )
					{
						pKeyValues->SetInt("class", tf_PR->IsAlive(playerIndex) ? m_iClassEmblem[iClass] : m_iClassEmblemDead[iClass]);
					}
					else
					{
						pKeyValues->SetString( "class", "" );
					}
				}				
			}
			else
			{
				C_TFPlayer *pPlayerOther = ToTFPlayer( UTIL_PlayerByIndex( playerIndex ) );
				bool bUseTruncatedNames = false;
				if ( pPlayerOther && pPlayerOther->m_Shared.IsPlayerDominated( pLocalPlayer->entindex() ) )
				{
					// if local player is dominated by this player, show a nemesis icon
					pKeyValues->SetInt( "nemesis", m_iImageNemesis );
					pKeyValues->SetString( "class", bUseTruncatedNames ? "#TF_Nemesis_lodef" : "#TF_Nemesis" );
				}
				else if ( pLocalPlayer->m_Shared.IsPlayerDominated( playerIndex) )
				{
					// if this player is dominated by the local player, show the domination icon
					pKeyValues->SetInt( "nemesis", m_iImageDominated );
					pKeyValues->SetString( "class", bUseTruncatedNames ? "#TF_Dominated_lodef" : "#TF_Dominated" );
				}
			}

			// display whether player is alive or dead (all players see this for all other players on both teams)
			pKeyValues->SetInt( "status", tf_PR->IsAlive( playerIndex ) ?  0 : m_iImageDead );

			if ( g_PR->GetPing( playerIndex ) < 1 )
			{
				if ( g_PR->IsFakePlayer( playerIndex ) )
				{
					pKeyValues->SetString( "ping", "#TF_Scoreboard_Bot" );
				}
				else
				{
					pKeyValues->SetString( "ping", "" );
				}
			}
			else
			{
				pKeyValues->SetInt( "ping", g_PR->GetPing( playerIndex ) );
			}


			UpdatePlayerAvatar( playerIndex, pKeyValues );
			
			int itemID = pPlayerList->AddItem( 0, pKeyValues );
			Color clr = g_PR->GetTeamColor( g_PR->GetTeam( playerIndex ) );
			if ( !tf_PR->IsAlive( playerIndex ) )
				clr.SetColor( clr.r(), clr.g(), clr.b(), clr.a() / 2 );

			pPlayerList->SetItemFgColor( itemID, clr );

			if ( iSelectedPlayerIndex == playerIndex )
			{
				bMadeSelection = true;
				pPlayerList->SetSelectedItem( itemID );
			}

			pKeyValues->deleteThis();
		}
	}

	// If we're on spectator, find a default selection
	if ( !bMadeSelection )
	{
		if ( m_pPlayerListBlue->GetItemCount() > 0 )
		{
			m_pPlayerListBlue->SetSelectedItem( 0 );
		}
		else if ( m_pPlayerListRed->GetItemCount() > 0 )
		{
			m_pPlayerListRed->SetSelectedItem( 0 );
		}
		else if ( m_pPlayerListGreen->GetItemCount() > 0 )
		{
			m_pPlayerListGreen->SetSelectedItem( 0 );
		}
		else if ( m_pPlayerListYellow->GetItemCount() > 0 )
		{
			m_pPlayerListYellow->SetSelectedItem( 0 );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the spectator list
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::UpdateSpectatorList()
{
	char szSpectatorList[512] = "" ;
	int nSpectators = 0;
	for( int playerIndex = 1 ; playerIndex <= MAX_PLAYERS; playerIndex++ )
	{
		if ( ShouldShowAsSpectator( playerIndex ) )
		{
			if ( nSpectators > 0 )
			{
				Q_strncat( szSpectatorList, ", ", ARRAYSIZE( szSpectatorList ) );
			}

			Q_strncat( szSpectatorList, g_PR->GetPlayerName( playerIndex ), ARRAYSIZE( szSpectatorList ) );
			nSpectators++;
		}
	}

	wchar_t wzSpectators[512] = L"";
	if ( nSpectators > 0 )
	{
		const char *pchFormat = ( 1 == nSpectators ? "#ScoreBoard_Spectator" : "#ScoreBoard_Spectators" );

		wchar_t wzSpectatorCount[16];
		wchar_t wzSpectatorList[1024];
		_snwprintf( wzSpectatorCount, ARRAYSIZE( wzSpectatorCount ), L"%i", nSpectators );
		g_pVGuiLocalize->ConvertANSIToUnicode( szSpectatorList, wzSpectatorList, sizeof( wzSpectatorList ) );
		g_pVGuiLocalize->ConstructString( wzSpectators, sizeof(wzSpectators), g_pVGuiLocalize->Find( pchFormat), 2, wzSpectatorCount, wzSpectatorList );
	}
	SetDialogVariable( "spectators", wzSpectators );
}

//-----------------------------------------------------------------------------
// Purpose: Updates details about a player
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::UpdatePlayerDetails()
{
	ClearPlayerDetails();

	C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>( g_PR );
	if ( !tf_PR )
		return;
	
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return;
	int playerIndex = pLocalPlayer->entindex();

	// Make sure the selected player is still connected. 
	if ( !tf_PR->IsConnected( playerIndex ) ) 
		return;

	if ( engine->IsHLTV() )
	{
		SetDialogVariable( "playername", tf_PR->GetPlayerName( playerIndex ) );
		return;
	}

	RoundStats_t &roundStats = GetStatPanel()->GetRoundStatsCurrentGame();

	SetDialogVariable( "kills", tf_PR->GetPlayerScore( playerIndex ) );
	SetDialogVariable( "deaths", tf_PR->GetDeaths( playerIndex ) );
	SetDialogVariable( "assists", roundStats.m_iStat[TFSTAT_KILLASSISTS] );
	SetDialogVariable( "destruction", roundStats.m_iStat[TFSTAT_BUILDINGSDESTROYED] );
	SetDialogVariable( "captures", roundStats.m_iStat[TFSTAT_CAPTURES] );
	SetDialogVariable( "defenses", roundStats.m_iStat[TFSTAT_DEFENSES] );
	SetDialogVariable( "dominations", roundStats.m_iStat[TFSTAT_DOMINATIONS] );
	SetDialogVariable( "revenge", roundStats.m_iStat[TFSTAT_REVENGE] );
	SetDialogVariable( "healing", roundStats.m_iStat[TFSTAT_HEALING] );
	SetDialogVariable( "invulns", roundStats.m_iStat[TFSTAT_INVULNS] );
	SetDialogVariable( "teleports", roundStats.m_iStat[TFSTAT_TELEPORTS] );
	SetDialogVariable( "headshots", roundStats.m_iStat[TFSTAT_HEADSHOTS] );
	SetDialogVariable( "backstabs", roundStats.m_iStat[TFSTAT_BACKSTABS] );
	SetDialogVariable( "bonus", roundStats.m_iStat[TFSTAT_BONUS] );
	SetDialogVariable( "playername", tf_PR->GetPlayerName( playerIndex ) );
	SetDialogVariable( "playerscore", GetPointsString( tf_PR->GetTotalScore( playerIndex ) ) );
	Color clr = g_PR->GetTeamColor( g_PR->GetTeam( playerIndex ) );
	m_pLabelPlayerName->SetFgColor( clr );
	m_pImagePanelHorizLine->SetFillColor( clr );

	int iClass = pLocalPlayer->m_Shared.GetDesiredPlayerClassIndex();
	int iTeam = pLocalPlayer->GetTeamNumber();
	if ( ( iTeam >= FIRST_GAME_TEAM ) && ( iClass >= TF_FIRST_NORMAL_CLASS ) && ( iClass < TF_CLASS_COUNT_ALL ) )
	{
		m_pClassImage->SetClass( iTeam, iClass, 0 );
		m_pClassImage->SetVisible( true );
	} 
	else
	{
		m_pClassImage->SetVisible( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Clears score details
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::ClearPlayerDetails()
{
	m_pClassImage->SetVisible( false );

	// HLTV has no game stats
	bool bVisible = !engine->IsHLTV();

	SetDialogVariable( "kills", "" ); 
	SetControlVisible( "KillsLabel", bVisible );

	SetDialogVariable( "deaths", "" );
	SetControlVisible( "DeathsLabel", bVisible );

	SetDialogVariable( "captures", "" );
	SetControlVisible( "CapturesLabel", bVisible );

	SetDialogVariable( "defenses", "" );
	SetControlVisible( "DefensesLabel", bVisible );

	SetDialogVariable( "dominations", "" );
	SetControlVisible( "DominationLabel", bVisible );

	SetDialogVariable( "revenge", "" );
	SetControlVisible( "RevengeLabel", bVisible );

	SetDialogVariable( "assists", "" );
	SetControlVisible( "AssistsLabel", bVisible );

	SetDialogVariable( "destruction", "" );
	SetControlVisible( "DestructionLabel", bVisible );

	SetDialogVariable( "healing", "" );
	SetControlVisible( "HealingLabel", bVisible );

	SetDialogVariable( "invulns", "" );
	SetControlVisible( "InvulnLabel", bVisible );

	SetDialogVariable( "teleports", "" );
	SetControlVisible( "TeleportsLabel", bVisible );

	SetDialogVariable( "headshots", "" );
	SetControlVisible( "HeadshotsLabel", bVisible );

	SetDialogVariable( "backstabs", "" );
	SetControlVisible( "BackstabsLabel", bVisible );

	SetDialogVariable( "playername", "" );

	SetDialogVariable( "playerscore", "" );

	
}

//-----------------------------------------------------------------------------
// Purpose: Used for sorting players
//-----------------------------------------------------------------------------
bool CTFFourTeamScoreBoardDialog::TFPlayerSortFunc(vgui::SectionedListPanel *list, int itemID1, int itemID2)
{
	KeyValues *it1 = list->GetItemData(itemID1);
	KeyValues *it2 = list->GetItemData(itemID2);
	Assert(it1 && it2);

	// first compare score
	int v1 = it1->GetInt("score");
	int v2 = it2->GetInt("score");
	if (v1 > v2)
		return true;
	else if (v1 < v2)
		return false;

	// if score is the same, use player index to get deterministic sort
	int iPlayerIndex1 = it1->GetInt( "playerIndex" );
	int iPlayerIndex2 = it2->GetInt( "playerIndex" );
	return ( iPlayerIndex1 > iPlayerIndex2 );
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether the specified player index is a spectator
//-----------------------------------------------------------------------------
bool CTFFourTeamScoreBoardDialog::ShouldShowAsSpectator(int iPlayerIndex)
{
	C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>( g_PR );
	if ( !tf_PR )
		return false;

	// see if player is connected
	if ( tf_PR->IsConnected( iPlayerIndex ) ) 
	{
		// either spectating or unassigned team should show in spectator list
		int iTeam = tf_PR->GetTeam( iPlayerIndex );
		if ( TEAM_SPECTATOR == iTeam || TEAM_UNASSIGNED == iTeam )
			return true;
	}
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Event handler
//-----------------------------------------------------------------------------
void CTFFourTeamScoreBoardDialog::FireGameEvent(IGameEvent *event)
{
	const char *type = event->GetName();

	if ( 0 == Q_strcmp( type, "server_spawn" ) )
	{		
		// set server name in scoreboard
		const char *hostname = event->GetString( "hostname" );
		wchar_t wzHostName[256];
		wchar_t wzServerLabel[256];
		g_pVGuiLocalize->ConvertANSIToUnicode( hostname, wzHostName, sizeof( wzHostName ) );
		g_pVGuiLocalize->ConstructString( wzServerLabel, sizeof(wzServerLabel), g_pVGuiLocalize->Find( "#Scoreboard_Server" ), 1, wzHostName );
		SetDialogVariable( "server", wzServerLabel );
		// Set the level name after the server spawn
		char szMapName[MAX_MAP_NAME];
		Q_FileBase(engine->GetLevelName(), szMapName, sizeof(szMapName));
		Q_strlower(szMapName);
		SetDialogVariable("mapname", GetMapDisplayName(szMapName));
	}

	if( IsVisible() )
	{
		Update();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
SectionedListPanel *CTFFourTeamScoreBoardDialog::GetSelectedPlayerList(void)
{
	SectionedListPanel *pList = NULL;

	// navigation
	if ( m_pPlayerListBlue->GetSelectedItem() >= 0 )
	{
		pList = m_pPlayerListBlue;
	}
	else if ( m_pPlayerListRed->GetSelectedItem() >= 0 )
	{
		pList = m_pPlayerListRed;
	}
	else if ( m_pPlayerListGreen->GetSelectedItem() >= 0 )
	{
		pList = m_pPlayerListGreen;
	}
	else if ( m_pPlayerListYellow->GetSelectedItem() >= 0 )
	{
		pList = m_pPlayerListYellow;
	}

	return pList;
}
