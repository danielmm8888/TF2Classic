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
	TF_BACKPACK_HEALTHKIT,
	TF_BACKPACK_AMMOKIT,
	//add new here

	TF_BACKPACK_COUNT		
};

#define TF_BACKPACK_TAKE_SOUND	"Backback.Take"
#define TF_BACKPACK_DROP_SOUND	"Backback.Drop"
#define TF_BACKPACK_SIZES		3

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
	virtual void Precache();

private:

	const char* cEntityName;
	static const char* Entities[TF_BACKPACK_COUNT][TF_BACKPACK_SIZES];
	CTFBackpack( const CTFBackpack & ) {}
};

#endif // TF_WEAPON_Backpack_H
