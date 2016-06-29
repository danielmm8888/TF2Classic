#include "cbase.h"
#include "tf_wearable.h"

#ifdef GAME_DLL
#include "tf_player.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFWearable, DT_TFWearable );

BEGIN_NETWORK_TABLE( CTFWearable, DT_TFWearable )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_wearable, CTFWearable );
PRECACHE_REGISTER( tf_wearable );

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWearable::Equip( CBasePlayer *pPlayer )
{
	BaseClass::Equip( pPlayer );
	UpdateModelToClass();
}

//---------------------------------------------------------------------------- -
// Purpose: 
//-----------------------------------------------------------------------------
void CTFWearable::UpdateModelToClass( void )
{
	CTFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );

	if ( pOwner )
	{
		const char *pszModel = m_Item.GetPlayerDisplayModel( pOwner->GetPlayerClass()->GetClassIndex() );

		if ( pszModel[0] != '\0' )
		{
			SetModel( pszModel );
		}
	}
}

#endif // GAME_DLL
