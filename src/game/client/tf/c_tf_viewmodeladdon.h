//========= Copyright Valve Corporation, All rights reserved. =================//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_TF_VIEWMODELENT_H
#define C_TF_VIEWMODELENT_H

#include "cbase.h"
#include "c_baseanimating.h"
#include "c_baseviewmodel.h"

class C_TFViewModel;

class C_ViewmodelAttachmentModel : public C_BaseViewModel
{
	DECLARE_CLASS( C_ViewmodelAttachmentModel, C_BaseViewModel );
public:

	virtual bool InitializeAsClientEntity( const char *pszModelName, RenderGroup_t renderGroup );

	virtual int	InternalDrawModel( int flags );
	virtual int	DrawModel( int flags );
	virtual int DrawOverriddenViewmodel( int flags );

	void SetViewmodel( C_TFViewModel *vm );

	virtual bool			IsViewModel() const { return true; }
	virtual RenderGroup_t	GetRenderGroup( void ) { return RENDER_GROUP_VIEW_MODEL_TRANSLUCENT; }

	virtual C_BaseEntity	*GetItemTintColorOwner( void ) { return GetOwner(); }

private:
	CHandle< C_TFViewModel > m_viewmodel;
};

#endif