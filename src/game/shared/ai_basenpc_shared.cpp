//=============================================================================//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "ai_basenpc_shared.h"
#if defined(TF_CLASSIC) || defined(TF_CLASSIC_CLIENT)
#include "tf_shareddefs.h"
#endif

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#if defined(TF_CLASSIC) || defined(TF_CLASSIC_CLIENT)
extern ConVar tf_max_health_boost;

//-----------------------------------------------------------------------------
// Purpose: Add a condition and duration
// duration of PERMANENT_CONDITION means infinite duration
//-----------------------------------------------------------------------------
void CAI_BaseNPC::AddCond( int nCond, float flDuration /* = PERMANENT_CONDITION */ )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );
	m_nPlayerCond |= (1<<nCond);
	m_flCondExpireTimeLeft[nCond] = flDuration;
	OnConditionAdded( nCond );
}

//-----------------------------------------------------------------------------
// Purpose: Forcibly remove a condition
//-----------------------------------------------------------------------------
void CAI_BaseNPC::RemoveCond( int nCond )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	m_nPlayerCond &= ~(1<<nCond);
	m_flCondExpireTimeLeft[nCond] = 0;

	OnConditionRemoved( nCond );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
bool CAI_BaseNPC::InCond( int nCond )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	return ( ( m_nPlayerCond & (1<<nCond) ) != 0 );
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
float CAI_BaseNPC::GetConditionDuration( int nCond )
{
	Assert( nCond >= 0 && nCond < TF_COND_LAST );

	if ( InCond( nCond ) )
	{
		return m_flCondExpireTimeLeft[nCond];
	}
	
	return 0.0f;
}

//-----------------------------------------------------------------------------
// Purpose: Remove any conditions affecting players
//-----------------------------------------------------------------------------
void CAI_BaseNPC::RemoveAllCond( void )
{
	int i;
	for ( i=0;i<TF_COND_LAST;i++ )
	{
		if ( m_nPlayerCond & (1<<i) )
		{
			RemoveCond( i );
		}
	}

	// Now remove all the rest
	m_nPlayerCond = 0;
}


//-----------------------------------------------------------------------------
// Purpose: Called on both client and server. Server when we add the bit,
// and client when it recieves the new cond bits and finds one added
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnConditionAdded( int nCond )
{
	switch( nCond )
	{
	case TF_COND_HEALTH_BUFF:
#ifdef GAME_DLL
		m_flHealFraction = 0;
#endif
		break;
/*
	case TF_COND_STEALTHED:
		OnAddStealthed();
		break;

	case TF_COND_INVULNERABLE:
		OnAddInvulnerable();
		break;

	case TF_COND_TELEPORTED:
		OnAddTeleported();
		break;
*/
	case TF_COND_BURNING:
		OnAddBurning();
		break;
/*
	case TF_COND_DISGUISING:
		OnAddDisguising();
		break;

	case TF_COND_DISGUISED:
		OnAddDisguised();
		break;

	case TF_COND_TAUNTING:
		{
			CTFWeaponBase *pWpn = m_pOuter->GetActiveTFWeapon();
			if ( pWpn )
			{
				// cancel any reload in progress.
				pWpn->AbortReload();
			}
		}
		break;
*/
	default:
		break;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Called on both client and server. Server when we remove the bit,
// and client when it recieves the new cond bits and finds one removed
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnConditionRemoved( int nCond )
{
	switch( nCond )
	{
/*
	case TF_COND_ZOOMED:
		OnRemoveZoomed();
		break;
*/
	case TF_COND_BURNING:
		OnRemoveBurning();
		break;

	case TF_COND_HEALTH_BUFF:
#ifdef GAME_DLL
		m_flHealFraction = 0;
#endif
		break;
/*
	case TF_COND_STEALTHED:
		OnRemoveStealthed();
		break;

	case TF_COND_DISGUISED:
		OnRemoveDisguised();
		break;

	case TF_COND_DISGUISING:
		OnRemoveDisguising();
		break;

	case TF_COND_INVULNERABLE:
		OnRemoveInvulnerable();
		break;

	case TF_COND_TELEPORTED:
		OnRemoveTeleported();
		break;
*/
	default:
		break;
	}
}

int CAI_BaseNPC::GetMaxBuffedHealth( void )
{
	float flBoostMax = GetMaxHealth() * tf_max_health_boost.GetFloat();

	int iRoundDown = floor( flBoostMax / 5 );
	iRoundDown = iRoundDown * 5;

	return iRoundDown;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnAddBurning( void )
{
#ifdef CLIENT_DLL
	// Start the burning effect
	if ( !m_pBurningEffect )
	{
		const char *pEffectName = ( GetTeamNumber() == TF_TEAM_BLUE ) ? "burningplayer_blue" : "burningplayer_red";
		m_pBurningEffect = ParticleProp()->Create( pEffectName, PATTACH_ABSORIGIN_FOLLOW );

		m_flBurnEffectStartTime = gpGlobals->curtime;
		m_flBurnEffectEndTime = gpGlobals->curtime + TF_BURNING_FLAME_LIFE;
	}
#endif

	// play a fire-starting sound
	EmitSound( "Fire.Engulf" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAI_BaseNPC::OnRemoveBurning( void )
{
#ifdef CLIENT_DLL
	StopBurningSound();

	if ( m_pBurningEffect )
	{
		ParticleProp()->StopEmission( m_pBurningEffect );
		m_pBurningEffect = NULL;
	}

	m_flBurnEffectStartTime = 0;
	m_flBurnEffectEndTime = 0;
#else
	m_hBurnAttacker = NULL;
#endif
}
#endif