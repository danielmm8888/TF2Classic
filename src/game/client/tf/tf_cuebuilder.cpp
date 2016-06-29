#include "cbase.h"
#include "tf_cuebuilder.h"
#include "tier3/tier3.h"
#include "cdll_util.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"
#include "script_parser.h"
#include "c_tf_player.h"
#include "tf_gamerules.h"
#include "c_playerresource.h"
#include "c_tf_playerresource.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

const char *g_aCueMood[MOOD_COUNT] =
{
	"MOOD_NEUTRAL",
	"MOOD_DANGER",
};

const char *g_aCueLayer[LAYER_COUNT] =
{
	"LAYER_MAIN",
	"LAYER_MISC",
	"LAYER_PERC",
};


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
static CTFCueBuilder g_TFCueBuilder;
CTFCueBuilder *GetCueBuilder()
{
	return &g_TFCueBuilder;
}

class CTFMusicScriptParser : public C_ScriptParser
{
public:
	DECLARE_CLASS_GAMEROOT(CTFMusicScriptParser, C_ScriptParser);

	void Parse(KeyValues *pKeyValuesData, bool bWildcard, const char *szFileWithoutEXT)
	{
		int id = -1;
		CueTrack *pTrack = new CueTrack(GetCueBuilder(), szFileWithoutEXT);

		for (KeyValues *pData = pKeyValuesData->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey())
		{
			id++;

			CueSequence pSeq;
			pSeq.id = id;
			Q_strncpy(pSeq.sName, pData->GetString("name", ""), sizeof(pSeq.sName));
			pSeq.pitch = pData->GetInt("pitch", PITCH_NORM);
			pSeq.skipmultiplier = pData->GetFloat("skipmultiplier", 0.0f);
			if (!Q_strncasecmp(pData->GetString("soundlevel", ""), "SNDLVL_", strlen("SNDLVL_")))
			{
				pSeq.soundlevel = TextToSoundLevel(pData->GetString("soundlevel", "SNDLVL_NONE"));
			}
			else
			{
				pSeq.soundlevel = (soundlevel_t)pData->GetInt("soundlevel", ATTN_TO_SNDLVL(ATTN_NORM));
			}

			for (int i = 0; i < MOOD_COUNT * LAYER_COUNT; i++)
			{
				Q_strncpy(pSeq.pTracks[i].sWaveName, "", sizeof(pSeq.pTracks[i].sWaveName));
				pSeq.pTracks[i].fWaveVolume = 1.0f;
			}
			for (KeyValues *pLayerData = pData->GetFirstSubKey(); pLayerData != NULL; pLayerData = pLayerData->GetNextKey())
			{
				for (int i = LAYER_MAIN; i < LAYER_COUNT; i++)
				{
					if (!Q_stricmp(pLayerData->GetName(), g_aCueLayer[i]))
					{
						char sDefaultTrackName[64];
						Q_strncpy(sDefaultTrackName, pLayerData->GetString("wave", ""), sizeof(sDefaultTrackName));
						float fDefaultVolume = pLayerData->GetFloat("volume", 1.0f);
						pSeq.AddTrack(sDefaultTrackName, fDefaultVolume, (CueLayer)i, MOOD_NEUTRAL);

						for (KeyValues *pMoodData = pLayerData->GetFirstSubKey(); pMoodData != NULL; pMoodData = pMoodData->GetNextKey())
						{
							for (int j = MOOD_NEUTRAL + 1; j < MOOD_COUNT; j++)
							{
								if (!Q_stricmp(pMoodData->GetName(), g_aCueMood[j]))
								{
									char sTrackName[64];
									Q_strncpy(sTrackName, pMoodData->GetString("wave_override", ""), sizeof(sTrackName));
									float fVolume = pMoodData->GetFloat("volume_override", 1.0f);
									pSeq.AddTrack(sTrackName, fVolume, (CueLayer)i, (CueMood)j);
								}
							}
						}
					}
				}
			}
			pTrack->AddSequence(pSeq, pData->GetName());
		}
		pTFCueBuilder->AddTrack(szFileWithoutEXT, pTrack);
	};

	void SetCueBuilder(CTFCueBuilder *pBuilder)
	{
		pTFCueBuilder = pBuilder;
	}

private:
	CTFCueBuilder *pTFCueBuilder;
	CUtlDict< CueSequence, unsigned short > m_TrackInfoDatabase;
};
CTFMusicScriptParser g_TFMusicScriptParser;

static void EnableCommand(IConVar *var, const char *pOldValue, float flOldValue)
{
	ConVar *pCvar = (ConVar *)var;
	if (pCvar->GetBool())
	{
		GetCueBuilder()->ResetAndStartCue();
	}
	else
	{
		GetCueBuilder()->StopCue();
	}
}
ConVar tf2c_cues_enabled("tf2c_cues_enabled", "0", FCVAR_USERINFO | FCVAR_ARCHIVE, "Enable dynamic music", EnableCommand);

void PlayCommand(const CCommand &args)
{
	const char* sName = args[1];
	GetCueBuilder()->StopCue();
	GetCueBuilder()->SetCurrentTrack(sName);
	GetCueBuilder()->ResetAndStartCue();
}
ConCommand tf2c_cues_play("tf2c_cues_play", PlayCommand, "", FCVAR_DEVELOPMENTONLY);

void StopCommand(const CCommand &args)
{
	GetCueBuilder()->StopCue();
}
ConCommand tf2c_cues_stop("tf2c_cues_stop", StopCommand, "", FCVAR_DEVELOPMENTONLY);

void SetSequenceCommand(const CCommand &args)
{
	int ID = atoi(args[1]);
	GetCueBuilder()->GetCurrentTrack()->Stop();
	GetCueBuilder()->GetCurrentTrack()->SetCurrentSeqID(ID);
	GetCueBuilder()->GetCurrentTrack()->Play();
}
ConCommand tf2c_cues_setsequence("tf2c_cues_setsequence", SetSequenceCommand, "", FCVAR_DEVELOPMENTONLY);

void SkipCommand(const CCommand &args)
{
	GetCueBuilder()->GetCurrentTrack()->SetShouldSkip(true);
}
ConCommand tf2c_cues_skipsequence("tf2c_cues_skipsequence", SkipCommand, "", FCVAR_DEVELOPMENTONLY);

void SetMoodCommand(const CCommand &args)
{
	int iMood = atoi(args[1]);
	GetCueBuilder()->SetMood((CueMood)iMood);
}
ConCommand tf2c_cues_setmood("tf2c_cues_setmood", SetMoodCommand, "", FCVAR_DEVELOPMENTONLY);


//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CTFCueBuilder::CTFCueBuilder() : CAutoGameSystemPerFrame("CTFCueBuilder")
{
	if (!filesystem)
		return;

	m_bInited = false;
	Init();
}

CTFCueBuilder::~CTFCueBuilder()
{
	m_PlaylistDatabase.RemoveAll();
}

//-----------------------------------------------------------------------------
// Purpose: Initializer
//-----------------------------------------------------------------------------
bool CTFCueBuilder::Init()
{
	if (!m_bInited)
	{
		m_iGlobalMood = MOOD_NEUTRAL;
		m_iCurrentTrack = -1;

		g_TFMusicScriptParser.SetCueBuilder(this);
		g_TFMusicScriptParser.InitParser("scripts/tf_music_*.txt", true, false);

		ListenForGameEvent("server_spawn");
		ListenForGameEvent("localplayer_changeteam");
		ListenForGameEvent("player_death");
		ListenForGameEvent("teamplay_win_panel");
		ListenForGameEvent("teamplay_round_start");
		m_bInited = true;
	}

	return true;
}

void CTFCueBuilder::Update(float frametime)
{
	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	if (!pLocalPlayer)
		return;

	for (unsigned int i = 0; i < m_PlaylistDatabase.Count(); i++)
	{
		m_PlaylistDatabase[i]->Update();
	}
}

//-----------------------------------------------------------------------------
// Purpose: Event handler
//-----------------------------------------------------------------------------
void CTFCueBuilder::FireGameEvent( IGameEvent *event )
{
	if ( !tf2c_cues_enabled.GetBool() )
		return;

	if ( !TFGameRules() )
		return;

	const char *type = event->GetName();

	if (0 == Q_strcmp(type, "localplayer_changeteam"))
	{
		if (TFGameRules()->IsDeathmatch())
		{
			StopCue();
			SetCurrentTrack("tf_music_deathmatch");
			ResetAndStartCue();
		}
	}

	if (0 == Q_strcmp(type, "teamplay_round_start"))
	{
		if (TFGameRules()->IsDeathmatch())
		{
			StopCue();
			SetCurrentTrack("tf_music_deathmatch");
			ResetAndStartCue();
		}
	}

	if (0 == Q_strcmp(type, "server_spawn"))
	{
		if (TFGameRules()->IsDeathmatch())
		{
			StopCue();
		}
	}

	if (0 == Q_strcmp(type, "teamplay_win_panel"))
	{
		if (TFGameRules()->IsDeathmatch())
		{
			StopCue();
		}
	}

	if (0 == Q_strcmp(type, "player_death"))
	{
		if (TFGameRules()->IsDeathmatch())
		{
			C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>(g_PR);
			if (!tf_PR)
				return;

			if (m_iCurrentTrack == -1)
				return;

			int iLocalIndex = GetLocalPlayerIndex();
			int iLocalScore = tf_PR->GetTotalScore(iLocalIndex);
			int iLocalKillstreak = tf_PR->GetKillstreak(iLocalIndex);

			float fSkipmult = GetCurrentTrack()->GetCurrentSequence().skipmultiplier;
			ConVar *mp_fraglimit = cvar->FindVar("mp_fraglimit");
			int iSkipAmmount = (mp_fraglimit ? fSkipmult * mp_fraglimit->GetInt() : 0);
			if (iLocalScore > iSkipAmmount)
			{
				GetCurrentTrack()->SetShouldSkip(true);
			}
			
			int userid = event->GetInt("userid");
			if (userid == iLocalIndex + 1)
			{
				SetMood(MOOD_NEUTRAL);
			}
			else if (iLocalKillstreak > 3)
			{
				SetMood(MOOD_DANGER);
			}
		}
	}
}

void CTFCueBuilder::AddTrack(const char* name, CueTrack* pCueTrack)
{
	m_PlaylistDatabase.Insert(name, pCueTrack);
}

void CTFCueBuilder::SetMood(CueMood mood)
{
	DevMsg("Mood set to %s\n", g_aCueMood[mood]);
	m_iGlobalMood = mood;
	if (GetCurrentTrack()->GetCurrentSeqID() > -1)
		GetCurrentTrack()->SetVolumes();
};

void CTFCueBuilder::StartCue()
{
	CueTrack *pCurrentTrack = GetCurrentTrack();
	DevMsg("Playing track %s\n", pCurrentTrack->GetTrackName());
	if (!tf2c_cues_enabled.GetBool())
		return;
	pCurrentTrack->StartPlaying();
}

void CTFCueBuilder::ResetAndStartCue()
{
	if (m_iCurrentTrack == -1)
		return;
	GetCurrentTrack()->SetCurrentSeqID(-1);
	SetMood(MOOD_NEUTRAL);
	StartCue();
}

void CTFCueBuilder::StopCue()
{
	int iCurrentID = GetCurrentTrackID();
	if (iCurrentID < 0)
		return;

	CueTrack *pCurrentTrack = GetCurrentTrack();
	DevMsg("Stop playing track %s\n", pCurrentTrack->GetTrackName());
	pCurrentTrack->StopPlaying();
}



//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CueTrack::CueTrack(CTFCueBuilder *g_TFCueBuilder, const char* sName)
{
	pTFCueBuilder = g_TFCueBuilder;
	Q_strncpy(m_sName, sName, sizeof(m_sName));
	m_iCurrentSequence = -1;
	for (int i = 0; i < LAYER_COUNT * MOOD_COUNT; i++)
		m_pPlayList[i] = NULL;
	m_TrackInfoDatabase.RemoveAll();
	m_bPlay = false;
}

void CueTrack::Update()
{
	if (!m_bPlay)
		return;

	bool bPlaying = IsStillPlaying();
	bool bLoopEnded = false;
	if (GetCurrentSeqID() == -1)
	{
		NextSeq();
		return;
	}

	CueSequence pSeqInfo = GetSequence(GetCurrentSeqID());
	float fDuration = enginesound->GetSoundDuration(pSeqInfo.GetTrack(LAYER_MAIN, MOOD_NEUTRAL).sWaveName);
	if (m_fCurrentDuration + fDuration <= gpGlobals->curtime)
	{
		DevMsg("Loop ended\n");
		m_fCurrentDuration += fDuration;
		bLoopEnded = true;
	}
	if (bLoopEnded && GetShouldSkip())
	{
		SetShouldSkip(false);
		Stop();
		NextSeq();
	}
	if (!bPlaying)
	{
		Play();
	}
}

GUID CueTrack::PlayLayer(int ID, CueLayer Layer, CueMood Mood)
{
	char m_pzMusicLink[64];
	CueSequence pSongInfo = GetSequence(ID);

	Q_strncpy(m_pzMusicLink, pSongInfo.GetTrack(Layer, Mood).sWaveName, sizeof(m_pzMusicLink));
	if (m_pzMusicLink[0] == '\0')
	{
		Q_strncpy(m_pzMusicLink, pSongInfo.GetTrack(Layer, MOOD_NEUTRAL).sWaveName, sizeof(m_pzMusicLink));
	}

	if (m_pzMusicLink[0] == '\0')
		return 0;
	
	CLocalPlayerFilter filter;
	enginesound->EmitSound(filter, SOUND_FROM_LOCAL_PLAYER, CHAN_STATIC, m_pzMusicLink, 1.0f, pSongInfo.soundlevel, 0, pSongInfo.pitch);
	GUID guid = enginesound->GetGuidForLastSoundEmitted();
	SetGuid(guid, Layer, Mood);
	return guid;
}


void CueTrack::AddSequence(CueSequence pSequence, const char* name)
{
	m_TrackInfoDatabase.Insert(name, pSequence);
}

CueSequence CueTrack::GetSequence(int ID)
{
	return m_TrackInfoDatabase[ID];
}

CueSequence CueTrack::GetCurrentSequence()
{
	return m_TrackInfoDatabase[GetCurrentSeqID()];
}

int CueTrack::GetSeqCount()
{
	return m_TrackInfoDatabase.Count();
}

bool CueTrack::GetShouldSkip() 
{
	return pTFCueBuilder->GetShouldSkip(); 
};

void CueTrack::SetShouldSkip(bool bSkip)
{
	if (bSkip)
	{
		DevMsg("Skipping to the next loop\n");
	}
	pTFCueBuilder->SetShouldSkip(bSkip); 
};

void CueTrack::SetVolumes()
{
	for (int i = 0; i < LAYER_COUNT; i++)
	{
		for (int j = 0; j < MOOD_COUNT; j++)
		{
			float fTrackVolume = GetCurrentSequence().GetTrack((CueLayer)i, (CueMood)j).fWaveVolume;
			if (GetCurrentSequence().GetTrack((CueLayer)i, (CueMood)j).sWaveName[0] == '\0')
			{
				fTrackVolume = GetCurrentSequence().GetTrack((CueLayer)i, MOOD_NEUTRAL).fWaveVolume;
			}
			ConVar *snd_musicvolume = cvar->FindVar("snd_musicvolume");
			fTrackVolume = (snd_musicvolume ? fTrackVolume * snd_musicvolume->GetFloat() : fTrackVolume);
			float fVolume = ((j == GetGlobalMood()) ? fTrackVolume : 0.01f);
			GUID guid = GetGuid((CueLayer)i, (CueMood)j);
			if (guid && enginesound->IsSoundStillPlaying(guid))
				enginesound->SetVolumeByGuid(guid, fVolume);
		}
	}
}


CueMood CueTrack::GetGlobalMood()
{
	return pTFCueBuilder->GetMood();
}

void CueTrack::Play()
{
	if (GetCurrentSeqID() >= GetSeqCount())
	{
		SetCurrentSeqID(-1);
		return;
	}

	bool bPlaying = IsStillPlaying();
	if (!bPlaying)
	{
		CueSequence Part = GetCurrentSequence();
		DevMsg("Playing part %s\n", Part.sName);

		for (int i = 0; i < LAYER_COUNT; i++)
		{
			for (int j = 0; j < MOOD_COUNT; j++)
			{
				PlayLayer(GetCurrentSeqID(), (CueLayer)i, (CueMood)j);
			}
		}
	}
	SetVolumes();
	m_fCurrentDuration = gpGlobals->curtime;
}

void CueTrack::Stop()
{
	for (int i = 0; i < LAYER_COUNT; i++)
	{
		for (int j = 0; j < MOOD_COUNT; j++)
		{
			enginesound->StopSoundByGuid(GetGuid((CueLayer)i, (CueMood)j));
		}
	}
}

void CueTrack::StopPlaying()
{
	m_bPlay = false;
	if (!IsStillPlaying())
		return;
	Stop();
}

void CueTrack::StartPlaying()
{
	m_bPlay = true;
}

bool CueTrack::IsStillPlaying()
{
	for (int j = 0; j < MOOD_COUNT; j++)
	{
		if (enginesound->IsSoundStillPlaying(GetGuid(LAYER_MAIN, (CueMood)j)))
			return true;
	}
	return false;
}

void CueTrack::SetGuid(GUID guid, CueLayer Layer, CueMood Mood)
{
	GUID id = (Layer * MOOD_COUNT) + Mood;
	m_pPlayList[id] = guid;
}

GUID CueTrack::GetGuid(CueLayer Layer, CueMood Mood)
{
	GUID id = (Layer * MOOD_COUNT) + Mood;
	return m_pPlayList[id];
}