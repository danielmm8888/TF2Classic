//====== Copyright © 1996-2005, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_weapon_ubersaw.h"
#include "tf_weapon_medigun.h"
#include "tf_viewmodel.h"
#ifdef GAME_DLL
#include "tf_player.h"
#else
#include "c_tf_player.h"
#endif

//=============================================================================
//
// Weapon Ubersaw tables.
//

IMPLEMENT_NETWORKCLASS_ALIASED( TFUbersaw, DT_TFWeaponUbersaw )

BEGIN_NETWORK_TABLE( CTFUbersaw, DT_TFWeaponUbersaw )
END_NETWORK_TABLE()

BEGIN_PREDICTION_DATA( CTFUbersaw )
END_PREDICTION_DATA()

LINK_ENTITY_TO_CLASS( tf_weapon_ubersaw, CTFUbersaw );
PRECACHE_WEAPON_REGISTER( tf_weapon_ubersaw );

void CTFUbersaw::ItemPostFrame( void )
{
	CTFPlayer *pOwner = GetTFPlayerOwner();
	if ( !pOwner )
		return;

	BaseClass::ItemPostFrame();

	CWeaponMedigun *pMedigun = pOwner->GetMedigun();
	if ( pMedigun )
	{
		SetPoseParameter( "syringe_charge_level", pMedigun->GetChargeLevel() );

		CTFViewModel *vm = dynamic_cast< CTFViewModel * >( pOwner->GetViewModel( m_nViewModelIndex ) );
		if ( vm )
			vm->SetPoseParameter( "syringe_charge_level", pMedigun->GetChargeLevel() );
	}

}
