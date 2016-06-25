#ifndef TF_MUSIC_MANAGER_H
#define TF_MUSIC_MANAGER_H

#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "GameEventListener.h"
#include "soundenvelope.h"

#define TF_MUSIC_LAYERS 4
#define TF_MUSIC_INTENSITY_FRACTION ( 1.0f / TF_MUSIC_LAYERS )

struct SMusicTrack
{
	SMusicTrack()
	{
		bPlay = false;
		pSound = NULL;
	}

	bool bPlay;
	CSoundPatch *pSound;
};

class CHudMusicManager;

class CTFMusicManager : public CAutoGameSystemPerFrame, public CGameEventListener
{
public:
	CTFMusicManager();
	~CTFMusicManager();

	friend class CHudMusicManager;

	// Methods of IGameSystem
	virtual char const *Name() { return "CTFMusicManager"; }
	virtual bool Init( void );
	virtual void LevelShutdownPreEntity( void );
	virtual void Update( float flFrameTime );

	// Methods of CGameEventListener
	virtual void FireGameEvent( IGameEvent *event );

	void StartMusic( void );
	void StopMusic( bool bPlayEnding = false );
	bool IsPlayingMusic( void ) { return m_bPlaying; }
	int GetMusicPower( void );
	bool CanPlayMusic( void );

private:
	SMusicTrack m_Tracks[TF_MUSIC_LAYERS];
	bool m_bPlaying;
	float m_flDuration;
	float m_flLoopTime;
	
	int m_iTrack;
	float m_flIntensity;
	float m_flPlayerIntensity;
};

extern CTFMusicManager *GetTFMusicManager( void );

#endif // TF_MUSIC_MANAGER_H
