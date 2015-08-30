//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "c_ai_basenpc.h"
#include "engine/ivdebugoverlay.h"

#if defined( HL2_DLL ) || defined( HL2_EPISODIC )
#include "c_basehlplayer.h"
#endif

#include "death_pose.h"

#ifdef TF_CLASSIC_CLIENT
#include "c_tf_player.h"
#include "tf_shareddefs.h"
#include "iclientmode.h"
#include "vgui/ILocalize.h"
#include "soundenvelope.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define PING_MAX_TIME	2.0

IMPLEMENT_CLIENTCLASS_DT( C_AI_BaseNPC, DT_AI_BaseNPC, CAI_BaseNPC )
	RecvPropInt( RECVINFO( m_lifeState ) ),
	RecvPropBool( RECVINFO( m_bPerformAvoidance ) ),
	RecvPropBool( RECVINFO( m_bIsMoving ) ),
	RecvPropBool( RECVINFO( m_bFadeCorpse ) ),
	RecvPropInt( RECVINFO ( m_iDeathPose) ),
	RecvPropInt( RECVINFO( m_iDeathFrame) ),
	RecvPropInt( RECVINFO( m_iHealth ) ),
	RecvPropInt( RECVINFO( m_iMaxHealth ) ),
	RecvPropInt( RECVINFO( m_iSpeedModRadius ) ),
	RecvPropInt( RECVINFO( m_iSpeedModSpeed ) ),
	RecvPropInt( RECVINFO( m_bSpeedModActive ) ),
	RecvPropBool( RECVINFO( m_bImportanRagdoll ) ),
	RecvPropFloat( RECVINFO( m_flTimePingEffect ) ),
	RecvPropString( RECVINFO( m_szClassname ) ),
#ifdef TF_CLASSIC_CLIENT
	RecvPropInt( RECVINFO( m_nPlayerCond ) ),
	RecvPropInt( RECVINFO( m_nNumHealers ) ),
	RecvPropBool( RECVINFO( m_bBurningDeath ) )
#endif
END_RECV_TABLE()

extern ConVar cl_npc_speedmod_intime;

bool NPC_IsImportantNPC( C_BaseAnimating *pAnimating )
{
	C_AI_BaseNPC *pBaseNPC = dynamic_cast < C_AI_BaseNPC* > ( pAnimating );

	if ( pBaseNPC == NULL )
		return false;

	return pBaseNPC->ImportantRagdoll();
}

C_AI_BaseNPC::C_AI_BaseNPC()
{
#ifdef TF_CLASSIC_CLIENT
	m_pBurningSound = NULL;
	m_pBurningEffect = NULL;
	m_flBurnEffectStartTime = 0;
	m_flBurnEffectEndTime = 0;
#endif
}

//-----------------------------------------------------------------------------
// Makes ragdolls ignore npcclip brushes
//-----------------------------------------------------------------------------
unsigned int C_AI_BaseNPC::PhysicsSolidMaskForEntity( void ) const 
{
	// This allows ragdolls to move through npcclip brushes
	if ( !IsRagdoll() )
	{
		return MASK_NPCSOLID; 
	}
	return MASK_SOLID;
}


void C_AI_BaseNPC::ClientThink( void )
{
	BaseClass::ClientThink();

#ifdef HL2_DLL
	C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer*>( C_BasePlayer::GetLocalPlayer() );

	if ( ShouldModifyPlayerSpeed() == true )
	{
		if ( pPlayer )
		{
			float flDist = (GetAbsOrigin() - pPlayer->GetAbsOrigin()).LengthSqr();

			if ( flDist <= GetSpeedModifyRadius() )
			{
				if ( pPlayer->m_hClosestNPC )
				{
					if ( pPlayer->m_hClosestNPC != this )
					{
						float flDistOther = (pPlayer->m_hClosestNPC->GetAbsOrigin() - pPlayer->GetAbsOrigin()).Length();

						//If I'm closer than the other NPC then replace it with myself.
						if ( flDist < flDistOther )
						{
							pPlayer->m_hClosestNPC = this;
							pPlayer->m_flSpeedModTime = gpGlobals->curtime + cl_npc_speedmod_intime.GetFloat();
						}
					}
				}
				else
				{
					pPlayer->m_hClosestNPC = this;
					pPlayer->m_flSpeedModTime = gpGlobals->curtime + cl_npc_speedmod_intime.GetFloat();
				}
			}
		}
	}
#endif // HL2_DLL

#ifdef HL2_EPISODIC
	C_BaseHLPlayer *pPlayer = dynamic_cast<C_BaseHLPlayer*>( C_BasePlayer::GetLocalPlayer() );

	if ( pPlayer && m_flTimePingEffect > gpGlobals->curtime )
	{
		float fPingEffectTime = m_flTimePingEffect - gpGlobals->curtime;
		
		if ( fPingEffectTime > 0.0f )
		{
			Vector vRight, vUp;
			Vector vMins, vMaxs;

			float fFade;

			if( fPingEffectTime <= 1.0f )
			{
				fFade = 1.0f - (1.0f - fPingEffectTime);
			}
			else
			{
				fFade = 1.0f;
			}

			GetRenderBounds( vMins, vMaxs );
			AngleVectors (pPlayer->GetAbsAngles(), NULL, &vRight, &vUp );
			Vector p1 = GetAbsOrigin() + vRight * vMins.x + vUp * vMins.z;
			Vector p2 = GetAbsOrigin() + vRight * vMaxs.x + vUp * vMins.z;
			Vector p3 = GetAbsOrigin() + vUp * vMaxs.z;

			int r = 0 * fFade;
			int g = 255 * fFade;
			int b = 0 * fFade;

			debugoverlay->AddLineOverlay( p1, p2, r, g, b, true, 0.05f );
			debugoverlay->AddLineOverlay( p2, p3, r, g, b, true, 0.05f );
			debugoverlay->AddLineOverlay( p3, p1, r, g, b, true, 0.05f );
		}
	}
#endif
}

void C_AI_BaseNPC::OnPreDataChanged( DataUpdateType_t updateType )
{
	BaseClass::OnPreDataChanged( updateType );

#ifdef TF_CLASSIC_CLIENT
	m_nOldConditions = m_nPlayerCond;
#endif
}

void C_AI_BaseNPC::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( ( ShouldModifyPlayerSpeed() == true ) || ( m_flTimePingEffect > gpGlobals->curtime ) )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}

#ifdef TF_CLASSIC_CLIENT
	if ( InCond( TF_COND_BURNING ) && !m_pBurningSound )
	{
		StartBurningSound();
	}

	// Update conditions from last network change
	if ( m_nOldConditions != m_nPlayerCond )
	{
		UpdateConditions();

		m_nOldConditions = m_nPlayerCond;
	}
#endif
}

void C_AI_BaseNPC::UpdateOnRemove( void )
{
#ifdef TF_CLASSIC_CLIENT
	ParticleProp()->OwnerSetDormantTo( true );
	ParticleProp()->StopParticlesInvolving( this );

	RemoveAllCond();
#endif

	BaseClass::UpdateOnRemove();
}

void C_AI_BaseNPC::GetRagdollInitBoneArrays( matrix3x4_t *pDeltaBones0, matrix3x4_t *pDeltaBones1, matrix3x4_t *pCurrentBones, float boneDt )
{
	ForceSetupBonesAtTime( pDeltaBones0, gpGlobals->curtime - boneDt );
	GetRagdollCurSequenceWithDeathPose( this, pDeltaBones1, gpGlobals->curtime, m_iDeathPose, m_iDeathFrame );
	float ragdollCreateTime = PhysGetSyncCreateTime();
	if ( ragdollCreateTime != gpGlobals->curtime )
	{
		// The next simulation frame begins before the end of this frame
		// so initialize the ragdoll at that time so that it will reach the current
		// position at curtime.  Otherwise the ragdoll will simulate forward from curtime
		// and pop into the future a bit at this point of transition
		ForceSetupBonesAtTime( pCurrentBones, ragdollCreateTime );
	}
	else
	{
		SetupBones( pCurrentBones, MAXSTUDIOBONES, BONE_USED_BY_ANYTHING, gpGlobals->curtime );
	}
}

#ifdef TF_CLASSIC_CLIENT
C_BaseAnimating *C_AI_BaseNPC::BecomeRagdollOnClient()
{
	C_BaseAnimating *pRagdoll = BaseClass::BecomeRagdollOnClient();
	if ( pRagdoll && m_bBurningDeath )
	{
		pRagdoll->ParticleProp()->Create( "burningplayer_corpse", PATTACH_ABSORIGIN_FOLLOW );
	}

	return pRagdoll;
}

//-----------------------------------------------------------------------------
// Purpose: check the newly networked conditions for changes
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::UpdateConditions( void )
{
	int nCondChanged = m_nPlayerCond ^ m_nOldConditions;
	int nCondAdded = nCondChanged & m_nPlayerCond;
	int nCondRemoved = nCondChanged & m_nOldConditions;

	int i;
	for ( i=0;i<TF_COND_LAST;i++ )
	{
		if ( nCondAdded & (1<<i) )
		{
			OnConditionAdded( i );
		}
		else if ( nCondRemoved & (1<<i) )
		{
			OnConditionRemoved( i );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::GetTargetIDString( wchar_t *sIDString, int iMaxLenInBytes )
{
	sIDString[0] = '\0';

	C_TFPlayer *pLocalTFPlayer = C_TFPlayer::GetLocalTFPlayer();
	
	if ( !pLocalTFPlayer )
		return;

	if ( InSameTeam( pLocalTFPlayer ) || pLocalTFPlayer->IsPlayerClass( TF_CLASS_SPY ) || pLocalTFPlayer->GetTeamNumber() == TEAM_SPECTATOR )
	{
		const char *pszClassname = GetClassname();
		wchar_t *wszNPCName;
		wszNPCName = g_pVGuiLocalize->Find( pszClassname );

		if ( !wszNPCName )
		{
			wchar_t wszNPCNameBuf[MAX_PLAYER_NAME_LENGTH];
			g_pVGuiLocalize->ConvertANSIToUnicode( pszClassname, wszNPCNameBuf, sizeof(wszNPCNameBuf) );
			wszNPCName = wszNPCNameBuf;
		}

		const char *printFormatString = NULL;

		if ( pLocalTFPlayer->GetTeamNumber() == TEAM_SPECTATOR || InSameTeam( pLocalTFPlayer ) )
		{
			printFormatString = "#TF_playerid_sameteam";
		}
		else if ( pLocalTFPlayer->IsPlayerClass( TF_CLASS_SPY ) )
		{
			// Spy can see enemy's health.
			printFormatString = "#TF_playerid_diffteam";
		}

		wchar_t *wszPrepend = L"";

		if ( printFormatString )
		{
			g_pVGuiLocalize->ConstructString( sIDString, iMaxLenInBytes, g_pVGuiLocalize->Find(printFormatString), 3, wszPrepend, wszNPCName );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::StartBurningSound( void )
{
	CSoundEnvelopeController &controller = CSoundEnvelopeController::GetController();

	if ( !m_pBurningSound )
	{
		CLocalPlayerFilter filter;
		m_pBurningSound = controller.SoundCreate( filter, entindex(), "Player.OnFire" );
	}

	controller.Play( m_pBurningSound, 0.0, 100 );
	controller.SoundChangeVolume( m_pBurningSound, 1.0, 0.1 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void C_AI_BaseNPC::StopBurningSound( void )
{
	if ( m_pBurningSound )
	{
		CSoundEnvelopeController::GetController().SoundDestroy( m_pBurningSound );
		m_pBurningSound = NULL;
	}
}
#endif
