//=============================================================================
//
// Purpose: Stub class for compatibility with item schema
//
//=============================================================================
#ifndef TF_WEARABLE_H
#define TF_WEARABLE_H

#ifdef _WIN32
#pragma once
#endif

#include "econ_wearable.h"

#ifdef CLIENT_DLL
#define CTFWearable C_TFWearable
#endif

class CTFWearable : public CEconWearable
{
public:
	DECLARE_CLASS( CTFWearable, CEconWearable );
	DECLARE_NETWORKCLASS();

#ifdef GAME_DLL
	virtual void	Equip( CBasePlayer *pPlayer );
	void			UpdateModelToClass( void );
#endif
};

#endif // TF_WEARABLE_H
