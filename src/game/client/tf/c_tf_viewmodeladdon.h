//========= Copyright Valve Corporation, All rights reserved. =================//
//
// Purpose: 
//
//=============================================================================//

#ifndef C_TF_VIEWMODELENT_H
#define C_TF_VIEWMODELENT_H

#include "cbase.h"
#include "c_baseanimating.h"

class C_TFViewModel;

class C_TFViewmodelAddon : public C_BaseAnimating
{
	DECLARE_CLASS(C_TFViewmodelAddon, C_BaseAnimating);
public:

	virtual int	InternalDrawModel(int flags);

	void SetViewmodel(C_TFViewModel *vm);

	CHandle< C_TFViewModel > m_viewmodel;

	virtual bool			IsViewModel() const { return true; }

	virtual RenderGroup_t	GetRenderGroup(void) { return RENDER_GROUP_VIEW_MODEL_TRANSLUCENT; }

};

#endif