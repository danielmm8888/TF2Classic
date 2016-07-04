//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"

#include <vgui_controls/Label.h>
#include <vgui_controls/Button.h>
#include <vgui_controls/ComboBox.h>
#include <vgui_controls/ImagePanel.h>
#include <vgui_controls/RichText.h>
#include <vgui_controls/Frame.h>
#include <vgui_controls/QueryBox.h>
#include <vgui/IScheme.h>
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include "ienginevgui.h"
#include <game/client/iviewport.h>
#include "tf_tips.h"

#include "tf_statsummary.h"
#include <convar.h>
#include "fmtstr.h"

using namespace vgui;


CTFStatsSummaryPanel *g_pTFStatsSummaryPanel = NULL;

//-----------------------------------------------------------------------------
// Purpose: Returns the global stats summary panel
//-----------------------------------------------------------------------------
CTFStatsSummaryPanel *GStatsSummaryPanel()
{
	if ( NULL == g_pTFStatsSummaryPanel )
	{
		g_pTFStatsSummaryPanel = new CTFStatsSummaryPanel();
	}
	return g_pTFStatsSummaryPanel;
}

//-----------------------------------------------------------------------------
// Purpose: Destroys the global stats summary panel
//-----------------------------------------------------------------------------
void DestroyStatsSummaryPanel()
{
	if ( NULL != g_pTFStatsSummaryPanel )
	{
		delete g_pTFStatsSummaryPanel;
		g_pTFStatsSummaryPanel = NULL;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFStatsSummaryPanel::CTFStatsSummaryPanel() : vgui::EditablePanel( NULL, "TFStatsSummary",
	vgui::scheme()->LoadSchemeFromFile( "Resource/ClientScheme.res", "ClientScheme" ) )
{
	m_bControlsLoaded = false;
	m_bInteractive = false;

	m_pPlayerData = new vgui::EditablePanel( this, "statdata" );
	m_pInteractiveHeaders = new vgui::EditablePanel( m_pPlayerData, "InteractiveHeaders" );
	m_pNonInteractiveHeaders = new vgui::EditablePanel( m_pPlayerData, "NonInteractiveHeaders" );
	m_pTipLabel = new CExLabel( this, "TipLabel", "" );
	m_pTipText = new CExLabel( this, "TipText", "" );

	ListenForGameEvent( "server_spawn" );

	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: Shows this dialog as a modal dialog
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::ShowModal()
{
	// we are in interactive mode, enable controls
	m_bInteractive = true;

	SetParent( enginevgui->GetPanel( PANEL_GAMEDLL ) );
	UpdateDialog();
	SetVisible( true );
	MoveToFront();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	if ( m_pTipLabel && m_pTipText )
	{
		m_pTipLabel->SizeToContents();
		int width = m_pTipLabel->GetWide();

		int x, y, w, t;
		m_pTipText->GetBounds( x, y, w, t );
		m_pTipText->SetBounds( x + width, y, w - width, t );
		m_pTipText->InvalidateLayout( false, true ); // have it re-layout the contents so it's wrapped correctly now that we've changed the size
	}
}

//-----------------------------------------------------------------------------
// Purpose: Command handler
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::OnCommand( const char *command )
{
	if ( 0 == Q_stricmp( command, "vguicancel" ) )
	{
		m_bInteractive = false;
		UpdateDialog();
		SetVisible( false );
		SetParent( (VPANEL)NULL );
	}
	else if ( 0 == Q_stricmp( command, "nexttip" ) )
	{
		UpdateTip();
	}

	BaseClass::OnCommand( command );
}

//-----------------------------------------------------------------------------
// Purpose: Resets the dialog
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::Reset()
{
	m_aClassStats.RemoveAll();

	SetDefaultSelections();
}

static const char* const s_ClassImages[] = {
	"../console/image_scout",				// SCOUT
	"../console/image_sniper",				// SNIPER
	"../console/image_soldier",				// SOLDIER
	"../console/image_demoman",				// DEMOMAN
	"../console/image_medic",				// MEDIC
	"../console/image_heavy_weapons",		// HEAVY_WEAPONS
	"../console/image_pyro",				// PYRO
	"../console/image_spy",					// SPY
	"../console/image_engineer",			// ENGINEER
};

static const char* const s_ClassImagesWidescreen[] = {
	"../console/image_widescreen_scout",			// SCOUT
	"../console/image_widescreen_sniper",			// SNIPER
	"../console/image_widescreen_soldier",			// SOLDIER
	"../console/image_widescreen_demoman",			// DEMOMAN
	"../console/image_widescreen_medic",			// MEDIC
	"../console/image_widescreen_heavy_weapons",	// HEAVY_WEAPONS
	"../console/image_widescreen_pyro",				// PYRO
	"../console/image_widescreen_spy",				// SPY
	"../console/image_widescreen_engineer",			// ENGINEER
};

static const char* const s_Backgrounds[] = {
	"../console/background04",
};

static const char* const s_BackgroundsWidescreen[] = {
	"../console/background04_widescreen",
};

//-----------------------------------------------------------------------------
// Purpose: Sets all user-controllable dialog settings to default values
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::SetDefaultSelections()
{
	m_iSelectedClass = random->RandomInt( TF_FIRST_NORMAL_CLASS, TF_LAST_NORMAL_CLASS ); //TF_CLASS_UNDEFINED;
	m_iSelectedBackground = random->RandomInt( 1, ARRAYSIZE(s_Backgrounds) );
}

//-----------------------------------------------------------------------------
// Purpose: Applies scheme settings
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	SetProportional( true );
	LoadControlSettings( "Resource/UI/StatSummary.res" );
	m_bControlsLoaded = true;

	UpdateDialog();
	SetVisible( false );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::OnKeyCodePressed( KeyCode code )
{}

//-----------------------------------------------------------------------------
// Purpose: Sets stats to use
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::SetStats( CUtlVector<ClassStats_t> &vecClassStats )
{
	m_aClassStats = vecClassStats;
	if ( m_bControlsLoaded )
	{
		UpdateDialog();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the dialog
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::ClearMapLabel()
{
	SetDialogVariable("maplabel", "");
	SetDialogVariable("maptype", "");
	SetDialogVariable("mapauthor", "");

	vgui::Label *pMapAuthorLabel = dynamic_cast<Label *>( FindChildByName("MapAuthorLabel") );
	if ( pMapAuthorLabel && pMapAuthorLabel->IsVisible() )
		pMapAuthorLabel->SetVisible(false);
}

//-----------------------------------------------------------------------------
// Purpose: Updates the dialog
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::UpdateDialog()
{
	RandomSeed(Plat_MSTime());

	m_iTotalSpawns = 0;

	// if we don't have stats for any class, add empty stat entries for them 
	for ( int iClass = TF_FIRST_NORMAL_CLASS; iClass <= TF_LAST_NORMAL_CLASS; iClass++ )
	{
		int j;
		for ( j = 0; j < m_aClassStats.Count(); j++ )
		{
			if ( m_aClassStats[j].iPlayerClass == iClass )
			{
				m_iTotalSpawns += m_aClassStats[j].iNumberOfRounds;
				break;
			}
		}
		if ( j == m_aClassStats.Count() )
		{
			ClassStats_t stats;
			stats.iPlayerClass = iClass;
			m_aClassStats.AddToTail( stats );
		}
	}

	ClearMapLabel();

	// randomize the class and background
	SetDefaultSelections();
	// fill out class details
	UpdateClassDetails();
	// update the tip
	UpdateTip();

	// determine if we're in widescreen or not
	int screenWide, screenTall;
	surface()->GetScreenSize( screenWide, screenTall );
	float aspectRatio = (float) screenWide / (float) screenTall;
	bool bIsWidescreen = aspectRatio >= 1.6f;

	// set the background image
	ImagePanel *pImagePanel = dynamic_cast<ImagePanel *>( FindChildByName("MainBackground") );
	if ( pImagePanel )
	{
		pImagePanel->SetImage( bIsWidescreen ? s_BackgroundsWidescreen[m_iSelectedBackground - 1] : s_Backgrounds[m_iSelectedBackground - 1] );
	}

	// set the class image
	ImagePanel *pImagePanelClassBackground = dynamic_cast<ImagePanel *>( FindChildByName("ClassImage") );
	if ( pImagePanelClassBackground )
	{
		pImagePanelClassBackground->SetImage( bIsWidescreen ? s_ClassImagesWidescreen[m_iSelectedClass - 1] : s_ClassImages[m_iSelectedClass - 1] );
	}
}

#define MAKEFLAG(x)	( 1 << x )

#define ALL_CLASSES 0xFFFFFFFF

//-----------------------------------------------------------------------------
// Purpose: Updates class details
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::UpdateClassDetails()
{
	for ( int i = 0; i < m_aClassStats.Count(); i++ )
	{
		if ( m_aClassStats[i].iPlayerClass != m_iSelectedClass )
			continue;
		const char* playtime = RenderValue( GetDisplayValue( m_aClassStats[i], TFSTAT_PLAYTIME, SHOW_TOTAL ), TFSTAT_PLAYTIME, SHOW_TOTAL );

		SetDialogVariable( "playtime", playtime );
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Updates the tip
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::UpdateTip()
{
	SetDialogVariable( "tiptext", g_TFTips.GetNextClassTip( m_iSelectedClass ) );
}

//-----------------------------------------------------------------------------
// Purpose: Shows or hides controls
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::UpdateControls()
{
	bool bShowPlayerData = ( m_bInteractive || m_iTotalSpawns > 0 );

	m_pPlayerData->SetVisible( bShowPlayerData );
	m_pInteractiveHeaders->SetVisible( m_bInteractive );
	m_pNonInteractiveHeaders->SetVisible( !m_bInteractive );
	m_pTipText->SetVisible( bShowPlayerData );
	m_pTipLabel->SetVisible( bShowPlayerData );
}

//-----------------------------------------------------------------------------
// Purpose: Calculates a fraction and guards from divide by 0.  (Returns 0 if 
//			denominator is 0.)
//-----------------------------------------------------------------------------
float CTFStatsSummaryPanel::SafeCalcFraction( float flNumerator, float flDemoninator )
{
	if ( 0 == flDemoninator )
		return 0;
	return flNumerator / flDemoninator;
}

//-----------------------------------------------------------------------------
// Purpose: Formats # of seconds into a string
//-----------------------------------------------------------------------------
const char *FormatSeconds( int seconds )
{
	static char string[64];

	int hours = 0;
	int minutes = seconds / 60;

	if ( minutes > 0 )
	{
		seconds -= (minutes * 60);
		hours = minutes / 60;

		if ( hours > 0 )
		{
			minutes -= (hours * 60);
		}
	}

	if ( hours > 0 )
	{
		Q_snprintf( string, sizeof(string), "%2i:%02i:%02i", hours, minutes, seconds );
	}
	else
	{
		Q_snprintf( string, sizeof(string), "%02i:%02i", minutes, seconds );
	}

	return string;
}

//-----------------------------------------------------------------------------
// Purpose: Returns the stat value for specified display type
//-----------------------------------------------------------------------------
float CTFStatsSummaryPanel::GetDisplayValue( ClassStats_t &stats, TFStatType_t statType, StatDisplay_t statDisplay )
{
	switch ( statDisplay )
	{
	case SHOW_MAX:
		return stats.max.m_iStat[statType];
		break;
	case SHOW_TOTAL:
		return stats.accumulated.m_iStat[statType];
		break;
	case SHOW_AVG:
		return SafeCalcFraction( stats.accumulated.m_iStat[statType], stats.iNumberOfRounds );
		break;
	default:
		AssertOnce( false );
		return 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Gets the text representation of this value
//-----------------------------------------------------------------------------
const char *CTFStatsSummaryPanel::RenderValue( float flValue, TFStatType_t statType, StatDisplay_t statDisplay )
{
	static char szValue[64];
	if ( TFSTAT_PLAYTIME == statType )
	{
		// the playtime stat is shown in seconds
		return FormatSeconds( (int) flValue );
	}
	else if (SHOW_AVG == statDisplay)
	{
		// if it's an average, render as a float w/2 decimal places
		Q_snprintf( szValue, ARRAYSIZE( szValue ), "%.2f", flValue );
	}
	else
	{
		// otherwise, render as an integer
		Q_snprintf( szValue, ARRAYSIZE( szValue ), "%d", (int) flValue );
	}

	return szValue;
}

extern const char *GetMapDisplayName( const char *mapName );
extern const char *GetMapType( const char *mapName );
extern const char *GetMapAuthor( const char *mapName );

//-----------------------------------------------------------------------------
// Purpose: Event handler
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::FireGameEvent( IGameEvent *event )
{
	const char *pEventName = event->GetName();

	// when we are changing levels and 
	if ( 0 == Q_strcmp( pEventName, "server_spawn" ) )
	{
		if ( !m_bInteractive )
		{
			const char *pMapName = event->GetString( "mapname" );
			if ( pMapName )
			{
				// If we're loading a background map, don't display anything
				// HACK: Client doesn't get gpGlobals->eLoadType, so just do string compare for now.
				if ( Q_stristr( pMapName, "background" ) )
				{
					ClearMapLabel();
				}
				else
				{
					// set the map name in the UI
					wchar_t wzMapName[255] = L"";
					g_pVGuiLocalize->ConvertANSIToUnicode( GetMapDisplayName( pMapName ), wzMapName, sizeof( wzMapName ) );

					SetDialogVariable( "maplabel", wzMapName );

					// set the map type in the UI
					const char *szMapType = GetMapType( pMapName );
					SetDialogVariable( "maptype", g_pVGuiLocalize->Find( szMapType ) );

					// set the map author name in the UI
					const char *szMapAuthor = GetMapAuthor( pMapName );
					if ( szMapAuthor[0] != '\0' )
					{
						SetDialogVariable( "mapauthor", szMapAuthor );

						Label *pMapAuthorLabel = dynamic_cast<Label *>( FindChildByName( "MapAuthorLabel" ) );
						if ( pMapAuthorLabel && !pMapAuthorLabel->IsVisible() )
							pMapAuthorLabel->SetVisible( true );
					}
				}
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called when we are activated during level load
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::OnActivate()
{
	ClearMapLabel();

	UpdateDialog();
}

//-----------------------------------------------------------------------------
// Purpose: Called when we are deactivated at end of level load
//-----------------------------------------------------------------------------
void CTFStatsSummaryPanel::OnDeactivate()
{
	ClearMapLabel();
}

CON_COMMAND( showstatsdlg, "Shows the player stats dialog" )
{
	GStatsSummaryPanel()->ShowModal();
}
