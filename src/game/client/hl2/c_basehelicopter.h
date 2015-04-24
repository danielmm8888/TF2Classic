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
class C_BaseHelicopterHL1 : public C_AI_BaseNPC
{
public:
	DECLARE_CLASS(C_BaseHelicopterHL1, C_AI_BaseNPC);
	DECLARE_CLIENTCLASS();

	C_BaseHelicopterHL1();

	float StartupTime() const { return m_flStartupTime; }

private:
	C_BaseHelicopterHL1(const C_BaseHelicopterHL1 &other) {}
	float m_flStartupTime;
};
#endif // TF_CLASSIC_CLIENT
#endif // C_BASEHELICOPTER_H
