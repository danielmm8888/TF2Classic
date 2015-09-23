//=============================================================================//
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

#if defined(TF_CLASSIC) || defined(TF_CLASSIC_CLIENT)
//-------------------------------------
// TF2 flags
//-------------------------------------
#define TFFL_ALLOW_BACKSTAB				( 1 << 0 )
#define TFFL_MECH						( 1 << 1 ) // Can't be healed, don't make "flesh hit" melee sound, show "gears" icon in Target ID
#endif

// Shared header file for NPCs
#if defined( CLIENT_DLL )
#include "c_ai_basenpc.h"
#else
#include "ai_basenpc.h"
#endif

#endif // AI_BASENPC_SHARED_H
