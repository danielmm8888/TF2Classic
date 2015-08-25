//========= Copyright Valve Corporation, All rights reserved. =================//
//
// Purpose: 
//
//=============================================================================//

#include "cbase.h"
#include "c_tf_viewmodeladdon.h"
#include "c_tf_player.h"
#include "tf_viewmodel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void C_TFViewmodelAddon::SetViewmodel(C_TFViewModel *vm)
{
	m_viewmodel.Set(vm);
}

int C_TFViewmodelAddon::InternalDrawModel(int flags)
{
	CMatRenderContextPtr pRenderContext(materials);

	if (m_viewmodel->ShouldFlipViewModel())
		pRenderContext->CullMode(MATERIAL_CULLMODE_CW);

	int ret = BaseClass::InternalDrawModel(flags);

	pRenderContext->CullMode(MATERIAL_CULLMODE_CCW);

	return ret;
}

int C_TFViewmodelAddon::DrawModel( int flags )
{
	if ( !IsVisible() )
		return 0;

	if (m_viewmodel.Get() == NULL)
		return 0;

	C_BasePlayer *localplayer = C_BasePlayer::GetLocalPlayer();

	if ( localplayer && localplayer->IsObserver() 
		&& localplayer->GetObserverTarget() != m_viewmodel.Get()->GetOwner() )
		return false;

	if ( localplayer && !localplayer->IsObserver() && ( localplayer != m_viewmodel.Get()->GetOwner() ) )
		return false;

	return BaseClass::DrawModel( flags );
}