//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef C_AI_BASENPC_H
#define C_AI_BASENPC_H
#ifdef _WIN32
#pragma once
#endif

#include "c_basecombatcharacter.h"

#ifdef TF_CLASSIC_CLIENT
#include "ai_basenpc_shared.h"
#include "tf_shareddefs.h"
#endif

// NOTE: Moved all controller code into c_basestudiomodel
class C_AI_BaseNPC : public C_BaseCombatCharacter
{
	DECLARE_CLASS( C_AI_BaseNPC, C_BaseCombatCharacter );

public:
	DECLARE_CLIENTCLASS();

	C_AI_BaseNPC();
	virtual unsigned int	PhysicsSolidMaskForEntity( void ) const;
	virtual bool			IsNPC( void ) { return true; }
	bool					IsMoving( void ){ return m_bIsMoving; }
	bool					ShouldAvoidObstacle( void ){ return m_bPerformAvoidance; }
	virtual bool			AddRagdollToFadeQueue( void ) { return m_bFadeCorpse; }

	virtual bool			GetRagdollInitBoneArrays( matrix3x4_t *pDeltaBones0, matrix3x4_t *pDeltaBones1, matrix3x4_t *pCurrentBones, float boneDt );

	int						GetDeathPose( void ) { return m_iDeathPose; }

	bool					ShouldModifyPlayerSpeed( void ) { return m_bSpeedModActive;	}
	int						GetSpeedModifyRadius( void ) { return m_iSpeedModRadius; }
	int						GetSpeedModifySpeed( void ) { return m_iSpeedModSpeed;	}

	void					ClientThink( void );
	virtual void			OnPreDataChanged( DataUpdateType_t updateType );
	virtual void			OnDataChanged( DataUpdateType_t type );
	bool					ImportantRagdoll( void ) { return m_bImportanRagdoll;	}
	virtual void			UpdateOnRemove( void );

	virtual int				GetHealth() const { return m_iHealth; }
	void					SetHealth( int health ) { m_iHealth = health; }
	virtual int				GetMaxHealth() const { return m_iMaxHealth; }

	const char				*GetClassname( void ) { return m_szClassname; }

#ifdef TF_CLASSIC_CLIENT
	virtual int				InternalDrawModel( int flags );
	virtual void AddDecal( const Vector& rayStart, const Vector& rayEnd,
		const Vector& decalCenter, int hitbox, int decalIndex, bool doTrace, trace_t& tr, int maxLODToDecal = ADDDECAL_TO_ALL_LODS );

	virtual C_BaseAnimating	*BecomeRagdollOnClient();
	virtual IRagdoll		*GetRepresentativeRagdoll() const;
	virtual Vector			GetObserverCamOrigin( void );

	virtual void			GetTargetIDString( wchar_t *sIDString, int iMaxLenInBytes );
	virtual void			GetTargetIDDataString( wchar_t *sDataString, int iMaxLenInBytes );

	int						GetNumHealers( void ) { return m_nNumHealers; }
	int						GetMaxBuffedHealth( void );

	// TF2 conditions
	int		GetCond() const						{ return m_nPlayerCond; }
	void	SetCond( int nCond )				{ m_nPlayerCond = nCond; }
	void	AddCond( int nCond, float flDuration = PERMANENT_CONDITION );
	void	RemoveCond( int nCond );
	bool	InCond( int nCond );
	void	RemoveAllCond( void );
	void	OnConditionAdded( int nCond );
	void	OnConditionRemoved( int nCond );
	//void	ConditionThink( void );
	float	GetConditionDuration( int nCond );

	void	UpdateConditions( void );

	virtual bool IsOnFire() { return InCond( TF_COND_BURNING ); }
	void	OnAddBurning( void );
	void	OnRemoveBurning( void );

	void	StartBurningSound( void );
	void	StopBurningSound( void );

	CMaterialReference *GetInvulnMaterialRef( void ) { return &m_InvulnerableMaterial; }
	void	InitInvulnerableMaterial( void );

	bool	AllowBackstab( void ) { return ( m_nTFFlags & TFFL_NOBACKSTAB ) == 0; }
	bool	IsMech( void ) { return ( m_nTFFlags & TFFL_MECH ) != 0; }
	bool	CanBeHealed( void ) { return ( m_nTFFlags & TFFL_NOHEALING ) == 0; }
#endif

private:
	C_AI_BaseNPC( const C_AI_BaseNPC & ); // not defined, not accessible
	float m_flTimePingEffect;
	int  m_iDeathPose;
	int	 m_iDeathFrame;

	int	 m_iHealth;
	int	 m_iMaxHealth;

	int m_iSpeedModRadius;
	int m_iSpeedModSpeed;

	bool m_bPerformAvoidance;
	bool m_bIsMoving;
	bool m_bFadeCorpse;
	bool m_bSpeedModActive;
	bool m_bImportanRagdoll;

	char m_szClassname[128];

#ifdef TF_CLASSIC_CLIENT
	int m_iOldTeam;

	// Conditions
	int m_nPlayerCond;
	int m_nOldConditions;
	float m_flCondExpireTimeLeft[TF_COND_LAST];
	int m_nNumHealers;

	// Burning
	CSoundPatch			*m_pBurningSound;
	CNewParticleEffect	*m_pBurningEffect;
	float				m_flBurnEffectStartTime;
	float				m_flBurnEffectEndTime;
	bool				m_bBurningDeath;

	// Ragdoll
	EHANDLE				m_hRagdoll;

	CMaterialReference	m_InvulnerableMaterial;

	// Flags
	int m_nTFFlags;
#endif
};


#endif // C_AI_BASENPC_H
