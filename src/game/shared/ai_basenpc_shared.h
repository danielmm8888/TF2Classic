//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef AI_BASENPC_SHARED_H
#define AI_BASENPC_SHARED_H
#ifdef _WIN32
#pragma once
#endif

#if defined( CLIENT_DLL )
#define CAI_BaseNPC C_AI_BaseNPC
#endif

class CAI_BaseNPC;
typedef CHandle<CAI_BaseNPC>	NPCHandle;

// Shared header file for players
#if defined( CLIENT_DLL )
#include "c_ai_basenpc.h"
#else
#include "ai_basenpc.h"
#endif

#endif // AI_BASENPC_SHARED_H
