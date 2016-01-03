#include "cbase.h"
#include "tf_wearable.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

IMPLEMENT_NETWORKCLASS_ALIASED( TFWearable, DT_TFWearable );

BEGIN_NETWORK_TABLE( CTFWearable, DT_TFWearable )
END_NETWORK_TABLE()

LINK_ENTITY_TO_CLASS( tf_wearable, CTFWearable );
PRECACHE_REGISTER( tf_wearable );
