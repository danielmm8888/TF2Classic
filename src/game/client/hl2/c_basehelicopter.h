//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_BASEHELICOPTER_H
#define C_BASEHELICOPTER_H
#ifdef _WIN32
#pragma once
#endif


#include "c_ai_basenpc.h"


class C_BaseHelicopter : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS( C_BaseHelicopter, C_AI_BaseNPC );
	DECLARE_CLIENTCLASS();

	C_BaseHelicopter();

	float StartupTime() const { return m_flStartupTime; }

private:
	C_BaseHelicopter( const C_BaseHelicopter &other ) {}
	float m_flStartupTime;
};

#ifdef TF_CLASSIC_CLIENT
class C_HL1BaseHelicopter : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_HL1BaseHelicopter, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	C_HL1BaseHelicopter();

	float StartupTime() const { return m_flStartupTime; }

private:
	C_HL1BaseHelicopter(const C_HL1BaseHelicopter &other) {}
	float m_flStartupTime;
};
#endif // TF_CLASSIC_CLIENT
#endif // C_BASEHELICOPTER_H
