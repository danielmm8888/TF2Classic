//=============================================================================
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "c_tf_spymask.h"
#include "c_tf_player.h"

C_TFSpyMask::C_TFSpyMask()
{
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool C_TFSpyMask::InitializeAsClientEntity( const char *pszModelName, RenderGroup_t renderGroup )
{
	if ( BaseClass::InitializeAsClientEntity( pszModelName, renderGroup ) )
	{
		AddEffects( EF_BONEMERGE | EF_NOSHADOW | EF_BONEMERGE_FASTCULL );
		
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool C_TFSpyMask::ShouldDraw( void )
{
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );

	if ( !pOwner )
		return false;

	if ( pOwner->IsEnemyPlayer() )
		return false;

	if ( !pOwner->ShouldDrawThisPlayer() )
		return false;

	return BaseClass::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int C_TFSpyMask::GetSkin( void )
{
	C_TFPlayer *pOwner = ToTFPlayer( GetOwnerEntity() );
	
	if ( pOwner && pOwner->m_Shared.InCond( TF_COND_DISGUISED ) )
	{
		return ( pOwner->m_Shared.GetDisguiseClass() - 1 );
	}

	return 0;
}
