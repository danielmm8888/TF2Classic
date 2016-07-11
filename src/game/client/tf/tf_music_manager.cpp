#include "cbase.h"
#include "tf_music_manager.h"
#include "tf_gamerules.h"
#include "c_tf_player.h"
#include <vgui_controls/Controls.h>
#include <vgui/ISurface.h>
#include <vgui/IScheme.h>
#include <vgui_controls/Panel.h>
#include "hudelement.h"
#include "iclientmode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static CTFMusicManager g_TFMusicManager;
CTFMusicManager *GetTFMusicManager( void )
{
	return &g_TFMusicManager;
}

static void MusicManagerToggle( IConVar *var, const char *pOldValue, float flOldValue )
{
	if ( !TFGameRules() || !TFGameRules()->IsDeathmatch() )
		return;

	ConVar *pCvar = (ConVar *)var;
	if ( pCvar->GetBool() )
	{
		if ( g_TFMusicManager.CanPlayMusic() )
		{
			g_TFMusicManager.StartMusic();
		}
	}
	else
	{
		g_TFMusicManager.StopMusic();
	}
}

ConVar tf2c_music_manager( "tf2c_music_manager", "0", FCVAR_ARCHIVE, "Enable dynamic music in Deathmatch.", MusicManagerToggle );
ConVar tf2c_music_manager_volume( "tf2c_music_manager_volume", "0.75", FCVAR_ARCHIVE );
ConVar tf2c_music_manager_track( "tf2c_music_manager_track", "1", FCVAR_ARCHIVE, NULL, true, 1, true, 3 );

ConVar tf2c_music_manager_debug( "tf2c_music_manager_debug", "0", FCVAR_CHEAT );


CTFMusicManager::CTFMusicManager() : CAutoGameSystemPerFrame( "CTFMusicManager" )
{
	m_bPlaying = false;
	m_flIntensity = 0.0f;
	m_flPlayerIntensity = 0.0f;
	m_iTrack = 1;
	m_flLoopTime = 0.0f;
	m_flDuration = 0.0f;
}

CTFMusicManager::~CTFMusicManager()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFMusicManager::Init( void )
{
	ListenForGameEvent( "localplayer_changeteam" );
	ListenForGameEvent( "teamplay_round_start" );
	ListenForGameEvent( "player_death" );
	ListenForGameEvent( "player_spawn" );
	ListenForGameEvent( "teamplay_win_panel" );

	return true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMusicManager::LevelShutdownPreEntity( void )
{
	StopMusic( false );
	m_flIntensity = 0.0f;
	m_flPlayerIntensity = 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Check intensity conditions here.
//-----------------------------------------------------------------------------
void CTFMusicManager::Update( float flFrameTime )
{
	if ( !m_bPlaying )
		return;

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalPlayer )
		return;

	// Watch track changes.
	if ( tf2c_music_manager_track.GetInt() != m_iTrack )
	{
		StopMusic();

		if ( CanPlayMusic() )
			StartMusic();

		return;
	}

	bool bAlive = pLocalPlayer->IsAlive();
	bool bPreGame = TFGameRules()->IsInWaitingForPlayers();

	m_flIntensity = 0.0f;

	// Keep it quiet if player is dead.
	if ( bAlive && !bPreGame )
	{
		// Killstreak increases intensity, 5% per kill.
		m_flIntensity += (float)pLocalPlayer->m_Shared.GetKillstreak() * 0.05f;

		// Low health increases intensity up to 70%.
		int iHalfHealth = pLocalPlayer->GetMaxHealth() / 2;
		if ( pLocalPlayer->GetHealth() < iHalfHealth )
		{
			m_flIntensity += RemapValClamped( pLocalPlayer->GetHealth(), iHalfHealth, 1, 0.0f, 0.70f );
		}

		m_flIntensity = clamp( m_flIntensity, 0.0f, 1.0f );

		float flPlayerIntensity = 0.0f;

		// Calculate intensity based on distance from other players.
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			C_BasePlayer *pPlayer = UTIL_PlayerByIndex( i );
			if ( !pPlayer || pPlayer == pLocalPlayer || !pPlayer->IsAlive() )
				continue;

			int x, y;

			// Only count them if they're on-screen.
			if ( GetVectorInScreenSpace( pPlayer->WorldSpaceCenter(), x, y ) == false )
				continue;

			trace_t tr;
			UTIL_TraceLine( pLocalPlayer->EyePosition(), pPlayer->WorldSpaceCenter(), MASK_VISIBLE, NULL, COLLISION_GROUP_NONE, &tr );
			if ( tr.fraction != 1.0f )
				continue;

			// Up to 50% per player.
			float flDist = ( pPlayer->GetAbsOrigin() - pLocalPlayer->GetAbsOrigin() ).Length();
			flPlayerIntensity += RemapValClamped( flDist, 1024, 0, 0.0f, 0.5f );
		}

		if ( flPlayerIntensity >= m_flPlayerIntensity )
		{
			m_flPlayerIntensity = flPlayerIntensity;
		}
		else
		{
			// Go down gradually.
			m_flPlayerIntensity = Approach( flPlayerIntensity, m_flPlayerIntensity, 0.05f * gpGlobals->frametime );
		}

		m_flPlayerIntensity = clamp( m_flPlayerIntensity, 0.0f, 1.0f - m_flIntensity );

		m_flIntensity += m_flPlayerIntensity;
	}
	 
	bool bLoop = false;
	if ( m_flLoopTime != 0.0f && gpGlobals->curtime >= m_flLoopTime )
	{
		DevMsg( "Looping MP3.\n" );
		bLoop = true;
		m_flLoopTime = gpGlobals->curtime + m_flDuration;
	}

	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	for ( int i = 0; i < TF_MUSIC_LAYERS; i++ )
	{
		SMusicTrack *pTrack = &m_Tracks[i];
		if ( !pTrack->pSound )
			continue;

		float flTargetLevel = TF_MUSIC_INTENSITY_FRACTION * (float)i;

		// Setting volume to 0 pauses the music so we're bringing inactive tracks to inaudible levels instead.
		pTrack->bPlay = m_flIntensity >= flTargetLevel;
		float flOldVolume = controller.SoundGetVolume( pTrack->pSound );
		float flVolume = tf2c_music_manager_volume.GetFloat();
		float flDeltaTime = 0.5f;

		if ( !pTrack->bPlay )
			flVolume = 0.01f;
		else if ( bPreGame )
		{
			flVolume *= 0.5f;
		}
		else if ( !bAlive )
			flVolume *= 0.25f; // Tone it down when player is dead.

		// HACK: Source can't loop MP3, gotta loop manually...
		if ( bLoop )
		{
			controller.Shutdown( pTrack->pSound );
			controller.Play( pTrack->pSound, flOldVolume, 100 );
			m_flLoopTime = gpGlobals->curtime + m_flDuration;
		}

		if ( flVolume != flOldVolume )
		{
			controller.SoundChangeVolume( pTrack->pSound, flVolume, flDeltaTime );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMusicManager::FireGameEvent( IGameEvent *event )
{
	if ( !tf2c_music_manager.GetBool() )
		return;

	if ( !TFGameRules() || !TFGameRules()->IsDeathmatch() )
		return;

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return;

	if ( V_strcmp( event->GetName(), "localplayer_changeteam" ) == 0 )
	{
		// Start music if player entered the battle.
		if ( CanPlayMusic() )
		{
			StartMusic();
		}
		else if ( pPlayer->GetTeamNumber() < FIRST_GAME_TEAM )
		{
			// Play ending cue if player switched to spec.
			StopMusic( true );
		}
	}
	else if ( V_strcmp( event->GetName(), "teamplay_round_start" ) == 0 )
	{
		// Restart music.
		StopMusic( false );

		if ( CanPlayMusic() )
		{
			StartMusic();
		}
	}
	else if ( V_strcmp( event->GetName(), "player_death" ) == 0 )
	{
		int iVictim = event->GetInt( "userid" );
		if ( iVictim == pPlayer->GetUserID() )
		{
			// Player died, reset intensity.
			m_flIntensity = 0.0f;
			m_flPlayerIntensity = 0.0f;
		}
	}
	else if ( V_strcmp( event->GetName(), "teamplay_win_panel" ) == 0 )
	{
		StopMusic( true );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMusicManager::StartMusic( void )
{
	if ( m_bPlaying )
		return;

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	for ( int i = 0; i < TF_MUSIC_LAYERS; i++ )
	{
		SMusicTrack *pTrack = &m_Tracks[i];

		m_iTrack = tf2c_music_manager_track.GetInt();
		char szSound[128];
		V_snprintf( szSound, 128, "music/combat_%d/%d.mp3", m_iTrack, i );

		// Gotta remember loop point for later. Stupid Source not being able to loop stupid MP3s...
		m_flDuration = enginesound->GetSoundDuration( szSound );
		m_flLoopTime = gpGlobals->curtime + m_flDuration;

		CLocalPlayerFilter filter;
		pTrack->pSound = controller.SoundCreate( filter, pPlayer->entindex(), CHAN_STATIC, szSound, SNDLVL_NONE );
		controller.Play( pTrack->pSound, 0.01f, 100 );
	}

	m_bPlaying = true;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFMusicManager::StopMusic( bool bPlayEnding /*= false*/ )
{
	if ( !m_bPlaying )
		return;

	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	for ( int i = 0; i < TF_MUSIC_LAYERS; i++ )
	{
		SMusicTrack *pTrack = &m_Tracks[i];

		if ( pTrack->pSound )
		{
			controller.SoundDestroy( pTrack->pSound );
			pTrack->pSound = NULL;
		}

		pTrack->bPlay = false;
	}

	// Play ending segment that corrensonds to the current track.
	if ( bPlayEnding )
	{
		const char *pszFormat = GetMusicPower() < 3 ? "music/combat_%d/end_medium.mp3" : "music/combat_%d/end_heavy.mp3";;
		char szSound[128];
		V_snprintf( szSound, 128, pszFormat, m_iTrack );

		EmitSound_t params;
		params.m_pSoundName = szSound;
		params.m_flVolume = tf2c_music_manager_volume.GetFloat();
		params.m_nChannel = CHAN_STATIC;

		CLocalPlayerFilter filter;
		C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, params );
	}

	m_flIntensity = 0.0f;
	m_flPlayerIntensity = 0.0f;
	m_bPlaying = false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int CTFMusicManager::GetMusicPower( void )
{
	return (int)( m_flIntensity / TF_MUSIC_INTENSITY_FRACTION );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CTFMusicManager::CanPlayMusic( void )
{
	// Don't play music for spectators.
	if ( GetLocalPlayerTeam() < FIRST_GAME_TEAM )
		return false;

	if ( TFGameRules() )
	{
		if ( TFGameRules()->State_Get() == GR_STATE_GAME_OVER )
			return false;
	}

	return true;
}

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: For debugging music manager.
//-----------------------------------------------------------------------------
class CHudMusicManager : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CHudMusicManager, vgui::Panel );

public:
	CHudMusicManager( const char *pElementName );
	~CHudMusicManager( void );

	virtual void	ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void	Paint();

	virtual bool	ShouldDraw( void );

	vgui::HFont		m_hFont;
};

DECLARE_HUDELEMENT( CHudMusicManager );

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *parent - 
//-----------------------------------------------------------------------------
CHudMusicManager::CHudMusicManager( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudMusicManager" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_hFont = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudMusicManager::~CHudMusicManager( void )
{
}

void CHudMusicManager::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_hFont = pScheme->GetFont( "Default" );

	SetPaintBackgroundEnabled( false );
	SetSize( ScreenWidth(), ScreenHeight() );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CHudMusicManager::ShouldDraw( void )
{
	if ( tf2c_music_manager_debug.GetBool() )
		return true;

	return false;
}

void CHudMusicManager::Paint( void )
{
	surface()->DrawSetTextFont( m_hFont );

	Color col;
	if ( g_TFMusicManager.m_flIntensity == 1.0f )
	{
		col.SetColor( 255, 0, 0, 255 );
	}
	else if ( g_TFMusicManager.m_flPlayerIntensity )
	{
		col.SetColor( 255, 255, 0, 255 );
	}
	else
	{
		col.SetColor( 0, 255, 0, 255 );
	}

	surface()->DrawSetTextColor( col );

	int vx, vy, vw, vh;
	vgui::surface()->GetFullscreenViewport( vx, vy, vw, vh );
	surface()->DrawSetTextPos( vw - 300, 12 );
	
	wchar_t buf[64];
	V_snwprintf( buf, 64, L"Intensity: %.f%%", g_TFMusicManager.m_flIntensity * 100.0f );
	surface()->DrawPrintText( buf, wcslen( buf ) );
}
