//========= Copyright Valve Corporation, All rights reserved. =================//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_tf_viewmodeladdon.h"
#include "c_tf_player.h"
#include "tf_viewmodel.h"
#include "model_types.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

extern ConVar r_drawothermodels;

bool C_ViewmodelAttachmentModel::InitializeAsClientEntity( const char *pszModelName, RenderGroup_t renderGroup )
{
	if ( BaseClass::InitializeAsClientEntity( pszModelName, renderGroup ) )
	{
		AddEffects( EF_BONEMERGE | EF_BONEMERGE_FASTCULL );
		return true;
	}

	return false;
}

void C_ViewmodelAttachmentModel::SetViewmodel( C_TFViewModel *vm )
{
	m_viewmodel.Set( vm );
}

int C_ViewmodelAttachmentModel::InternalDrawModel( int flags )
{
	CMatRenderContextPtr pRenderContext( materials );

	if ( m_viewmodel->ShouldFlipViewModel() )
		pRenderContext->CullMode( MATERIAL_CULLMODE_CW );

	int ret = BaseClass::InternalDrawModel( flags );

	pRenderContext->CullMode( MATERIAL_CULLMODE_CCW );

	return ret;
}

//-----------------------------------------------------------------------------
// Purpose: We're overriding this because DrawModel keeps calling the FollowEntity DrawModel function
//			which in this case is CTFViewModel::DrawModel.
//			This is basically just a straight copy of C_BaseAnimating::DrawModel, without the FollowEntity part
//-----------------------------------------------------------------------------
int C_ViewmodelAttachmentModel::DrawOverriddenViewmodel( int flags )
{
	if ( !m_bReadyToDraw )
		return 0;

	int drawn = 0;

	ValidateModelIndex();

	if ( r_drawothermodels.GetInt() )
	{
		MDLCACHE_CRITICAL_SECTION();

		int extraFlags = 0;
		if ( r_drawothermodels.GetInt() == 2 )
		{
			extraFlags |= STUDIO_WIREFRAME;
		}

		if ( flags & STUDIO_SHADOWDEPTHTEXTURE )
		{
			extraFlags |= STUDIO_SHADOWDEPTHTEXTURE;
		}

		if ( flags & STUDIO_SSAODEPTHTEXTURE )
		{
			extraFlags |= STUDIO_SSAODEPTHTEXTURE;
		}

		// Necessary for lighting blending
		CreateModelInstance();

		drawn = InternalDrawModel( flags | extraFlags );
	}

	// If we're visualizing our bboxes, draw them
	DrawBBoxVisualizations();

	return drawn;
}


int C_ViewmodelAttachmentModel::DrawModel( int flags )
{
	if ( !IsVisible() )
		return 0;

	if ( m_viewmodel.Get() == NULL )
		return 0;

	C_TFPlayer *pLocalPlayer = C_TFPlayer::GetLocalTFPlayer();
	C_TFPlayer *pPlayer = ToTFPlayer( m_viewmodel.Get()->GetOwner() );

	if ( pLocalPlayer && pLocalPlayer->IsObserver()
		&& pLocalPlayer->GetObserverTarget() != m_viewmodel.Get()->GetOwner() )
		return false;

	if ( pLocalPlayer && !pLocalPlayer->IsObserver() && ( pLocalPlayer != pPlayer ) )
		return false;

	return BaseClass::DrawModel( flags );
}