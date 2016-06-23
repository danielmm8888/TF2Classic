#include "cbase.h"
#include "tf_music_manager.h"
#include "tf_gamerules.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar tf2c_music_manager( "tf2c_music_manager", "0", FCVAR_ARCHIVE );
ConVar tf2c_music_manager_volume( "tf2c_music_manager_volume", "0.75", FCVAR_ARCHIVE );
ConVar tf2c_music_manager_track( "tf2c_music_manager_track", "1", FCVAR_ARCHIVE, NULL, true, 1, true, 3 );

static CTFMusicManager g_TFMusicManager;
CTFMusicManager *GetTFMusicManager( void )
{
	return &g_TFMusicManager;
}

CTFMusicManager::CTFMusicManager() : CAutoGameSystemPerFrame( "CTFMusicManager" )
{
	m_bPlaying = false;
	m_flIntensity = 0.0f;
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
// Purpose: Check intensity conditions here.
//-----------------------------------------------------------------------------
void CTFMusicManager::LevelShutdownPreEntity( void )
{
	StopMusic( false );
}

//-----------------------------------------------------------------------------
// Purpose: Check intensity conditions here.
//-----------------------------------------------------------------------------
void CTFMusicManager::Update( float flFrameTime )
{
	if ( !m_bPlaying )
		return;

	C_BasePlayer *pLocalPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pLocalPlayer )
		return;

	bool bAlive = pLocalPlayer->IsAlive();
	bool bPreGame = TFGameRules()->IsInWaitingForPlayers();

	if ( !bAlive || bPreGame )
	{
		// Keep it queit if player is dead.
		m_flIntensity = 0.0f;
	}
	else
	{
		// Calculate intensity based on distance from other players.
		float flClosest = FLT_MAX;
		for ( int i = 1; i <= gpGlobals->maxClients; i++ )
		{
			C_BasePlayer *pPlayer = UTIL_PlayerByIndex( i );
			if ( !pPlayer || pPlayer == pLocalPlayer || !pPlayer->IsAlive() )
				continue;

			trace_t tr;
			UTIL_TraceLine( pLocalPlayer->EyePosition(), pPlayer->WorldSpaceCenter(), MASK_VISIBLE, NULL, COLLISION_GROUP_NONE, &tr );
			if ( tr.fraction != 1.0f )
				continue;

			float flDist = ( pPlayer->GetAbsOrigin() - pLocalPlayer->GetAbsOrigin() ).Length();
			flClosest = min( flDist, flClosest );
		}

		m_flIntensity = RemapValClamped( flClosest, 0, 1024, 1.0f, 0.0f );
	}

	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	bool bLoop = false;
	if ( m_flLoopTime != 0.0f && gpGlobals->curtime >= m_flLoopTime )
	{
		DevMsg( "Looping MP3.\n" );
		bLoop = true;
		m_flLoopTime = gpGlobals->curtime + m_flDuration;
	}

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
	else if ( V_strcmp( event->GetName(), "teamplay_win_panel" ) == 0 )
	{
		StopMusic( false );
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
	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();
	if ( !pPlayer )
		return false;

	// Don't play music for spectators.
	if ( pPlayer->GetTeamNumber() < FIRST_GAME_TEAM )
		return false;

	if ( TFGameRules() )
	{
		if ( TFGameRules()->State_Get() == GR_STATE_GAME_OVER )
			return false;
	}

	return true;
}
