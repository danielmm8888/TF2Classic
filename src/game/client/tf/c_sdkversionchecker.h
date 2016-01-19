#ifndef TF_SDKCHECK_H
#define TF_SDKCHECK_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"
#include "igamesystem.h"
#include "GameEventListener.h"

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class C_SDKVersionChecker : public CAutoGameSystemPerFrame, public CGameEventListener
{
public:
	C_SDKVersionChecker();
	~C_SDKVersionChecker();

	// Methods of IGameSystem
	virtual bool Init();
	virtual char const *Name() { return "C_SDKVersionChecker"; }

	// Methods of CGameEventListener
	virtual void FireGameEvent(IGameEvent *event){}

	const char* GetKey();

private:

	bool		m_bInited;
};

C_SDKVersionChecker *GetSDKVersionChecker();
#endif // TF_CUEBUILDER_H
