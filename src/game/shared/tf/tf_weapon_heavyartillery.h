//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: A lightweight minigun variant for use in DM
//
//=============================================================================

#ifndef TF_WEAPON_HEAVYARTILLERY_H
#define TF_WEAPON_HEAVYARTILLERY_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase_gun.h"

#ifdef CLIENT_DLL
#include "particles_new.h"
#endif

// Client specific.
#ifdef CLIENT_DLL
#define CTFHeavyArtillery C_TFHeavyArtillery
#endif

class CTFHeavyArtillery : public CTFWeaponBaseGun
{
public:

	DECLARE_CLASS( CTFHeavyArtillery, CTFWeaponBaseGun );
	DECLARE_NETWORKCLASS();
	DECLARE_PREDICTABLE();

	CTFHeavyArtillery();
	~CTFHeavyArtillery();

	virtual int	GetWeaponID( void ) const	{ return TF_WEAPON_HEAVYARTILLERY; }

	virtual void PrimaryAttack( void );

	virtual int GetCustomDamageType() const { return TF_DMG_CUSTOM_MINIGUN; }

protected:
	float m_fLastAttack, m_fBaseAccuracy, m_fNextAccuracy;

private:

	void ClipPunchAngleOffset( QAngle &in, const QAngle &punch, const QAngle &clip );

};

#endif // TF_WEAPON_HEAVYARTILLERY_H
