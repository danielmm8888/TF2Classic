//====== Copyright © 1996-2006, Valve Corporation, All rights reserved. =======//
//
// Purpose: 
//
//=============================================================================//

#ifndef TF_GENERIC_BOMB_H
#define TF_GENERIC_BOMB_H
#ifdef _WIN32
#pragma once
#endif

#include "props.h"

//=============================================================================
//
// TF Generic Bomb Class
//

class CTFGenericBomb : public CBreakableProp
{
public:
	DECLARE_CLASS( CTFGenericBomb, CBreakableProp );
	DECLARE_DATADESC();

	CTFGenericBomb();
	virtual void		Precache( void );

	virtual void		Spawn( void );
	
	virtual int			OnTakeDamage( const CTakeDamageInfo &info );

	virtual void		Event_Killed( const CTakeDamageInfo &info );

	virtual void		InputDetonate( inputdata_t &inputdata );

protected:
	float				m_flDamage;
	float				m_flRadius;
	string_t			m_iszParticleName;
	string_t			m_iszExplodeSound;
	bool				m_bFriendlyFire;

	COutputEvent		m_OnDetonate;
};


#endif // TF_GENERIC_BOMB_H