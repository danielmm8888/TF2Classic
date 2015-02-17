//======
//
// Purpose: 
//
//=============================================================================

#ifndef TF_WEAPON_BACKPACK_H
#define TF_WEAPON_BACKPACK_H
#ifdef _WIN32
#pragma once
#endif

#include "tf_weaponbase.h"

#ifdef CLIENT_DLL
#define CTFBackpack C_TFBackpack
#endif

enum
{
	HealthKit,
	AmmoKit
};

//=============================================================================
//
// Backpack class.
//
class CTFBackpack : public CTFWeaponBase
{
public:

	DECLARE_CLASS(CTFBackpack, CTFWeaponBase);
	DECLARE_NETWORKCLASS(); 
	DECLARE_PREDICTABLE();

	CTFBackpack();
	virtual void PrimaryAttack();
	virtual int	GetWeaponID( void ) const { return TF_WEAPON_BACKPACK; }
	bool CanPickup(void){ return (cEntityName[0] == '\0') ? true : false; }
	void AddNewEntity(const char* cNewEntity) { cEntityName = cNewEntity; };
	void AddNewEntityByType(int iType, int iSize);
	const char *GetStatus(void);
	int GetEntityType(void) { return iEntityType; };
	int GetEntitySize(void) { return iEntitySize; };
	

private:

	const char* cEntityName;
	int iEntityType;
	int iEntitySize;
	static const char* HealthEntities[3];
	static const char* AmmoEntities[3];
	CTFBackpack( const CTFBackpack & ) {}
};

#endif // TF_WEAPON_Backpack_H
