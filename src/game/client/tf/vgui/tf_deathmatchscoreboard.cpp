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
#include <vgui_controls/Menu.h>
#include <vgui_controls/MenuItem.h>
#include <game/client/iviewport.h>
#include <KeyValues.h>
#include <filesystem.h>
#include "IGameUIFuncs.h" // for key bindings

#include "tf_controls.h"
#include "tf_shareddefs.h"
#include "tf_deathmatchscoreboard.h"
#include "tf_gamestats_shared.h"
#include "tf_hud_statpanel.h"
#include "c_playerresource.h"
#include "c_tf_playerresource.h"
#include "c_tf_team.h"
#include "c_tf_player.h"
#include "vgui_avatarimage.h"
#include "tf_gamerules.h"
#include "inputsystem/iinputsystem.h"
#include "basemodelpanel.h"
#include "engine/IEngineSound.h"
#include "in_buttons.h"
#include "voice_status.h"
#include "tf_music_manager.h"

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
CTFDeathMatchScoreBoardDialog::CTFDeathMatchScoreBoardDialog( IViewPort *pViewPort ) : CClientScoreBoardDialog( pViewPort )
{
	SetProportional( true );
	SetKeyBoardInputEnabled( false );
	SetMouseInputEnabled( false );
	MakePopup( true );
	SetScheme( "ClientScheme" );

	m_pPlayerListRed = new SectionedListPanel( this, "RedPlayerList" );
	m_pRedScoreBG = new ImagePanel( this, "RedScoreBG" );
	m_iImageDead = 0;
	m_iImageDominated = 0;
	m_iImageNemesis = 0;

	bLockInput = false;
	m_pWinPanel = new EditablePanel( this, "WinPanel" );
	m_flTimeUpdateTeamScore = 0;
	iSelectedPlayerIndex = 0;

	ListenForGameEvent( "server_spawn" );
	ListenForGameEvent( "teamplay_win_panel" );
	ListenForGameEvent( "teamplay_round_start" );
	ListenForGameEvent( "teamplay_game_over" );
	ListenForGameEvent( "tf_game_over" );

	SetDialogVariable( "server", "" );

	m_pContextMenu = new Menu( this, "contextmenu" );
	m_pContextMenu->AddMenuItem( "Mute", new KeyValues( "Command", "command", "mute" ), this );
	m_pContextMenu->AddMenuItem( "Vote kick...", new KeyValues( "Command", "command", "kick" ), this );
	m_pContextMenu->AddMenuItem( "Show Steam profile", new KeyValues( "Command", "command", "showprofile" ), this );

	SetVisible( false );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFDeathMatchScoreBoardDialog::~CTFDeathMatchScoreBoardDialog()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::PerformLayout()
{
	BaseClass::PerformLayout();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	LoadControlSettings( "Resource/UI/DeathMatchScoreBoard.res" );

	if ( m_pImageList )
	{
		m_iImageDead = m_pImageList->AddImage( scheme()->GetImage( "../hud/leaderboard_dead", true ) );
		m_iImageDominated = m_pImageList->AddImage( scheme()->GetImage( "../hud/leaderboard_dominated", true ) );
		m_iImageNemesis = m_pImageList->AddImage( scheme()->GetImage( "../hud/leaderboard_nemesis", true ) );

		// We're skipping the mercenary, as he shouldn't have a visible class emblem during regular gameplay
		for ( int i = TF_CLASS_SCOUT; i < TF_CLASS_MERCENARY; i++ )
		{
			m_iClassEmblem[i] = m_pImageList->AddImage( scheme()->GetImage( g_aPlayerClassEmblems[i - 1], true ) );
			m_iClassEmblemDead[i] = m_pImageList->AddImage( scheme()->GetImage( g_aPlayerClassEmblemsDead[i - 1], true ) );
		}

		// resize the images to our resolution
		for ( int i = 1; i < m_pImageList->GetImageCount(); i++ )
		{
			int wide = 13, tall = 13;
			m_pImageList->GetImage( i )->SetSize( scheme()->GetProportionalScaledValueEx( GetScheme(), wide ), scheme()->GetProportionalScaledValueEx( GetScheme(), tall ) );
		}
	}

	if ( m_pWinPanel )
	{
		m_pWinPanel->SetVisible( false );
	}

	SetPlayerListImages( m_pPlayerListRed );
	m_pPlayerListRed->SetVerticalScrollbar( true );
	iDefaultTall = m_pPlayerListRed->GetTall();

	SetVisible( false );

	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::ShowContextMenu( KeyValues* data )
{
	Panel *pItem = (Panel*)data->GetPtr( "SubPanel" );
	int iItem = data->GetInt( "itemID" );
	if ( pItem )
	{
		KeyValues *pData = m_pPlayerListRed->GetItemData( iItem );
		iSelectedPlayerIndex = pData->GetInt( "playerIndex", 0 );

		bool bMuted = GetClientVoiceMgr()->IsPlayerBlocked( iSelectedPlayerIndex );
		vgui::MenuItem *pMenuMute = m_pContextMenu->GetMenuItem( 0 );
		pMenuMute->SetText( !bMuted ? "Mute" : "Unmute" );
		if ( !( g_PR->GetPing( iSelectedPlayerIndex ) < 1 && g_PR->IsFakePlayer( iSelectedPlayerIndex ) ) )
		{
			Menu::PlaceContextMenu( this, m_pContextMenu );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::OnCommand( const char* command )
{
	if ( !Q_strcmp( command, "mute" ) )
	{
		bool bMuted = GetClientVoiceMgr()->IsPlayerBlocked( iSelectedPlayerIndex );
		GetClientVoiceMgr()->SetPlayerBlockedState( iSelectedPlayerIndex, !bMuted );
	}
	else if ( !Q_strcmp( command, "kick" ) )
	{
		//add proper votekicking after callvotes support
		engine->ExecuteClientCmd( "callvote" );
	}
	else if ( !Q_strcmp( command, "showprofile" ) )
	{
		C_BasePlayer *pPlayerOther = UTIL_PlayerByIndex( iSelectedPlayerIndex );
		if ( pPlayerOther )
		{
			CSteamID pPlayerSteamID;
			pPlayerOther->GetSteamID( &pPlayerSteamID );
			steamapicontext->SteamFriends()->ActivateGameOverlayToUser( "steamid", pPlayerSteamID );
		}
	}
	else
	{
		BaseClass::OnCommand( command );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::ShowPanel( bool bShow )
{
	// Catch the case where we call ShowPanel before ApplySchemeSettings, eg when
	// going from windowed <-> fullscreen
	if ( m_pImageList == NULL )
	{
		InvalidateLayout( true, true );
	}

	if ( !bShow && bLockInput )
	{
		return;
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
		SetVisible( true );
		SetKeyBoardInputEnabled( false );
		gHUD.LockRenderGroup( iRenderGroup );

		// Clear the selected item, this forces the default to the local player
		SectionedListPanel *pList = GetSelectedPlayerList();
		if ( pList )
		{
			pList->ClearSelection();
		}
	}
	else
	{
		SetVisible( false );
		m_pContextMenu->SetVisible( false );
		SetMouseInputEnabled( false );
		SetKeyBoardInputEnabled( false );
		bLockInput = false;
		gHUD.UnlockRenderGroup( iRenderGroup );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Resets the scoreboard panel
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::Reset()
{
	InitPlayerList( m_pPlayerListRed );
}

//-----------------------------------------------------------------------------
// Purpose: Inits the player list in a list panel
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::InitPlayerList( SectionedListPanel *pPlayerList )
{
	//pPlayerList->SetVerticalScrollbar( true );
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

	pPlayerList->AddColumnToSection( 0, "name", "#TF_Scoreboard_Name", 0, m_iNameWidth );
	pPlayerList->AddColumnToSection( 0, "status", "", SectionedListPanel::COLUMN_IMAGE, m_iStatusWidth );
	pPlayerList->AddColumnToSection( 0, "nemesis", "", SectionedListPanel::COLUMN_IMAGE, m_iNemesisWidth );
	pPlayerList->AddColumnToSection( 0, "kills", "#TF_ScoreBoard_KillsLabel", 0, m_iKillsWidth );
	pPlayerList->AddColumnToSection( 0, "deaths", "#TF_ScoreBoard_DeathsLabel", 0, m_iDeathsWidth );
	pPlayerList->AddColumnToSection( 0, "streak", "#TF_ScoreBoard_KillStreak", 0, m_iKillstreakWidth );
	pPlayerList->AddColumnToSection( 0, "score", "#TF_Scoreboard_Score", 0, m_iScoreWidth );
	pPlayerList->AddColumnToSection( 0, "ping", "#TF_Scoreboard_Ping", 0, m_iPingWidth );
}

//-----------------------------------------------------------------------------
// Purpose: Builds the image list to use in the player list
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::SetPlayerListImages( vgui::SectionedListPanel *pPlayerList )
{
	pPlayerList->SetImageList( m_pImageList, false );
	pPlayerList->SetVisible( true );
}

//-----------------------------------------------------------------------------
// Purpose: Updates the dialog
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::Update()
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
void CTFDeathMatchScoreBoardDialog::UpdateTeamInfo()
{
	// update the team sections in the scoreboard
	int teamIndex = TF_TEAM_RED;
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
			g_pVGuiLocalize->ConstructString( string1, sizeof( string1 ), g_pVGuiLocalize->Find( "#TF_ScoreBoard_Player" ), 1, wNumPlayers );
		}
		else
		{
			g_pVGuiLocalize->ConstructString( string1, sizeof( string1 ), g_pVGuiLocalize->Find( "#TF_ScoreBoard_Players" ), 1, wNumPlayers );
		}

		// set # of players for team in dialog
		SetDialogVariable( pDialogVarTeamPlayerCount, string1 );

		// set team score in dialog
		SetDialogVariable( pDialogVarTeamScore, team->Get_Score() );

		// set team name
		SetDialogVariable( pDialogVarTeamName, team->Get_Name() );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the player list
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::UpdatePlayerList()
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

	C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>( g_PR );
	if ( !tf_PR )
		return;
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return;

	int localteam = pLocalPlayer->GetTeamNumber();

	bool bMadeSelection = false;

	for ( int playerIndex = 1; playerIndex <= MAX_PLAYERS; playerIndex++ )
	{
		if ( g_PR->IsConnected( playerIndex ) )
		{
			SectionedListPanel *pPlayerList = NULL;
			switch ( g_PR->GetTeam( playerIndex ) )
			{
			case TF_TEAM_RED:
				pPlayerList = m_pPlayerListRed;
				break;
			}
			if ( null == pPlayerList )
				continue;

			const char *szName = tf_PR->GetPlayerName( playerIndex );
			int score = tf_PR->GetTotalScore( playerIndex );
			int kills = tf_PR->GetPlayerScore( playerIndex );
			int deaths = tf_PR->GetDeaths( playerIndex );
			int streak = tf_PR->GetKillstreak( playerIndex );

			KeyValues *pKeyValues = new KeyValues( "data" );

			pKeyValues->SetInt( "playerIndex", playerIndex );
			pKeyValues->SetString( "name", szName );
			pKeyValues->SetInt( "score", score );
			pKeyValues->SetInt( "kills", kills );
			pKeyValues->SetInt( "deaths", deaths );
			pKeyValues->SetInt( "streak", streak );

			// can only see class information if we're on the same team
			if ( !AreEnemyTeams( g_PR->GetTeam( playerIndex ), localteam ) && !( localteam == TEAM_UNASSIGNED ) )
			{
				// class name
				if ( g_PR->IsConnected( playerIndex ) )
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
				}
			}
			else
			{
				C_TFPlayer *pPlayerOther = ToTFPlayer( UTIL_PlayerByIndex( playerIndex ) );

				if ( pPlayerOther && pPlayerOther->m_Shared.IsPlayerDominated( pLocalPlayer->entindex() ) )
				{
					// if local player is dominated by this player, show a nemesis icon
					pKeyValues->SetInt( "nemesis", m_iImageNemesis );
				}
				else if ( pLocalPlayer->m_Shared.IsPlayerDominated( playerIndex ) )
				{
					// if this player is dominated by the local player, show the domination icon
					pKeyValues->SetInt( "nemesis", m_iImageDominated );
				}
			}

			// display whether player is alive or dead (all players see this for all other players on both teams)
			pKeyValues->SetInt( "status", tf_PR->IsAlive( playerIndex ) ? 0 : m_iImageDead );

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

			Color clr = tf_PR->GetPlayerColor( playerIndex );
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
		if ( m_pPlayerListRed->GetItemCount() > 0 )
		{
			m_pPlayerListRed->SetSelectedItem( 0 );
		}
	}

	ResizeScoreboard();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::ResizeScoreboard()
{
	int _wide, _tall;
	int wide, tall;
	int x, y;
	surface()->GetScreenSize( _wide, _tall );
	m_pPlayerListRed->GetContentSize( wide, tall );
	m_pPlayerListRed->GetPos( x, y );
	int yshift = y + scheme()->GetProportionalScaledValue( 10 );
	if ( tall > iDefaultTall  && tall + yshift < _tall )
	{
		m_pPlayerListRed->SetSize( wide, tall );
		tall += yshift;
		wide = GetWide();
		SetSize( wide, tall );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the spectator list
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::UpdateSpectatorList()
{
	char szSpectatorList[512] = "";
	int nSpectators = 0;
	for ( int playerIndex = 1; playerIndex <= MAX_PLAYERS; playerIndex++ )
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
		g_pVGuiLocalize->ConstructString( wzSpectators, sizeof( wzSpectators ), g_pVGuiLocalize->Find( pchFormat ), 2, wzSpectatorCount, wzSpectatorList );
	}
	SetDialogVariable( "spectators", wzSpectators );
}

//-----------------------------------------------------------------------------
// Purpose: Updates details about a player
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::UpdatePlayerDetails()
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
	SetDialogVariable( "dominations", roundStats.m_iStat[TFSTAT_DOMINATIONS] );
	SetDialogVariable( "revenge", roundStats.m_iStat[TFSTAT_REVENGE] );

	SetDialogVariable( "playername", tf_PR->GetPlayerName( playerIndex ) );
	SetDialogVariable( "playerscore", GetPointsString( tf_PR->GetTotalScore( playerIndex ) ) );

	Color clr = tf_PR->GetPlayerColor( playerIndex );
	m_pRedScoreBG->SetFillColor( clr );
}

//-----------------------------------------------------------------------------
// Purpose: Clears score details
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::ClearPlayerDetails()
{
	// HLTV has no game stats
	bool bVisible = !engine->IsHLTV();

	SetDialogVariable( "kills", "" );
	SetControlVisible( "KillsLabel", bVisible );

	SetDialogVariable( "deaths", "" );
	SetControlVisible( "DeathsLabel", bVisible );

	SetDialogVariable( "dominations", "" );
	SetControlVisible( "DominationLabel", bVisible );

	SetDialogVariable( "revenge", "" );
	SetControlVisible( "RevengeLabel", bVisible );

	SetDialogVariable( "assists", "" );
	SetControlVisible( "AssistsLabel", bVisible );

	SetDialogVariable( "playername", "" );

	SetDialogVariable( "playerscore", "" );


}

//-----------------------------------------------------------------------------
// Purpose: Used for sorting players
//-----------------------------------------------------------------------------
bool CTFDeathMatchScoreBoardDialog::TFPlayerSortFunc( vgui::SectionedListPanel *list, int itemID1, int itemID2 )
{
	KeyValues *it1 = list->GetItemData( itemID1 );
	KeyValues *it2 = list->GetItemData( itemID2 );
	Assert( it1 && it2 );

	// first compare score
	int v1 = it1->GetInt( "score" );
	int v2 = it2->GetInt( "score" );
	if ( v1 > v2 )
		return true;
	else if ( v1 < v2 )
		return false;

	// if score is the same, use player index to get deterministic sort
	int iPlayerIndex1 = it1->GetInt( "playerIndex" );
	int iPlayerIndex2 = it2->GetInt( "playerIndex" );
	return ( iPlayerIndex1 > iPlayerIndex2 );
}

//-----------------------------------------------------------------------------
// Purpose: Returns whether the specified player index is a spectator
//-----------------------------------------------------------------------------
bool CTFDeathMatchScoreBoardDialog::ShouldShowAsSpectator( int iPlayerIndex )
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
void CTFDeathMatchScoreBoardDialog::FireGameEvent( IGameEvent *event )
{
	const char *type = event->GetName();

	if ( 0 == Q_strcmp( type, "server_spawn" ) )
	{
		// set server name in scoreboard
		const char *hostname = event->GetString( "hostname" );
		wchar_t wzHostName[256];
		wchar_t wzServerLabel[256];
		g_pVGuiLocalize->ConvertANSIToUnicode( hostname, wzHostName, sizeof( wzHostName ) );
		g_pVGuiLocalize->ConstructString( wzServerLabel, sizeof( wzServerLabel ), g_pVGuiLocalize->Find( "#Scoreboard_Server" ), 1, wzHostName );
		SetDialogVariable( "server", wzServerLabel );
		// Set the level name after the server spawn
		char szMapName[MAX_MAP_NAME];
		Q_FileBase( engine->GetLevelName(), szMapName, sizeof( szMapName ) );
		Q_strlower( szMapName );
		SetDialogVariable( "mapname", GetMapDisplayName( szMapName ) );

		m_pWinPanel->SetVisible( false );
		bLockInput = false;
		ShowPanel( false );
	}
	else if ( Q_strcmp( "teamplay_win_panel", type ) == 0 )
	{
		if ( !TFGameRules() || !TFGameRules()->IsDeathmatch() )
			return;

		m_fNextUpdateTime = gpGlobals->curtime + 0.1;
		m_flTimeUpdateTeamScore = gpGlobals->curtime + 4.5f;
		bLockInput = true;
		bool bPlayerFirst = false;

		C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>( g_PR );
		if ( !tf_PR )
			return;

		// look for the top 3 players sent in the event
		for ( int i = 1; i <= 3; i++ )
		{
			bool bShow = false;
			char szPlayerIndexVal[64] = "", szPlayerScoreVal[64] = "", szPlayerKillsVal[64] = "", szPlayerDeathsVal[64] = "";
			// get player index and round points from the event
			Q_snprintf( szPlayerIndexVal, ARRAYSIZE( szPlayerIndexVal ), "player_%d", i );
			Q_snprintf( szPlayerScoreVal, ARRAYSIZE( szPlayerScoreVal ), "player_%d_points", i );
			Q_snprintf( szPlayerKillsVal, ARRAYSIZE( szPlayerKillsVal ), "player_%d_kills", i );
			Q_snprintf( szPlayerDeathsVal, ARRAYSIZE( szPlayerDeathsVal ), "player_%d_deaths", i );
			int iPlayerIndex = event->GetInt( szPlayerIndexVal, 0 );
			int iRoundScore = event->GetInt( szPlayerScoreVal, 0 );
			int iPlayerKills = event->GetInt( szPlayerKillsVal, 0 );
			int iPlayerDeaths = event->GetInt( szPlayerDeathsVal, 0 );

			// round score of 0 means no player to show for that position (not enough players, or didn't score any points that round)
			if ( iRoundScore > 0 )
				bShow = true;

			CAvatarImagePanel *pPlayerAvatar = dynamic_cast<CAvatarImagePanel *>( m_pWinPanel->FindChildByName( CFmtStr( "Player%dAvatar", i ) ) );

			if ( pPlayerAvatar )
			{
				pPlayerAvatar->ClearAvatar();
				if ( bShow )
				{
					pPlayerAvatar->SetPlayer( GetSteamIDForPlayerIndex( iPlayerIndex ), k_EAvatarSize32x32 );
					pPlayerAvatar->SetAvatarSize( 32, 32 );
				}
				pPlayerAvatar->SetVisible( bShow );
			}

			vgui::Label *pPlayerName = dynamic_cast<Label *>( m_pWinPanel->FindChildByName( CFmtStr( "Player%dName", i ) ) );
			vgui::Label *pPlayerKills = dynamic_cast<Label *>( m_pWinPanel->FindChildByName( CFmtStr( "Player%dKills", i ) ) );
			vgui::Label *pPlayerDeaths = dynamic_cast<Label *>( m_pWinPanel->FindChildByName( CFmtStr( "Player%dDeaths", i ) ) );
			CModelPanel *pPlayerModel = dynamic_cast<CModelPanel *>( m_pWinPanel->FindChildByName( CFmtStr( "Player%dModel", i ) ) );

			if ( !pPlayerName || !pPlayerKills || !pPlayerDeaths )
				return;

			if ( bShow )
			{
				// set the player labels to team color
				Color clr = tf_PR->GetPlayerColor( iPlayerIndex );
				pPlayerName->SetFgColor( clr );
				pPlayerKills->SetFgColor( clr );
				pPlayerDeaths->SetFgColor( clr );

				// set label contents
				pPlayerName->SetText( g_PR->GetPlayerName( iPlayerIndex ) );
				pPlayerKills->SetText( CFmtStr( "Kills: %d", iPlayerKills ) );
				pPlayerDeaths->SetText( CFmtStr( "Deaths: %d", iPlayerDeaths ) );

				if ( i == 1 && iPlayerIndex == GetLocalPlayerIndex() )
					bPlayerFirst = true;

				// store the colors for model coloring
				m_vecWinningPlayerColor.AddToTail( Vector( clr.r() / 255.0f, clr.g() / 255.0f, clr.b() / 255.0f ) );
			}

			// show or hide labels for this player position
			pPlayerName->SetVisible( bShow );
			pPlayerKills->SetVisible( bShow );
			pPlayerDeaths->SetVisible( bShow );
			pPlayerModel->SetVisible( bShow );
		}
		ShowPanel( true );

		if ( !GetTFMusicManager()->IsPlayingMusic() )
		{
			CLocalPlayerFilter filter;
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, ( bPlayerFirst ? "music.dm_winpanel_first" : "music.dm_winpanel" ) );
		}
	}
	else if ( Q_strcmp( "teamplay_round_start", type ) == 0 )
	{
		m_flTimeUpdateTeamScore = 0.0f;
		m_pWinPanel->SetVisible( false );
		bLockInput = false;
		ShowPanel( false );
	}
	if ( IsVisible() )
	{
		Update();
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
SectionedListPanel *CTFDeathMatchScoreBoardDialog::GetSelectedPlayerList( void )
{
	SectionedListPanel *pList = NULL;

	// navigation
	if ( m_pPlayerListRed->GetSelectedItem() >= 0 )
	{
		pList = m_pPlayerListRed;
	}

	return pList;
}


//-----------------------------------------------------------------------------
// Purpose: panel think method
//-----------------------------------------------------------------------------
void CTFDeathMatchScoreBoardDialog::OnThink()
{
	BaseClass::OnThink();

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( IsVisible() && pLocalPlayer && pLocalPlayer->m_nButtons & IN_ATTACK2 )
	{
		SetMouseInputEnabled( true );
	}

	// if we've scheduled ourselves to update the team scores, handle it now
	if ( m_flTimeUpdateTeamScore > 0 && ( gpGlobals->curtime > m_flTimeUpdateTeamScore ) && m_pWinPanel )
	{
		m_pWinPanel->SetVisible( true );
		m_flTimeUpdateTeamScore = 0;
	}

	if ( m_pWinPanel && m_pWinPanel->IsVisible() && !m_vecWinningPlayerColor.IsEmpty() )
	{
		for ( int i = 1; i <= 3; i++ )
		{
			CModelPanel *pPlayerModelPanel = dynamic_cast<CModelPanel *>( m_pWinPanel->FindChildByName( CFmtStr( "Player%dModel", i ) ) );
			if ( pPlayerModelPanel )
			{
				CModelPanelModel *pPanelModel = pPlayerModelPanel->m_hModel.Get();
				if ( pPanelModel )
				{
					pPanelModel->m_nSkin = 8;
					pPanelModel->SetModelColor( m_vecWinningPlayerColor.Head() );
					m_vecWinningPlayerColor.Remove( 0 );
				}
			}
		}
	}
}
