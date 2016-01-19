#ifndef TF_CUEBUILDER_H
#define TF_CUEBUILDER_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"
#include "igamesystem.h"
#include "GameEventListener.h"

#define GUID int
class CTFCueBuilder;

enum CueMood
{
	MOOD_NEUTRAL,
	MOOD_DANGER,
	MOOD_COUNT,
};

enum CueLayer
{
	LAYER_MAIN,
	LAYER_MISC,
	LAYER_PERC,
	LAYER_COUNT
};

struct CueWaveInfo
{
	char sWaveName[64];
	float fWaveVolume;
};

struct CueSequence
{
	int	id;
	int pitch;
	char sName[64];
	float skipmultiplier;
	soundlevel_t soundlevel;
	CueWaveInfo pTracks[MOOD_COUNT * LAYER_COUNT];
	void AddTrack(const char* sTrack, float iVolume = 1.0f, CueLayer Layer = LAYER_MAIN, CueMood Mood = MOOD_NEUTRAL)
	{
		int id = (Layer * MOOD_COUNT) + Mood;
		Q_strncpy(pTracks[id].sWaveName, sTrack, sizeof(pTracks[id].sWaveName));
		pTracks[id].fWaveVolume = iVolume;
	};
	CueWaveInfo GetTrack(CueLayer Layer = LAYER_MAIN, CueMood Mood = MOOD_NEUTRAL)
	{
		int id = (Layer * MOOD_COUNT) + Mood;
		return pTracks[id];
	};
};

class CueTrack
{
public:
	CueTrack(CTFCueBuilder *pCueBuilder, const char* sName);

	void Update();

	GUID PlayLayer(int ID, CueLayer Layer = LAYER_MAIN, CueMood Mood = MOOD_NEUTRAL);
	void Play();
	void Stop();

	void StartPlaying();
	void StopPlaying();

	void SetVolumes();
	bool IsStillPlaying();

	void SetGuid(GUID guid, CueLayer Layer = LAYER_MAIN, CueMood Mood = MOOD_NEUTRAL);
	GUID GetGuid(CueLayer Layer = LAYER_MAIN, CueMood Mood = MOOD_NEUTRAL);

	void SetCurrentSeqID(int ID) { m_iCurrentSequence = ID; };
	int	 GetCurrentSeqID() { return m_iCurrentSequence; };
	void NextSeq() { m_iCurrentSequence++; };
	void PrevSeq() { m_iCurrentSequence--; };

	void AddSequence(CueSequence pSequence, const char* name);
	CueSequence GetSequence(int ID);
	CueSequence GetCurrentSequence();
	int GetSeqCount();

	bool GetShouldSkip();
	void SetShouldSkip(bool bSkip);

	CueMood GetGlobalMood();
	char*	GetTrackName() { return m_sName; };

private:
	CTFCueBuilder	 *pTFCueBuilder;
	CUtlDict< CueSequence, unsigned short > m_TrackInfoDatabase;
	GUID m_pPlayList[MOOD_COUNT * LAYER_COUNT];

	bool		m_bPlay;
	int			m_iCurrentSequence;
	float		m_fCurrentDuration;
	char		m_sName[64];
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFCueBuilder : public CAutoGameSystemPerFrame, public CGameEventListener
{
public:
	CTFCueBuilder();
	~CTFCueBuilder();

	// Methods of IGameSystem
	virtual bool Init();
	virtual char const *Name() { return "CTFCueBuilder"; }
	// Gets called each frame
	virtual void Update(float frametime);

	// Methods of CGameEventListener
	virtual void FireGameEvent(IGameEvent *event);

	//
	void SetMood(CueMood mood);
	CueMood GetMood() { return m_iGlobalMood; }
	void AddTrack(const char* name, CueTrack* pCueTrack);

	void SetCurrentTrack(int ID) { m_iCurrentTrack = ID; };
	void SetCurrentTrack(const char* sName) { m_iCurrentTrack = m_PlaylistDatabase.Find(sName); };
	int	 GetCurrentTrackID() { return m_iCurrentTrack; };
	char* GetCurrentTrackName() { return m_PlaylistDatabase[m_iCurrentTrack]->GetTrackName(); };
	bool GetShouldSkip() { return m_bShouldSkipTrack; };
	void SetShouldSkip(bool bSkip) { m_bShouldSkipTrack = bSkip; };

	void StartCue();
	void ResetAndStartCue();
	void StopCue();

	CueTrack*	 GetCurrentTrack() { return m_PlaylistDatabase[m_iCurrentTrack]; };

private:
	CUtlDict< CueTrack*, unsigned short > m_PlaylistDatabase;

	bool		m_bInited;
	bool		m_bShouldSkipTrack;
	int			m_iCurrentTrack;
	CueMood		m_iGlobalMood;
};

CTFCueBuilder *GetCueBuilder();
#endif // TF_CUEBUILDER_H
