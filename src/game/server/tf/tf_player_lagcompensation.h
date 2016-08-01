#ifndef TF_PLAYER_LAGCOMPENSATION_H
#define TF_PLAYER_LAGCOMPENSATION_H

#ifdef _WIN32
#pragma once
#endif

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
#define MAX_LAYER_RECORDS (CBaseAnimatingOverlay::MAX_OVERLAYS)

struct LayerRecord
{
	int m_sequence;
	float m_cycle;
	float m_weight;
	int m_order;

	LayerRecord()
	{
		m_sequence = 0;
		m_cycle = 0;
		m_weight = 0;
		m_order = 0;
	}

	LayerRecord( const LayerRecord& src )
	{
		m_sequence = src.m_sequence;
		m_cycle = src.m_cycle;
		m_weight = src.m_weight;
		m_order = src.m_order;
	}
};

struct LagRecord
{
public:
	LagRecord()
	{
		m_fFlags = 0;
		m_vecOrigin.Init();
		m_vecAngles.Init();
		m_vecMins.Init();
		m_vecMaxs.Init();
		m_flSimulationTime = -1;
		m_masterSequence = 0;
		m_masterCycle = 0;
	}

	LagRecord( const LagRecord& src )
	{
		m_fFlags = src.m_fFlags;
		m_vecOrigin = src.m_vecOrigin;
		m_vecAngles = src.m_vecAngles;
		m_vecMins = src.m_vecMins;
		m_vecMaxs = src.m_vecMaxs;
		m_flSimulationTime = src.m_flSimulationTime;
		for ( int layerIndex = 0; layerIndex < MAX_LAYER_RECORDS; ++layerIndex )
		{
			m_layerRecords[layerIndex] = src.m_layerRecords[layerIndex];
		}
		m_masterSequence = src.m_masterSequence;
		m_masterCycle = src.m_masterCycle;
	}

	// Did player die this frame
	int						m_fFlags;

	// Player position, orientation and bbox
	Vector					m_vecOrigin;
	QAngle					m_vecAngles;
	Vector					m_vecMins;
	Vector					m_vecMaxs;

	float					m_flSimulationTime;

	// Player animation details, so we can get the legs in the right spot.
	LayerRecord				m_layerRecords[MAX_LAYER_RECORDS];
	int						m_masterSequence;
	float					m_masterCycle;
};

#endif // TF_PLAYER_LAGCOMPENSATION_H
