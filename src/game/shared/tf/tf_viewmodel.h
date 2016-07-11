//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef TF_VIEWMODEL_H
#define TF_VIEWMODEL_H
#ifdef _WIN32
#pragma once
#endif

#include "predictable_entity.h"
#include "utlvector.h"
#include "baseplayer_shared.h"
#include "shared_classnames.h"
#include "tf_weaponbase.h"

#ifdef CLIENT_DLL
#include "c_tf_viewmodeladdon.h"
#endif

#if defined( CLIENT_DLL )
#define CTFViewModel C_TFViewModel
#endif

enum
{
	VMTYPE_NONE = -1,	// Hasn't been set yet. We should never have this.
	VMTYPE_HL2,			// HL2-Type vmodels. Hands, weapon and anims are in the same model. (Used in HL1, HL2, CS:S, DOD:S, pretty much any old-gen Valve game)
	VMTYPE_TF2,			// TF2-Type cmodels. Hands are a separate model, anims are in the hands model. (Only used in live TF2)
	VMTYPE_L4D			// L4D-Type vmodels. Hands are a separate model, anims are in the weapon model. (Used in L4D, L4D2, Portal 2, CS:GO)
};

class CTFViewModel : public CBaseViewModel
{
	DECLARE_CLASS( CTFViewModel, CBaseViewModel );
public:

	DECLARE_NETWORKCLASS();

	CTFViewModel( void );
	virtual ~CTFViewModel( void );

	virtual void CalcViewModelLag( Vector& origin, QAngle& angles, QAngle& original_angles );
	virtual void CalcViewModelView( CBasePlayer *owner, const Vector& eyePosition, const QAngle& eyeAngles );
	virtual void AddViewModelBob( CBasePlayer *owner, Vector& eyePosition, QAngle& eyeAngles );

	virtual void SetWeaponModel( const char *pszModelname, CBaseCombatWeapon *weapon );

	int GetViewModelType( void ){ return m_iViewModelType;}
	void SetViewModelType( int iType ){ this->m_iViewModelType = iType; }

#if defined( CLIENT_DLL )
	virtual bool ShouldPredict( void )
	{
		if ( GetOwner() && GetOwner() == C_BasePlayer::GetLocalPlayer() )
			return true;

		return BaseClass::ShouldPredict();
	}

	virtual void StandardBlendingRules( CStudioHdr *hdr, Vector pos[], Quaternion q[], float currentTime, int boneMask );
	virtual void ProcessMuzzleFlashEvent( void );

	void UpdateViewModel();

	virtual bool ShouldReceiveProjectedTextures( int flags ) { return true; }

	virtual int GetSkin();
	BobState_t	&GetBobState() { return m_BobState; }

	virtual int DrawModel( int flags );

	CHandle< C_ViewmodelAttachmentModel > m_hViewmodelAddon;

	void UpdateViewmodelAddon( const char *pszModelname );

	void RemoveViewmodelAddon( void );

	// Attachments
	virtual int				LookupAttachment( const char *pAttachmentName );
	virtual bool			GetAttachment( int number, matrix3x4_t &matrix );
	virtual bool			GetAttachment( int number, Vector &origin );
	virtual	bool			GetAttachment( int number, Vector &origin, QAngle &angles );
	virtual bool			GetAttachmentVelocity( int number, Vector &originVel, Quaternion &angleVel );

	virtual void			FireEvent( const Vector& origin, const QAngle& angles, int event, const char *options );

	virtual C_BaseEntity	*GetItemTintColorOwner( void ) { return GetOwner(); }

#endif

private:

#if defined( CLIENT_DLL )

	// This is used to lag the angles.
	CInterpolatedVar<QAngle> m_LagAnglesHistory;
	QAngle m_vLagAngles;
	BobState_t		m_BobState;		// view model head bob state

	CTFViewModel( const CTFViewModel & ); // not defined, not accessible

	QAngle m_vLoweredWeaponOffset;

#endif

	int m_iViewModelType;

};

#endif // TF_VIEWMODEL_H
