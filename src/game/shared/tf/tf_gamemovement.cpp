//========= Copyright © 1996-2004, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================
#include "cbase.h"
#include "gamemovement.h"
#include "tf_gamerules.h"
#include "tf_shareddefs.h"
#include "in_buttons.h"
#include "movevars_shared.h"
#include "collisionutils.h"
#include "debugoverlay_shared.h"
#include "baseobject_shared.h"
#include "particle_parse.h"
#include "baseobject_shared.h"
#include "coordsize.h"
#include "func_ladder.h"

#ifdef CLIENT_DLL
	#include "c_tf_player.h"
	#include "c_world.h"
	#include "c_team.h"

	#define CTeam C_Team

#else
	#include "tf_player.h"
	#include "team.h"
	#include "env_player_surface_trigger.h"
#endif

ConVar	tf_maxspeed( "tf_maxspeed", "400", FCVAR_NOTIFY | FCVAR_REPLICATED | FCVAR_CHEAT  | FCVAR_DEVELOPMENTONLY);
ConVar	tf_showspeed( "tf_showspeed", "0", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar	tf_avoidteammates( "tf_avoidteammates", "1", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar  tf_solidobjects( "tf_solidobjects", "1", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
ConVar	tf_clamp_back_speed( "tf_clamp_back_speed", "0.9", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );
ConVar  tf_clamp_back_speed_min( "tf_clamp_back_speed_min", "100", FCVAR_REPLICATED | FCVAR_DEVELOPMENTONLY );

ConVar	lf_bunnyjump_max_speed_factor("lf_bunnyjump_max_speed_factor", "1.2", FCVAR_REPLICATED);
ConVar  lf_autojump("lf_autojump", "0", FCVAR_REPLICATED, "Automatically jump while holding the jump button down");
ConVar  lf_duckjump("lf_duckjump", "0", FCVAR_REPLICATED, "Toggles jumping while ducked");
ConVar  lf_groundspeed_cap("lf_groundspeed_cap", "1", FCVAR_REPLICATED, "Toggles the max speed cap imposed when a player is standing on the ground");

ConVar sv_autoladderdismount( "sv_autoladderdismount", "1", FCVAR_REPLICATED, "Automatically dismount from ladders when you reach the end (don't have to +USE)." );
ConVar sv_ladderautomountdot( "sv_ladderautomountdot", "0.4", FCVAR_REPLICATED, "When auto-mounting a ladder by looking up its axis, this is the tolerance for looking now directly along the ladder axis." );

ConVar sv_ladder_useonly( "sv_ladder_useonly", "0", FCVAR_REPLICATED, "If set, ladders can only be mounted by pressing +USE" );

#define TF_MAX_SPEED   400

#define TF_WATERJUMP_FORWARD  30
#define TF_WATERJUMP_UP       300
//ConVar	tf_waterjump_up( "tf_waterjump_up", "300", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );
//ConVar	tf_waterjump_forward( "tf_waterjump_forward", "30", FCVAR_REPLICATED | FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY );

#define USE_DISMOUNT_SPEED 100

struct LadderMove_t;
class CInfoLadderDismount;

struct NearbyDismount_t
{
	CInfoLadderDismount		*dismount;
	float					distSqr;
};

class CTFGameMovement : public CGameMovement
{
public:
	DECLARE_CLASS( CTFGameMovement, CGameMovement );

	CTFGameMovement(); 

	virtual void PlayerMove();
	virtual unsigned int PlayerSolidMask( bool brushOnly = false );
	virtual void ProcessMovement( CBasePlayer *pBasePlayer, CMoveData *pMove );
	virtual bool CanAccelerate();
	virtual bool CheckJumpButton();
	virtual bool CheckWater( void );
	virtual void WaterMove( void );
	virtual void FullWalkMove();
	virtual void WalkMove( void );
	virtual void AirMove( void );
	virtual void FullTossMove( void );
	virtual void CategorizePosition( void );
	virtual void CheckFalling( void );
	virtual void Duck( void );
	virtual Vector GetPlayerViewOffset( bool ducked ) const;

	virtual void	TracePlayerBBox( const Vector& start, const Vector& end, unsigned int fMask, int collisionGroup, trace_t& pm );
	virtual CBaseHandle	TestPlayerPosition( const Vector& pos, int collisionGroup, trace_t& pm );
	virtual void	StepMove( Vector &vecDestination, trace_t &trace );
	virtual bool	GameHasLadders() const;
	virtual void SetGroundEntity( trace_t *pm );
	virtual void PlayerRoughLandingEffects( float fvol );
protected:

	virtual void CheckWaterJump(void );
	void		 FullWalkMoveUnderwater();

private:

	bool		CheckWaterJumpButton( void );
	void		AirDash( void );
	void		PreventBunnyJumping();

public:
	// HL2 ladders
	// Overrides
	virtual void FullLadderMove();
	virtual bool LadderMove( void );
	virtual bool OnLadder( trace_t &trace );
	virtual int GetCheckInterval( IntervalType_t type );

private:

	// See if we are pressing use near a ladder "mount" point and if so, latch us onto the ladder
	bool		CheckLadderAutoMount( CFuncLadder *ladder, const Vector& bestOrigin );

	bool		CheckLadderAutoMountCone( CFuncLadder *ladder, const Vector& bestOrigin, float maxAngleDelta, float maxDistToLadder );
	bool		CheckLadderAutoMountEndPoint(CFuncLadder *ladder, const Vector& bestOrigin );


	bool		LookingAtLadder( CFuncLadder *ladder );

	// Are we forcing the user's position to a new spot
	bool		IsForceMoveActive();
	// Start forcing player position
	void		StartForcedMove( bool mounting, float transit_speed, const Vector& goalpos, CFuncLadder *ladder );
	// Returns false when finished
	bool		ContinueForcedMove();

	// Given a list of nearby ladders, find the best ladder and the "mount" origin
	void		Findladder( float maxdist, CFuncLadder **ppLadder, Vector& ladderOrigin, const CFuncLadder *skipLadder );

	// Debounce the +USE key
	void		SwallowUseKey();

	// Returns true if the player will auto-exit the ladder via a dismount node
	bool		ExitLadderViaDismountNode( CFuncLadder *ladder, bool strict, bool useAlternate = false );
	void		GetSortedDismountNodeList( const Vector &org, float radius, CFuncLadder *ladder, CUtlRBTree< NearbyDismount_t, int >& list );

	LadderMove_t *GetLadderMove();
	CTFPlayer	*GetTFPlayer();

	void		SetLadder( CFuncLadder *ladder );
	CFuncLadder *GetLadder();

private:

	Vector		m_vecWaterPoint;
	CTFPlayer  *m_pTFPlayer;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
inline CTFPlayer	*CTFGameMovement::GetTFPlayer()
{
	return ToTFPlayer( player );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : inline LadderMove*
//-----------------------------------------------------------------------------
inline LadderMove_t *CTFGameMovement::GetLadderMove()
{
	CTFPlayer *p = GetTFPlayer();
	if ( !p )
	{
		return NULL;
	}
	return p->GetLadderMove();
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *ladder - 
//-----------------------------------------------------------------------------
inline void CTFGameMovement::SetLadder( CFuncLadder *ladder )
{
	CFuncLadder* oldLadder = GetLadder();

	if ( !ladder && oldLadder )
	{
		oldLadder->PlayerGotOff( GetTFPlayer() );
	}


	GetTFPlayer()->m_hLadder.Set( ladder );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : CFuncLadder
//-----------------------------------------------------------------------------
inline CFuncLadder *CTFGameMovement::GetLadder()
{
	return static_cast<CFuncLadder*>( static_cast<CBaseEntity *>( GetTFPlayer()->m_hLadder.Get() ) );
}

// Expose our interface.
static CTFGameMovement g_GameMovement;
IGameMovement *g_pGameMovement = ( IGameMovement * )&g_GameMovement;

EXPOSE_SINGLE_INTERFACE_GLOBALVAR(CGameMovement, IGameMovement,INTERFACENAME_GAMEMOVEMENT, g_GameMovement );


// ---------------------------------------------------------------------------------------- //
// CTFGameMovement.
// ---------------------------------------------------------------------------------------- //

CTFGameMovement::CTFGameMovement()
{
	m_pTFPlayer = NULL;
}

//---------------------------------------------------------------------------------------- 
// Purpose: moves the player
//----------------------------------------------------------------------------------------
void CTFGameMovement::PlayerMove()
{
	// call base class to do movement
	BaseClass::PlayerMove();

	// handle player's interaction with water
	int nNewWaterLevel = m_pTFPlayer->GetWaterLevel();
	if ( m_nOldWaterLevel != nNewWaterLevel )
	{
		if ( WL_NotInWater == m_nOldWaterLevel )
		{
			// The player has just entered the water.  Determine if we should play a splash sound.
			bool bPlaySplash = false;
					
			Vector vecVelocity = m_pTFPlayer->GetAbsVelocity();
			if ( vecVelocity.z <= -200.0f )
			{
				// If the player has significant downward velocity, play a splash regardless of water depth.  (e.g. Jumping hard into a puddle)
				bPlaySplash = true;
			}
			else
			{
				// Look at the water depth below the player.  If it's significantly deep, play a splash to accompany the sinking that's about to happen.
				Vector vecStart = m_pTFPlayer->GetAbsOrigin();
				Vector vecEnd = vecStart;
				vecEnd.z -= 20;	// roughly thigh deep
				trace_t tr;
				// see if we hit anything solid a little bit below the player
				UTIL_TraceLine( vecStart, vecEnd, MASK_SOLID,m_pTFPlayer, COLLISION_GROUP_NONE, &tr );
				if ( tr.fraction >= 1.0f ) 
				{
					// some amount of water below the player, play a splash
					bPlaySplash = true;
				}
			}

			if ( bPlaySplash )
			{
				m_pTFPlayer->EmitSound( "Physics.WaterSplash" );
			}
		} 
	}
}

Vector CTFGameMovement::GetPlayerViewOffset( bool ducked ) const
{
	return ducked ? VEC_DUCK_VIEW : ( m_pTFPlayer->GetClassEyeHeight() );
}

//-----------------------------------------------------------------------------
// Purpose: Allow bots etc to use slightly different solid masks
//-----------------------------------------------------------------------------
unsigned int CTFGameMovement::PlayerSolidMask( bool brushOnly )
{
	unsigned int uMask = 0;

	if ( m_pTFPlayer )
	{
		switch( m_pTFPlayer->GetTeamNumber() )
		{
		case TF_TEAM_RED:
			uMask = CONTENTS_BLUETEAM;
			break;

		case TF_TEAM_BLUE:
			uMask = CONTENTS_REDTEAM;
			break;
		}
	}

	return ( uMask | BaseClass::PlayerSolidMask( brushOnly ) );
}

//-----------------------------------------------------------------------------
// Purpose: Overridden to allow players to run faster than the maxspeed
//-----------------------------------------------------------------------------
void CTFGameMovement::ProcessMovement( CBasePlayer *pBasePlayer, CMoveData *pMove )
{
	// Verify data.
	Assert( pBasePlayer );
	Assert( pMove );
	if ( !pBasePlayer || !pMove )
		return;

	// Reset point contents for water check.
	ResetGetPointContentsCache();

	// Cropping movement speed scales mv->m_fForwardSpeed etc. globally
	// Once we crop, we don't want to recursively crop again, so we set the crop
	// flag globally here once per usercmd cycle.
	m_iSpeedCropped = SPEED_CROPPED_RESET;

	// Get the current TF player.
	m_pTFPlayer = ToTFPlayer( pBasePlayer );
	player = m_pTFPlayer;
	mv = pMove;

	// The max speed is currently set to the scout - if this changes we need to change this!
	mv->m_flMaxSpeed = TF_MAX_SPEED; /*tf_maxspeed.GetFloat();*/

	// Run the command.
	PlayerMove();
	FinishMove();
}


bool CTFGameMovement::CanAccelerate()
{
	// Only allow the player to accelerate when in certain states.
	int nCurrentState = m_pTFPlayer->m_Shared.GetState();
	if ( nCurrentState == TF_STATE_ACTIVE )
	{
		return player->GetWaterJumpTime() == 0;
	}
	else if ( player->IsObserver() )
	{
		return true;
	}
	else
	{	
		return false;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Check to see if we are in water.  If so the jump button acts like a
// swim upward key.
//-----------------------------------------------------------------------------
bool CTFGameMovement::CheckWaterJumpButton( void )
{
	// See if we are water jumping.  If so, decrement count and return.
	if ( player->m_flWaterJumpTime )
	{
		player->m_flWaterJumpTime -= gpGlobals->frametime;
		if (player->m_flWaterJumpTime < 0)
		{
			player->m_flWaterJumpTime = 0;
		}

		return false;
	}

	// In water above our waist.
	if ( player->GetWaterLevel() >= 2 )
	{	
		// Swimming, not jumping.
		SetGroundEntity( NULL );

		// We move up a certain amount.
		if ( player->GetWaterType() == CONTENTS_WATER )
		{
			mv->m_vecVelocity[2] = 100;
		}
		else if ( player->GetWaterType() == CONTENTS_SLIME )
		{
			mv->m_vecVelocity[2] = 80;
		}

		// Play swiming sound.
		if ( player->m_flSwimSoundTime <= 0 )
		{
			// Don't play sound again for 1 second.
			player->m_flSwimSoundTime = 1000;
			PlaySwimSound();
		}

		return false;
	}

	return true;
}

void CTFGameMovement::AirDash( void )
{
	// Apply approx. the jump velocity added to an air dash.
	Assert( sv_gravity.GetFloat() == 800.0f );
	float flDashZ = 268.3281572999747f;

	// Get the wish direction.
	Vector vecForward, vecRight;
	AngleVectors( mv->m_vecViewAngles, &vecForward, &vecRight, NULL );
	vecForward.z = 0.0f;
	vecRight.z = 0.0f;		
	VectorNormalize( vecForward );
	VectorNormalize( vecRight );

	// Copy movement amounts
	float flForwardMove = mv->m_flForwardMove;
	float flSideMove = mv->m_flSideMove;

	// Find the direction,velocity in the x,y plane.
	Vector vecWishDirection( ( ( vecForward.x * flForwardMove ) + ( vecRight.x * flSideMove ) ),
		                     ( ( vecForward.y * flForwardMove ) + ( vecRight.y * flSideMove ) ), 
		                     0.0f );
	
	// Update the velocity on the scout.
	mv->m_vecVelocity = vecWishDirection;
	mv->m_vecVelocity.z += flDashZ;

	m_pTFPlayer->m_Shared.SetAirDash( true );

	// Play the gesture.
	m_pTFPlayer->DoAnimationEvent( PLAYERANIMEVENT_DOUBLEJUMP );
}

// Only allow bunny jumping up to 1.2x server / player maxspeed setting
//#define BUNNYJUMP_MAX_SPEED_FACTOR 1.2f

void CTFGameMovement::PreventBunnyJumping()
{
	// Speed at which bunny jumping is limited
	float maxscaledspeed = lf_bunnyjump_max_speed_factor.GetFloat() * player->m_flMaxspeed;
	if ( maxscaledspeed <= 0.0f )
		return;

	// Current player speed
	float spd = mv->m_vecVelocity.Length();
	if ( spd <= maxscaledspeed )
		return;

	// Apply this cropping fraction to velocity
	float fraction = ( maxscaledspeed / spd );


	mv->m_vecVelocity *= fraction;
}

bool CTFGameMovement::CheckJumpButton()
{
	// Are we dead?  Then we cannot jump.
	if ( player->pl.deadflag )
		return false;

	// Check to see if we are in water.
	if ( !CheckWaterJumpButton() )
		return false;

	// Cannot jump while taunting
	if ( m_pTFPlayer->m_Shared.InCond( TF_COND_TAUNTING ) )
		return false;

	// Check to see if the player is a scout.
	bool bScout = m_pTFPlayer->GetPlayerClass()->IsClass( TF_CLASS_SCOUT );
	bool bAirDash = false;
	bool bOnGround = ( player->GetGroundEntity() != NULL );

	// Cannot jump will ducked.
	if ( player->GetFlags() & FL_DUCKING )
	{
		// Let a scout do it.
		bool bAllow = ( bScout && !bOnGround ) || lf_duckjump.GetBool();

		if ( !bAllow )
			return false;
	}

	// Cannot jump while in the unduck transition.
	if ( ( player->m_Local.m_bDucking && (  player->GetFlags() & FL_DUCKING ) ) || ( player->m_Local.m_flDuckJumpTime > 0.0f ) && !lf_duckjump.GetBool() )
		return false;

	// Cannot jump again until the jump button has been released.
	if ( mv->m_nOldButtons & IN_JUMP && ( !lf_autojump.GetBool() || !bOnGround ) )
		return false;

	// In air, so ignore jumps (unless you are a scout).
	if ( !bOnGround )
	{
		if ( bScout && !m_pTFPlayer->m_Shared.IsAirDashing() )
		{
			bAirDash = true;
		}
		else
		{
			mv->m_nOldButtons |= IN_JUMP;
			return false;
		}
	}

	// Check for an air dash.
	if ( bAirDash )
	{
		AirDash();
		return true;
	}

	PreventBunnyJumping();

	// Start jump animation and player sound (specific TF animation and flags).
	m_pTFPlayer->DoAnimationEvent( PLAYERANIMEVENT_JUMP );
	player->PlayStepSound( (Vector &)mv->GetAbsOrigin(), player->m_pSurfaceData, 1.0, true );
	m_pTFPlayer->m_Shared.SetJumping( true );

	// Set the player as in the air.
	SetGroundEntity( NULL );

	// Check the surface the player is standing on to see if it impacts jumping.
	float flGroundFactor = 1.0f;
	if ( player->m_pSurfaceData )
	{
		flGroundFactor = player->m_pSurfaceData->game.jumpFactor; 
	}

	// fMul = sqrt( 2.0 * gravity * jump_height (21.0units) ) * GroundFactor
	Assert( sv_gravity.GetFloat() == 800.0f );
	float flMul = 268.3281572999747f * flGroundFactor;

	// Save the current z velocity.
	float flStartZ = mv->m_vecVelocity[2];

	// Acclerate upward
	if ( (  player->m_Local.m_bDucking ) || (  player->GetFlags() & FL_DUCKING ) )
	{
		// If we are ducking...
		// d = 0.5 * g * t^2		- distance traveled with linear accel
		// t = sqrt(2.0 * 45 / g)	- how long to fall 45 units
		// v = g * t				- velocity at the end (just invert it to jump up that high)
		// v = g * sqrt(2.0 * 45 / g )
		// v^2 = g * g * 2.0 * 45 / g
		// v = sqrt( g * 2.0 * 45 )
		mv->m_vecVelocity[2] = flMul;  // 2 * gravity * jump_height * ground_factor
	}
	else
	{
		mv->m_vecVelocity[2] += flMul;  // 2 * gravity * jump_height * ground_factor
	}

	// Apply gravity.
	FinishGravity();

	// Save the output data for the physics system to react to if need be.
	mv->m_outJumpVel.z += mv->m_vecVelocity[2] - flStartZ;
	mv->m_outStepHeight += 0.15f;

	// Flag that we jumped and don't jump again until it is released.
	mv->m_nOldButtons |= IN_JUMP;
	return true;
}

bool CTFGameMovement::CheckWater( void )
{
	Vector vecPlayerMin = GetPlayerMins();
	Vector vecPlayerMax = GetPlayerMaxs();

	Vector vecPoint( ( mv->GetAbsOrigin().x + ( vecPlayerMin.x + vecPlayerMax.x ) * 0.5f ),
				     ( mv->GetAbsOrigin().y + ( vecPlayerMin.y + vecPlayerMax.y ) * 0.5f ),
				     ( mv->GetAbsOrigin().z + vecPlayerMin.z + 1 ) );


	// Assume that we are not in water at all.
	int wl = WL_NotInWater;
	int wt = CONTENTS_EMPTY;

	// Check to see if our feet are underwater.
	int nContents = GetPointContentsCached( vecPoint, 0 );	
	if ( nContents & MASK_WATER )
	{
		// Clear our jump flag, because we have landed in water.
		m_pTFPlayer->m_Shared.SetJumping( false );

		// Set water type and level.
		wt = nContents;
		wl = WL_Feet;

		float flWaistZ = mv->GetAbsOrigin().z + ( vecPlayerMin.z + vecPlayerMax.z ) * 0.5f + 12.0f;

		// Now check eyes
		vecPoint.z = mv->GetAbsOrigin().z + player->GetViewOffset()[2];
		nContents = GetPointContentsCached( vecPoint, 1 );
		if ( nContents & MASK_WATER )
		{
			// In over our eyes
			wl = WL_Eyes;  
			VectorCopy( vecPoint, m_vecWaterPoint );
			m_vecWaterPoint.z = flWaistZ;
		}
		else
		{
			// Now check a point that is at the player hull midpoint (waist) and see if that is underwater.
			vecPoint.z = flWaistZ;
			nContents = GetPointContentsCached( vecPoint, 2 );
			if ( nContents & MASK_WATER )
			{
				// Set the water level at our waist.
				wl = WL_Waist;
				VectorCopy( vecPoint, m_vecWaterPoint );
			}
		}
	}

	player->SetWaterLevel( wl );
	player->SetWaterType( wt );

	// If we just transitioned from not in water to water, record the time for splashes, etc.
	if ( ( WL_NotInWater == m_nOldWaterLevel ) && ( wl >  WL_NotInWater ) )
	{
		m_flWaterEntryTime = gpGlobals->curtime;
	}

	return ( wl > WL_Feet );
}



//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::WaterMove( void )
{
	int i;
	float	wishspeed;
	Vector	wishdir;
	Vector	start, dest;
	Vector  temp;
	trace_t	pm;
	float speed, newspeed, addspeed, accelspeed;

	// Determine movement angles.
	Vector vecForward, vecRight, vecUp;
	AngleVectors( mv->m_vecViewAngles, &vecForward, &vecRight, &vecUp );

	// Calculate the desired direction and speed.
	Vector vecWishVelocity;
	int iAxis;
	for ( iAxis = 0 ; iAxis < 3; ++iAxis )
	{
		vecWishVelocity[iAxis] = ( vecForward[iAxis] * mv->m_flForwardMove ) + ( vecRight[iAxis] * mv->m_flSideMove );
	}

	// Check for upward velocity (JUMP).
	if ( mv->m_nButtons & IN_JUMP )
	{
		if ( player->GetWaterLevel() == WL_Eyes )
		{
			vecWishVelocity[2] += mv->m_flClientMaxSpeed;
		}
	}
	// Sinking if not moving.
	else if ( !mv->m_flForwardMove && !mv->m_flSideMove && !mv->m_flUpMove )
	{
		vecWishVelocity[2] -= 60;
	}
	// Move up based on view angle.
	else
	{
		vecWishVelocity[2] += mv->m_flUpMove;
	}

	// Copy it over and determine speed
	VectorCopy( vecWishVelocity, wishdir );
	wishspeed = VectorNormalize( wishdir );

	// Cap speed.
	if (wishspeed > mv->m_flMaxSpeed)
	{
		VectorScale( vecWishVelocity, mv->m_flMaxSpeed/wishspeed, vecWishVelocity );
		wishspeed = mv->m_flMaxSpeed;
	}

	// Slow us down a bit.
	wishspeed *= 0.8;
	
	// Water friction
	VectorCopy( mv->m_vecVelocity, temp );
	speed = VectorNormalize( temp );
	if ( speed )
	{
		newspeed = speed - gpGlobals->frametime * speed * sv_friction.GetFloat() * player->m_surfaceFriction;
		if ( newspeed < 0.1f )
		{
			newspeed = 0;
		}

		VectorScale (mv->m_vecVelocity, newspeed/speed, mv->m_vecVelocity);
	}
	else
	{
		newspeed = 0;
	}

	// water acceleration
	if (wishspeed >= 0.1f)  // old !
	{
		addspeed = wishspeed - newspeed;
		if (addspeed > 0)
		{
			VectorNormalize(vecWishVelocity);
			accelspeed = sv_accelerate.GetFloat() * wishspeed * gpGlobals->frametime * player->m_surfaceFriction;
			if (accelspeed > addspeed)
			{
				accelspeed = addspeed;
			}

			for (i = 0; i < 3; i++)
			{
				float deltaSpeed = accelspeed * vecWishVelocity[i];
				mv->m_vecVelocity[i] += deltaSpeed;
				mv->m_outWishVel[i] += deltaSpeed;
			}
		}
	}

	VectorAdd (mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

	// Now move
	// assume it is a stair or a slope, so press down from stepheight above
	VectorMA (mv->GetAbsOrigin(), gpGlobals->frametime, mv->m_vecVelocity, dest);
	
	TracePlayerBBox( mv->GetAbsOrigin(), dest, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm );
	if ( pm.fraction == 1.0f )
	{
		VectorCopy( dest, start );
		if ( player->m_Local.m_bAllowAutoMovement )
		{
			start[2] += player->m_Local.m_flStepSize + 1;
		}
		
		TracePlayerBBox( start, dest, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, pm );

		if (!pm.startsolid && !pm.allsolid)
		{	
#if 0
			float stepDist = pm.endpos.z - mv->GetAbsOrigin().z;
			mv->m_outStepHeight += stepDist;
			// walked up the step, so just keep result and exit

			Vector vecNewWaterPoint;
			VectorCopy( m_vecWaterPoint, vecNewWaterPoint );
			vecNewWaterPoint.z += ( dest.z - mv->GetAbsOrigin().z );
			bool bOutOfWater = !( enginetrace->GetPointContents( vecNewWaterPoint ) & MASK_WATER );
			if ( bOutOfWater && ( mv->m_vecVelocity.z > 0.0f ) && ( pm.fraction == 1.0f )  )
			{
				// Check the waist level water positions.
				trace_t traceWater;
				UTIL_TraceLine( vecNewWaterPoint, m_vecWaterPoint, CONTENTS_WATER, player, COLLISION_GROUP_NONE, &traceWater );
				if( traceWater.fraction < 1.0f )
				{
					float flFraction = 1.0f - traceWater.fraction;

//					Vector vecSegment;
//					VectorSubtract( mv->GetAbsOrigin(), dest, vecSegment );
//					VectorMA( mv->GetAbsOrigin(), flFraction, vecSegment, mv->GetAbsOrigin() );
					float flZDiff = dest.z - mv->GetAbsOrigin().z;
					float flSetZ = mv->GetAbsOrigin().z + ( flFraction * flZDiff );
					flSetZ -= 0.0325f;

					VectorCopy (pm.endpos, mv->GetAbsOrigin());
					mv->GetAbsOrigin().z = flSetZ;
					VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
					mv->m_vecVelocity.z = 0.0f;
				}

			}
			else
			{
				VectorCopy (pm.endpos, mv->GetAbsOrigin());
				VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
			}

			return;
#endif
			float stepDist = pm.endpos.z - mv->GetAbsOrigin().z;
			mv->m_outStepHeight += stepDist;
			// walked up the step, so just keep result and exit
			mv->SetAbsOrigin( pm.endpos );
			VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
			return;
		}

		// Try moving straight along out normal path.
		TryPlayerMove();
	}
	else
	{
		if ( !player->GetGroundEntity() )
		{
			TryPlayerMove();
			VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
			return;
		}

		StepMove( dest, pm );
	}
	
	VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::WalkMove( void )
{
	// Get the movement angles.
	Vector vecForward, vecRight, vecUp;
	AngleVectors( mv->m_vecViewAngles, &vecForward, &vecRight, &vecUp );
	vecForward.z = 0.0f;
	vecRight.z = 0.0f;		
	VectorNormalize( vecForward );
	VectorNormalize( vecRight );

	// Copy movement amounts
	float flForwardMove = mv->m_flForwardMove;
	float flSideMove = mv->m_flSideMove;
	
	// Find the direction,velocity in the x,y plane.
	Vector vecWishDirection( ( ( vecForward.x * flForwardMove ) + ( vecRight.x * flSideMove ) ),
		                     ( ( vecForward.y * flForwardMove ) + ( vecRight.y * flSideMove ) ), 
							 0.0f );

	// Calculate the speed and direction of movement, then clamp the speed.
	float flWishSpeed = VectorNormalize( vecWishDirection );
	flWishSpeed = clamp( flWishSpeed, 0.0f, mv->m_flMaxSpeed );

	// Accelerate in the x,y plane.
	mv->m_vecVelocity.z = 0;
	Accelerate( vecWishDirection, flWishSpeed, sv_accelerate.GetFloat() );
	Assert( mv->m_vecVelocity.z == 0.0f );

	// Clamp the players speed in x,y.
	if ( lf_groundspeed_cap.GetBool() )
	{
		float flNewSpeed = VectorLength(mv->m_vecVelocity);
		if (flNewSpeed > mv->m_flMaxSpeed)
		{
			float flScale = (mv->m_flMaxSpeed / flNewSpeed);
			mv->m_vecVelocity.x *= flScale;
			mv->m_vecVelocity.y *= flScale;
		}
	}

	// Now reduce their backwards speed to some percent of max, if they are travelling backwards
	// unless they are under some minimum, to not penalize deployed snipers or heavies
	if ( tf_clamp_back_speed.GetFloat() < 1.0 && VectorLength( mv->m_vecVelocity ) > tf_clamp_back_speed_min.GetFloat() )
	{
		float flDot = DotProduct( vecForward, mv->m_vecVelocity );

		// are we moving backwards at all?
		if ( flDot < 0 )
		{
			Vector vecBackMove = vecForward * flDot;
			Vector vecRightMove = vecRight * DotProduct( vecRight, mv->m_vecVelocity );

			// clamp the back move vector if it is faster than max
			float flBackSpeed = VectorLength( vecBackMove );
			float flMaxBackSpeed = ( mv->m_flMaxSpeed * tf_clamp_back_speed.GetFloat() );

			if ( flBackSpeed > flMaxBackSpeed )
			{
				vecBackMove *= flMaxBackSpeed / flBackSpeed;
			}
			
			// reassemble velocity	
			mv->m_vecVelocity = vecBackMove + vecRightMove;
		}
	}

	// Add base velocity to the player's current velocity - base velocity = velocity from conveyors, etc.
	VectorAdd( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );

	// Calculate the current speed and return if we are not really moving.
	float flSpeed = VectorLength( mv->m_vecVelocity );
	if ( flSpeed < 1.0f )
	{
		// I didn't remove the base velocity here since it wasn't moving us in the first place.
		mv->m_vecVelocity.Init();
		return;
	}

	// Calculate the destination.
	Vector vecDestination;
	vecDestination.x = mv->GetAbsOrigin().x + ( mv->m_vecVelocity.x * gpGlobals->frametime );
	vecDestination.y = mv->GetAbsOrigin().y + ( mv->m_vecVelocity.y * gpGlobals->frametime );	
	vecDestination.z = mv->GetAbsOrigin().z;

	// Try moving to the destination.
	trace_t trace;
	TracePlayerBBox( mv->GetAbsOrigin(), vecDestination, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace );
	if ( trace.fraction == 1.0f )
	{
		// Made it to the destination (remove the base velocity).
		mv->SetAbsOrigin( trace.endpos );
		VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );

		// Save the wish velocity.
		mv->m_outWishVel += ( vecWishDirection * flWishSpeed );

		// Try and keep the player on the ground.
		// NOTE YWB 7/5/07: Don't do this here, our version of CategorizePosition encompasses this test
		// StayOnGround();

		return;
	}

	// Now try and do a step move.
	StepMove( vecDestination, trace );

	// Remove base velocity.
	Vector baseVelocity = player->GetBaseVelocity();
	VectorSubtract( mv->m_vecVelocity, baseVelocity, mv->m_vecVelocity );

	// Save the wish velocity.
	mv->m_outWishVel += ( vecWishDirection * flWishSpeed );

	// Try and keep the player on the ground.
	// NOTE YWB 7/5/07: Don't do this here, our version of CategorizePosition encompasses this test
	// StayOnGround();

#if 0
	// Debugging!!!
	Vector vecTestVelocity = mv->m_vecVelocity;
	vecTestVelocity.z = 0.0f;
	float flTestSpeed = VectorLength( vecTestVelocity );
	if ( baseVelocity.IsZero() && ( flTestSpeed > ( mv->m_flMaxSpeed + 1.0f ) ) )
	{
		Msg( "Step Max Speed < %f\n", flTestSpeed );
	}

	if ( tf_showspeed.GetBool() )
	{
		Msg( "Speed=%f\n", flTestSpeed );
	}

#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::AirMove( void )
{
	int			i;
	Vector		wishvel;
	float		fmove, smove;
	Vector		wishdir;
	float		wishspeed;
	Vector forward, right, up;

	AngleVectors (mv->m_vecViewAngles, &forward, &right, &up);  // Determine movement angles

	// Copy movement amounts
	fmove = mv->m_flForwardMove;
	smove = mv->m_flSideMove;

	// Zero out z components of movement vectors
	forward[2] = 0;
	right[2]   = 0;
	VectorNormalize(forward);  // Normalize remainder of vectors
	VectorNormalize(right);    // 

	for (i=0 ; i<2 ; i++)       // Determine x and y parts of velocity
		wishvel[i] = forward[i]*fmove + right[i]*smove;
	wishvel[2] = 0;             // Zero out z part of velocity

	VectorCopy (wishvel, wishdir);   // Determine maginitude of speed of move
	wishspeed = VectorNormalize(wishdir);

	//
	// clamp to server defined max speed
	//
	if ( wishspeed != 0 && (wishspeed > mv->m_flMaxSpeed))
	{
		VectorScale (wishvel, mv->m_flMaxSpeed/wishspeed, wishvel);
		wishspeed = mv->m_flMaxSpeed;
	}

	AirAccelerate( wishdir, wishspeed, sv_airaccelerate.GetFloat() );

	// Add in any base velocity to the current velocity.
	VectorAdd( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );

	TryPlayerMove();

	// Now pull the base velocity back out.   Base velocity is set if you are on a moving object, like a conveyor (or maybe another monster?)
	VectorSubtract( mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity );
}

extern void TracePlayerBBoxForGround( const Vector& start, const Vector& end, const Vector& minsSrc,
							  const Vector& maxsSrc, IHandleEntity *player, unsigned int fMask,
							  int collisionGroup, trace_t& pm );


//-----------------------------------------------------------------------------
// This filter checks against buildable objects.
//-----------------------------------------------------------------------------
class CTraceFilterObject : public CTraceFilterSimple
{
public:
	DECLARE_CLASS( CTraceFilterObject, CTraceFilterSimple );

	CTraceFilterObject( const IHandleEntity *passentity, int collisionGroup );
	virtual bool ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask );
};

CTraceFilterObject::CTraceFilterObject( const IHandleEntity *passentity, int collisionGroup ) :
BaseClass( passentity, collisionGroup )
{

}

bool CTraceFilterObject::ShouldHitEntity( IHandleEntity *pHandleEntity, int contentsMask )
{
	CBaseEntity *pEntity = EntityFromEntityHandle( pHandleEntity );

	if ( pEntity && pEntity->IsBaseObject() )
	{
		CBaseObject *pObject = static_cast<CBaseObject *>( pEntity );

		Assert( pObject );

		if ( pObject && pObject->ShouldPlayersAvoid() )
		{
			if ( pObject->GetOwner() == GetPassEntity() )
				return true;
		}
	}

	return CTraceFilterSimple::ShouldHitEntity( pHandleEntity, contentsMask );
}

CBaseHandle CTFGameMovement::TestPlayerPosition( const Vector& pos, int collisionGroup, trace_t& pm )
{
	if( tf_solidobjects.GetBool() == false )
		return BaseClass::TestPlayerPosition( pos, collisionGroup, pm );

	Ray_t ray;
	ray.Init( pos, pos, GetPlayerMins(), GetPlayerMaxs() );
	
	CTraceFilterObject traceFilter( mv->m_nPlayerHandle.Get(), collisionGroup );
	enginetrace->TraceRay( ray, PlayerSolidMask(), &traceFilter, &pm );

	if ( (pm.contents & PlayerSolidMask()) && pm.m_pEnt )
	{
		return pm.m_pEnt->GetRefEHandle();
	}
	else
	{	
		return INVALID_EHANDLE_INDEX;
	}
}

//-----------------------------------------------------------------------------
// Traces player movement + position
//-----------------------------------------------------------------------------
void CTFGameMovement::TracePlayerBBox( const Vector& start, const Vector& end, unsigned int fMask, int collisionGroup, trace_t& pm )
{
	if( tf_solidobjects.GetBool() == false )
		return BaseClass::TracePlayerBBox( start, end, fMask, collisionGroup, pm );

	Ray_t ray;
	ray.Init( start, end, GetPlayerMins(), GetPlayerMaxs() );
	
	CTraceFilterObject traceFilter( mv->m_nPlayerHandle.Get(), collisionGroup );

	enginetrace->TraceRay( ray, fMask, &traceFilter, &pm );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &input - 
//-----------------------------------------------------------------------------
void CTFGameMovement::CategorizePosition( void )
{
	// Observer.
	if ( player->IsObserver() )
		return;

	// Reset this each time we-recategorize, otherwise we have bogus friction when we jump into water and plunge downward really quickly
	player->m_surfaceFriction = 1.0f;

	// Doing this before we move may introduce a potential latency in water detection, but
	// doing it after can get us stuck on the bottom in water if the amount we move up
	// is less than the 1 pixel 'threshold' we're about to snap to.	Also, we'll call
	// this several times per frame, so we really need to avoid sticking to the bottom of
	// water on each call, and the converse case will correct itself if called twice.
	CheckWater();

	// If standing on a ladder we are not on ground.
	if ( player->GetMoveType() == MOVETYPE_LADDER )
	{
		SetGroundEntity( NULL );
		return;
	}

	// Check for a jump.
	if ( mv->m_vecVelocity.z > 250.0f )
	{
		SetGroundEntity( NULL );
		return;
	}

	// Calculate the start and end position.
	Vector vecStartPos = mv->GetAbsOrigin();
	Vector vecEndPos( mv->GetAbsOrigin().x, mv->GetAbsOrigin().y, ( mv->GetAbsOrigin().z - 2.0f ) );

	// NOTE YWB 7/5/07:  Since we're already doing a traceline here, we'll subsume the StayOnGround (stair debouncing) check into the main traceline we do here to see what we're standing on
	bool bUnderwater = ( player->GetWaterLevel() >= WL_Eyes );
	bool bMoveToEndPos = false;
	if ( player->GetMoveType() == MOVETYPE_WALK && 
		player->GetGroundEntity() != NULL && !bUnderwater )
	{
		// if walking and still think we're on ground, we'll extend trace down by stepsize so we don't bounce down slopes
		vecEndPos.z -= player->GetStepSize();
		bMoveToEndPos = true;
	}

	trace_t trace;
	TracePlayerBBox( vecStartPos, vecEndPos, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace );

	// Steep plane, not on ground.
	if ( trace.plane.normal.z < 0.7f )
	{
		// Test four sub-boxes, to see if any of them would have found shallower slope we could actually stand on.
		TracePlayerBBoxForGround( vecStartPos, vecEndPos, GetPlayerMins(), GetPlayerMaxs(), mv->m_nPlayerHandle.Get(), PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace );
		if ( trace.plane.normal[2] < 0.7f )
		{
			// Too steep.
			SetGroundEntity( NULL );
			if ( ( mv->m_vecVelocity.z > 0.0f ) && 
				( player->GetMoveType() != MOVETYPE_NOCLIP ) )
			{
				player->m_surfaceFriction = 0.25f;
			}
		}
		else
		{
			SetGroundEntity( &trace );
		}
	}
	else
	{
		// YWB:  This logic block essentially lifted from StayOnGround implementation
		if ( bMoveToEndPos &&
			!trace.startsolid &&				// not sure we need this check as fraction would == 0.0f?
			trace.fraction > 0.0f &&			// must go somewhere
			trace.fraction < 1.0f ) 			// must hit something
		{
			float flDelta = fabs( mv->GetAbsOrigin().z - trace.endpos.z );
			// HACK HACK:  The real problem is that trace returning that strange value 
			//  we can't network over based on bit precision of networking origins
			if ( flDelta > 0.5f * COORD_RESOLUTION )
			{
				Vector org = mv->GetAbsOrigin();
				org.z = trace.endpos.z;
				mv->SetAbsOrigin( org );
			}
		}
		SetGroundEntity( &trace );
	}

#ifndef CLIENT_DLL

	//Adrian: vehicle code handles for us.
	if ( player->IsInAVehicle() == false && player->GetTeamNumber() == TF_STORY_TEAM )
	{
		// If our gamematerial has changed, tell any player surface triggers that are watching
		IPhysicsSurfaceProps *physprops = MoveHelper()->GetSurfaceProps();
		surfacedata_t *pSurfaceProp = physprops->GetSurfaceData( trace.surface.surfaceProps );
		char cCurrGameMaterial = pSurfaceProp->game.material;
		if ( !player->GetGroundEntity() )
		{
			cCurrGameMaterial = 0;
		}

		// Changed?
		if ( player->m_chPreviousTextureType != cCurrGameMaterial )
		{
			CEnvPlayerSurfaceTrigger::SetPlayerSurface( player, cCurrGameMaterial );
		}

		player->m_chPreviousTextureType = cCurrGameMaterial;
	}
#endif
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::CheckWaterJump( void )
{
	Vector	flatforward;
	Vector	flatvelocity;
	float curspeed;

	// Jump button down?
	bool bJump = ( ( mv->m_nButtons & IN_JUMP ) != 0 );

	Vector forward, right;
	AngleVectors( mv->m_vecViewAngles, &forward, &right, NULL );  // Determine movement angles

	// Already water jumping.
	if (player->m_flWaterJumpTime)
		return;

	// Don't hop out if we just jumped in
	if (mv->m_vecVelocity[2] < -180)
		return; // only hop out if we are moving up

	// See if we are backing up
	flatvelocity[0] = mv->m_vecVelocity[0];
	flatvelocity[1] = mv->m_vecVelocity[1];
	flatvelocity[2] = 0;

	// Must be moving
	curspeed = VectorNormalize( flatvelocity );
	
#if 1
	// Copy movement amounts
	float fmove = mv->m_flForwardMove;
	float smove = mv->m_flSideMove;

	for ( int iAxis = 0; iAxis < 2; ++iAxis )
	{
		flatforward[iAxis] = forward[iAxis] * fmove + right[iAxis] * smove;
	}
#else
	// see if near an edge
	flatforward[0] = forward[0];
	flatforward[1] = forward[1];
#endif
	flatforward[2] = 0;
	VectorNormalize( flatforward );

	// Are we backing into water from steps or something?  If so, don't pop forward
	if ( curspeed != 0.0 && ( DotProduct( flatvelocity, flatforward ) < 0.0 ) && !bJump )
		return;

	Vector vecStart;
	// Start line trace at waist height (using the center of the player for this here)
 	vecStart= mv->GetAbsOrigin() + (GetPlayerMins() + GetPlayerMaxs() ) * 0.5;

	Vector vecEnd;
	VectorMA( vecStart, TF_WATERJUMP_FORWARD/*tf_waterjump_forward.GetFloat()*/, flatforward, vecEnd );
	
	trace_t tr;
	TracePlayerBBox( vecStart, vecEnd, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, tr );
	if ( tr.fraction < 1.0 )		// solid at waist
	{
		IPhysicsObject *pPhysObj = tr.m_pEnt->VPhysicsGetObject();
		if ( pPhysObj )
		{
			if ( pPhysObj->GetGameFlags() & FVPHYSICS_PLAYER_HELD )
				return;
		}

		vecStart.z = mv->GetAbsOrigin().z + player->GetViewOffset().z + WATERJUMP_HEIGHT; 
		VectorMA( vecStart, TF_WATERJUMP_FORWARD/*tf_waterjump_forward.GetFloat()*/, flatforward, vecEnd );
		VectorMA( vec3_origin, -50.0f, tr.plane.normal, player->m_vecWaterJumpVel );

		TracePlayerBBox( vecStart, vecEnd, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, tr );
		if ( tr.fraction == 1.0 )		// open at eye level
		{
			// Now trace down to see if we would actually land on a standable surface.
			VectorCopy( vecEnd, vecStart );
			vecEnd.z -= 1024.0f;
			TracePlayerBBox( vecStart, vecEnd, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, tr );
			if ( ( tr.fraction < 1.0f ) && ( tr.plane.normal.z >= 0.7 ) )
			{
				mv->m_vecVelocity[2] = TF_WATERJUMP_UP/*tf_waterjump_up.GetFloat()*/;		// Push up
				mv->m_nOldButtons |= IN_JUMP;		// Don't jump again until released
				player->AddFlag( FL_WATERJUMP );
				player->m_flWaterJumpTime = 2000.0f;	// Do this for 2 seconds
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::CheckFalling( void )
{
	// if we landed on the ground
	if ( player->GetGroundEntity() != NULL && !IsDead() )
	{
		// turn off the jumping flag if we're on ground after a jump
		if ( m_pTFPlayer->m_Shared.IsJumping() )
		{
			m_pTFPlayer->m_Shared.SetJumping( false );
		}
	}

	BaseClass::CheckFalling();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::Duck( void )
{
	// Don't allowing ducking in water.
	if ( ( ( player->GetWaterLevel() >= WL_Feet ) && ( player->GetGroundEntity() == NULL ) ) ||
		 player->GetWaterLevel() >= WL_Eyes )
	{
		mv->m_nButtons &= ~IN_DUCK;
	}

	BaseClass::Duck();
}
void CTFGameMovement::FullWalkMoveUnderwater()
{
	if ( player->GetWaterLevel() == WL_Waist )
	{
		CheckWaterJump();
	}

	// If we are falling again, then we must not trying to jump out of water any more.
	if ( ( mv->m_vecVelocity.z < 0.0f ) && player->m_flWaterJumpTime )
	{
		player->m_flWaterJumpTime = 0.0f;
	}

	// Was jump button pressed?
	if ( mv->m_nButtons & IN_JUMP )
	{
		CheckJumpButton();
	}
	else
	{
		mv->m_nOldButtons &= ~IN_JUMP;
	}

	// Perform regular water movement
	WaterMove();

	// Redetermine position vars
	CategorizePosition();

	// If we are on ground, no downward velocity.
	if ( player->GetGroundEntity() != NULL )
	{
		mv->m_vecVelocity[2] = 0;			
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::FullWalkMove()
{
	if ( !InWater() ) 
	{
		StartGravity();
	}

	// If we are leaping out of the water, just update the counters.
	if ( player->m_flWaterJumpTime )
	{
		// Try to jump out of the water (and check to see if we still are).
		WaterJump();
		TryPlayerMove();
		CheckWater();
		return;
	}

	// If we are swimming in the water, see if we are nudging against a place we can jump up out
	//  of, and, if so, start out jump.  Otherwise, if we are not moving up, then reset jump timer to 0
	if ( InWater() ) 
	{
		FullWalkMoveUnderwater();
		return;
	}

	if (mv->m_nButtons & IN_JUMP)
	{
		CheckJumpButton();
	}
	else
	{
		mv->m_nOldButtons &= ~IN_JUMP;
	}

	// Make sure velocity is valid.
	CheckVelocity();

	if (player->GetGroundEntity() != NULL)
	{
		mv->m_vecVelocity[2] = 0.0;
		Friction();
		WalkMove();
	}
	else
	{
		AirMove();
	}

	// Set final flags.
	CategorizePosition();

	// Add any remaining gravitational component if we are not in water.
	if ( !InWater() )
	{
		FinishGravity();
	}

	// If we are on ground, no downward velocity.
	if ( player->GetGroundEntity() != NULL )
	{
		mv->m_vecVelocity[2] = 0;
	}

	// Handling falling.
	CheckFalling();

	// Make sure velocity is valid.
	CheckVelocity();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::FullTossMove( void )
{
	trace_t pm;
	Vector move;

	// add velocity if player is moving 
	if ( (mv->m_flForwardMove != 0.0f) || (mv->m_flSideMove != 0.0f) || (mv->m_flUpMove != 0.0f))
	{
		Vector forward, right, up;
		float fmove, smove;
		Vector wishdir, wishvel;
		float wishspeed;
		int i;

		AngleVectors (mv->m_vecViewAngles, &forward, &right, &up);  // Determine movement angles

		// Copy movement amounts
		fmove = mv->m_flForwardMove;
		smove = mv->m_flSideMove;

		VectorNormalize (forward);  // Normalize remainder of vectors.
		VectorNormalize (right);    // 

		for (i=0 ; i<3 ; i++)       // Determine x and y parts of velocity
			wishvel[i] = forward[i]*fmove + right[i]*smove;

		wishvel[2] += mv->m_flUpMove;

		VectorCopy (wishvel, wishdir);   // Determine maginitude of speed of move
		wishspeed = VectorNormalize(wishdir);

		//
		// Clamp to server defined max speed
		//
		if (wishspeed > mv->m_flMaxSpeed)
		{
			VectorScale (wishvel, mv->m_flMaxSpeed/wishspeed, wishvel);
			wishspeed = mv->m_flMaxSpeed;
		}

		// Set pmove velocity
		Accelerate ( wishdir, wishspeed, sv_accelerate.GetFloat() );
	}

	if ( mv->m_vecVelocity[2] > 0 )
	{
		SetGroundEntity( NULL );
	}

	// If on ground and not moving, return.
	if ( player->GetGroundEntity() != NULL )
	{
		if (VectorCompare(player->GetBaseVelocity(), vec3_origin) &&
			VectorCompare(mv->m_vecVelocity, vec3_origin))
			return;
	}

	CheckVelocity();

	// add gravity
	if ( player->GetMoveType() == MOVETYPE_FLYGRAVITY )
	{
		AddGravity();
	}

	// move origin
	// Base velocity is not properly accounted for since this entity will move again after the bounce without
	// taking it into account
	VectorAdd (mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

	CheckVelocity();

	VectorScale (mv->m_vecVelocity, gpGlobals->frametime, move);
	VectorSubtract (mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

	PushEntity( move, &pm );	// Should this clear basevelocity

	CheckVelocity();

	if (pm.allsolid)
	{	
		// entity is trapped in another solid
		SetGroundEntity( &pm );
		mv->m_vecVelocity.Init();
		return;
	}

	if ( pm.fraction != 1.0f )
	{
		PerformFlyCollisionResolution( pm, move );
	}

	// Check for in water
	CheckWater();
}

//-----------------------------------------------------------------------------
// Purpose: Does the basic move attempting to climb up step heights.  It uses
//          the mv->GetAbsOrigin() and mv->m_vecVelocity.  It returns a new
//          new mv->GetAbsOrigin(), mv->m_vecVelocity, and mv->m_outStepHeight.
//-----------------------------------------------------------------------------
void CTFGameMovement::StepMove( Vector &vecDestination, trace_t &trace )
{
	trace_t saveTrace;
	saveTrace = trace;

	Vector vecEndPos;
	VectorCopy( vecDestination, vecEndPos );

	Vector vecPos, vecVel;
	VectorCopy( mv->GetAbsOrigin(), vecPos );
	VectorCopy( mv->m_vecVelocity, vecVel );

	bool bLowRoad = false;
	bool bUpRoad = true;

	// First try the "high road" where we move up and over obstacles
	if ( player->m_Local.m_bAllowAutoMovement )
	{
		// Trace up by step height
		VectorCopy( mv->GetAbsOrigin(), vecEndPos );
		vecEndPos.z += player->m_Local.m_flStepSize + DIST_EPSILON;
		TracePlayerBBox( mv->GetAbsOrigin(), vecEndPos, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace );
		if ( !trace.startsolid && !trace.allsolid )
		{
			mv->SetAbsOrigin( trace.endpos );
		}

		// Trace over from there
		TryPlayerMove();

		// Then trace back down by step height to get final position
		VectorCopy( mv->GetAbsOrigin(), vecEndPos );
		vecEndPos.z -= player->m_Local.m_flStepSize + DIST_EPSILON;
		TracePlayerBBox( mv->GetAbsOrigin(), vecEndPos, PlayerSolidMask(), COLLISION_GROUP_PLAYER_MOVEMENT, trace );
		// If the trace ended up in empty space, copy the end over to the origin.
		if ( !trace.startsolid && !trace.allsolid )
		{
			mv->SetAbsOrigin( trace.endpos );
		}

		// If we are not on the standable ground any more or going the "high road" didn't move us at all, then we'll also want to check the "low road"
		if ( ( trace.fraction != 1.0f && 
			trace.plane.normal[2] < 0.7 ) || VectorCompare( mv->GetAbsOrigin(), vecPos ) )
		{
			bLowRoad = true;
			bUpRoad = false;
		}
	}
	else
	{
		bLowRoad = true;
		bUpRoad = false;
	}

	if ( bLowRoad )
	{
		// Save off upward results
		Vector vecUpPos, vecUpVel;
		if ( bUpRoad )
		{
			VectorCopy( mv->GetAbsOrigin(), vecUpPos );
			VectorCopy( mv->m_vecVelocity, vecUpVel );
		}

		// Take the "low" road
		mv->SetAbsOrigin( vecPos );
		VectorCopy( vecVel, mv->m_vecVelocity );
		VectorCopy( vecDestination, vecEndPos );
		TryPlayerMove( &vecEndPos, &saveTrace );

		// Down results.
		Vector vecDownPos, vecDownVel;
		VectorCopy( mv->GetAbsOrigin(), vecDownPos );
		VectorCopy( mv->m_vecVelocity, vecDownVel );

		if ( bUpRoad )
		{
			float flUpDist = ( vecUpPos.x - vecPos.x ) * ( vecUpPos.x - vecPos.x ) + ( vecUpPos.y - vecPos.y ) * ( vecUpPos.y - vecPos.y );
			float flDownDist = ( vecDownPos.x - vecPos.x ) * ( vecDownPos.x - vecPos.x ) + ( vecDownPos.y - vecPos.y ) * ( vecDownPos.y - vecPos.y );
	
			// decide which one went farther
			if ( flUpDist >= flDownDist )
			{
				mv->SetAbsOrigin( vecUpPos );
				VectorCopy( vecUpVel, mv->m_vecVelocity );

				// copy z value from the Low Road move
				mv->m_vecVelocity.z = vecDownVel.z;
			}
		}
	}

	float flStepDist = mv->GetAbsOrigin().z - vecPos.z;
	if ( flStepDist > 0 )
	{
		mv->m_outStepHeight += flStepDist;
	}
}

bool CTFGameMovement::GameHasLadders() const
{
	return true;
}

void CTFGameMovement::SetGroundEntity( trace_t *pm )
{
	BaseClass::SetGroundEntity( pm );
	if ( pm && pm->m_pEnt )
	{
		m_pTFPlayer->m_Shared.SetAirDash( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFGameMovement::PlayerRoughLandingEffects( float fvol )
{
	if ( m_pTFPlayer && m_pTFPlayer->IsPlayerClass(TF_CLASS_SCOUT) )
	{
		// Scouts don't play rumble unless they take damage.
		if ( fvol < 1.0 )
		{
			fvol = 0;
		}
	}

	BaseClass::PlayerRoughLandingEffects( fvol );
}


//-----------------------------------------------------------------------------
// Purpose: 
// Input  : type - 
// Output : int
//-----------------------------------------------------------------------------
int CTFGameMovement::GetCheckInterval( IntervalType_t type )
{
	// HL2 ladders need to check every frame!!!
	if ( type == LADDER )
		return 1;

	return BaseClass::GetCheckInterval( type );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFGameMovement::IsForceMoveActive()
{
	LadderMove_t *lm = GetLadderMove();
	return lm->m_bForceLadderMove;
}

//-----------------------------------------------------------------------------
// Purpose: Debounce the USE button
//-----------------------------------------------------------------------------
void CTFGameMovement::SwallowUseKey()
{
	mv->m_nOldButtons |= IN_USE;
	player->m_afButtonPressed &= ~IN_USE;

	//GetTFPlayer()->m_bPlayUseDenySound = false;
}

#if !defined( CLIENT_DLL )
// This is a simple helper class to reserver a player sized hull at a spot, owned by the current player so that nothing
//  can move into this spot and cause us to get stuck when we get there
class CReservePlayerSpot : public CBaseEntity
{
	DECLARE_CLASS( CReservePlayerSpot, CBaseEntity )
public:
	static CReservePlayerSpot *ReserveSpot( CBasePlayer *owner, const Vector& org, const Vector& mins, const Vector& maxs, bool& validspot );

	virtual void Spawn();
};

CReservePlayerSpot *CReservePlayerSpot::ReserveSpot( 
	CBasePlayer *owner, const Vector& org, const Vector& mins, const Vector& maxs, bool& validspot )
{
	CReservePlayerSpot *spot = ( CReservePlayerSpot * )CreateEntityByName( "reserved_spot" );
	Assert( spot );

	spot->SetAbsOrigin( org );
	UTIL_SetSize( spot, mins, maxs );
	spot->SetOwnerEntity( owner );
	spot->Spawn();

	// See if spot is valid
	trace_t tr;
	UTIL_TraceHull(
		org, 
		org, 
		mins,
		maxs,
		MASK_PLAYERSOLID,
		owner,
		COLLISION_GROUP_PLAYER_MOVEMENT,
		&tr );

	validspot = !tr.startsolid;

	if ( !validspot )
	{
		Vector org2 = org + Vector( 0, 0, 1 );

		// See if spot is valid
		trace_t tr;
		UTIL_TraceHull(
			org2, 
			org2, 
			mins,
			maxs,
			MASK_PLAYERSOLID,
			owner,
			COLLISION_GROUP_PLAYER_MOVEMENT,
			&tr );
		validspot = !tr.startsolid;
	}

	return spot;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CReservePlayerSpot::Spawn()
{
	BaseClass::Spawn();

	SetSolid( SOLID_BBOX );
	SetMoveType( MOVETYPE_NONE );
	// Make entity invisible
	AddEffects( EF_NODRAW );
}

LINK_ENTITY_TO_CLASS( reserved_spot, CReservePlayerSpot );

#endif
//-----------------------------------------------------------------------------
// Purpose: 
// Input  : mounting - 
//			transit_speed - 
//			goalpos - 
//			*ladder - 
//-----------------------------------------------------------------------------
void CTFGameMovement::StartForcedMove( bool mounting, float transit_speed, const Vector& goalpos, CFuncLadder *ladder )
{
	LadderMove_t* lm = GetLadderMove();
	Assert( lm );
	// Already active, just ignore
	if ( lm->m_bForceLadderMove )
	{
		return;
	}

#if !defined( CLIENT_DLL )
	if ( ladder )
	{
		ladder->PlayerGotOn( GetTFPlayer() );

		// If the Ladder only wants to be there for automount checking, abort now
		if ( ladder->DontGetOnLadder() )
			return;
	}
		
	// Reserve goal slot here
	bool valid = false;
	lm->m_hReservedSpot = CReservePlayerSpot::ReserveSpot( 
		player, 
		goalpos, 
		GetPlayerMins( ( player->GetFlags() & FL_DUCKING ) ? true : false ), 
		GetPlayerMaxs( ( player->GetFlags() & FL_DUCKING ) ? true : false ), 
		valid );
	if ( !valid )
	{
		// FIXME:  Play a deny sound?
		if ( lm->m_hReservedSpot )
		{
			UTIL_Remove( lm->m_hReservedSpot );
			lm->m_hReservedSpot = NULL;
		}
		return;
	}
#endif

	// Use current player origin as start and new origin as dest
	lm->m_vecGoalPosition	= goalpos;
	lm->m_vecStartPosition	= mv->GetAbsOrigin();

	// Figure out how long it will take to make the gap based on transit_speed
	Vector delta = lm->m_vecGoalPosition - lm->m_vecStartPosition;

	float distance = delta.Length();
	
	Assert( transit_speed > 0.001f );

	// Compute time required to move that distance
	float transit_time = distance / transit_speed;
	if ( transit_time < 0.001f )
	{
		transit_time = 0.001f;
	}

	lm->m_bForceLadderMove	= true;
	lm->m_bForceMount		= mounting;

	lm->m_flStartTime		= gpGlobals->curtime;
	lm->m_flArrivalTime		= lm->m_flStartTime + transit_time;

	lm->m_hForceLadder		= ladder;

	// Don't get stuck during this traversal since we'll just be slamming the player origin
	player->SetMoveType( MOVETYPE_NONE );
	player->SetMoveCollide( MOVECOLLIDE_DEFAULT );
	player->SetSolid( SOLID_NONE );
	SetLadder( ladder );

	// Debounce the use key
	SwallowUseKey();
}

//-----------------------------------------------------------------------------
// Purpose: Returns false when finished
//-----------------------------------------------------------------------------
bool CTFGameMovement::ContinueForcedMove()
{
	LadderMove_t* lm = GetLadderMove();
	Assert( lm );
	Assert( lm->m_bForceLadderMove );

	// Suppress regular motion
	mv->m_flForwardMove = 0.0f;
	mv->m_flSideMove = 0.0f;
	mv->m_flUpMove = 0.0f;

	// How far along are we
	float frac = ( gpGlobals->curtime - lm->m_flStartTime ) / ( lm->m_flArrivalTime - lm->m_flStartTime );
	if ( frac > 1.0f )
	{
		lm->m_bForceLadderMove = false;
#if !defined( CLIENT_DLL )
		// Remove "reservation entity"
		if ( lm->m_hReservedSpot )
		{
			UTIL_Remove( lm->m_hReservedSpot );
			lm->m_hReservedSpot = NULL;
		}
#endif
	}

	frac = clamp( frac, 0.0f, 1.0f );

	// Move origin part of the way
	Vector delta = lm->m_vecGoalPosition - lm->m_vecStartPosition;

	// Compute interpolated position
	Vector org;
	VectorMA( lm->m_vecStartPosition, frac, delta, org );
	mv->SetAbsOrigin( org );

	// If finished moving, reset player to correct movetype (or put them on the ladder)
	if ( !lm->m_bForceLadderMove )
	{
		player->SetSolid( SOLID_BBOX );
		player->SetMoveType( MOVETYPE_WALK );

		if ( lm->m_bForceMount && lm->m_hForceLadder != NULL )
		{
			player->SetMoveType( MOVETYPE_LADDER );
			SetLadder( lm->m_hForceLadder );
		}

		// Zero out any velocity
		mv->m_vecVelocity.Init();
	}

	// Stil active
	return lm->m_bForceLadderMove;
}

//-----------------------------------------------------------------------------
// Purpose: Returns true if the player is on a ladder
// Input  : &trace - ignored
//-----------------------------------------------------------------------------
bool CTFGameMovement::OnLadder( trace_t &trace )
{
	if (BaseClass::OnLadder(trace))
	{
		return true;
	}
	else if (GetLadder() != NULL)
	{
		return true;
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : ladders - 
//			maxdist - 
//			**ppLadder - 
//			ladderOrigin - 
//-----------------------------------------------------------------------------
void CTFGameMovement::Findladder( float maxdist, CFuncLadder **ppLadder, Vector& ladderOrigin, const CFuncLadder *skipLadder )
{
	CFuncLadder *bestLadder = NULL;
	float bestDist = MAX_COORD_INTEGER;
	Vector bestOrigin;

	bestOrigin.Init();

	float maxdistSqr = maxdist * maxdist;


	int c = CFuncLadder::GetLadderCount();
	for ( int i = 0 ; i < c; i++ )
	{
		CFuncLadder *ladder = CFuncLadder::GetLadder( i );

		if ( !ladder->IsEnabled() )
			continue;

		if ( skipLadder && ladder == skipLadder )
			continue;

		Vector topPosition;
		Vector bottomPosition;

		ladder->GetTopPosition( topPosition );
		ladder->GetBottomPosition( bottomPosition );

		Vector closest;
		CalcClosestPointOnLineSegment( mv->GetAbsOrigin(), bottomPosition, topPosition, closest, NULL );

		float distSqr = ( closest - mv->GetAbsOrigin() ).LengthSqr();

		// Too far away
		if ( distSqr > maxdistSqr )
		{
			continue;
		}

		// Need to trace to see if it's clear
		trace_t tr;

		UTIL_TraceLine( mv->GetAbsOrigin(), closest, 
			MASK_PLAYERSOLID,
			player,
			COLLISION_GROUP_NONE,
			&tr );

		if ( tr.fraction != 1.0f &&
			 tr.m_pEnt &&
			 tr.m_pEnt != ladder )
		{
			// Try a trace stepped up from the ground a bit, in case there's something at ground level blocking us.
			float sizez = GetPlayerMaxs().z - GetPlayerMins().z;

			UTIL_TraceLine( mv->GetAbsOrigin() + Vector( 0, 0, sizez * 0.5f ), closest, 
				MASK_PLAYERSOLID,
				player,
				COLLISION_GROUP_NONE,
				&tr );

			if ( tr.fraction != 1.0f &&
				 tr.m_pEnt &&
				 tr.m_pEnt != ladder &&
				 !tr.m_pEnt->IsSolidFlagSet( FSOLID_TRIGGER ) )
			{
				continue;
			}
		}

		// See if this is the best one so far
		if ( distSqr < bestDist )
		{
			bestDist = distSqr;
			bestLadder = ladder;
			bestOrigin = closest;
		}
	}

	// Return best ladder spot
	*ppLadder = bestLadder;
	ladderOrigin = bestOrigin;

}

static bool NearbyDismountLessFunc( const NearbyDismount_t& lhs, const NearbyDismount_t& rhs )
{
	return lhs.distSqr < rhs.distSqr;
}

void CTFGameMovement::GetSortedDismountNodeList( const Vector &org, float radius, CFuncLadder *ladder, CUtlRBTree< NearbyDismount_t, int >& list )
{
	float radiusSqr = radius * radius;

	int i;
	int c = ladder->GetDismountCount();
	for ( i = 0; i < c; i++ )
	{
		CInfoLadderDismount *spot = ladder->GetDismount( i );
		if ( !spot )
			continue;

		float distSqr = ( spot->GetAbsOrigin() - org ).LengthSqr();
		if ( distSqr > radiusSqr )
			continue;

		NearbyDismount_t nd;
		nd.dismount = spot;
		nd.distSqr = distSqr;

		list.Insert( nd );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//			*ladder - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFGameMovement::ExitLadderViaDismountNode( CFuncLadder *ladder, bool strict, bool useAlternate )
{
	// Find the best ladder exit node
	float bestDot = -99999.0f;
	float bestDistance = 99999.0f;
	Vector bestDest;
	bool found = false;

	// For 'alternate' dismount
	bool foundAlternate = false;
	Vector alternateDest;
	float alternateDist = 99999.0f;

	CUtlRBTree< NearbyDismount_t, int >	nearbyDismounts( 0, 0, NearbyDismountLessFunc );

	GetSortedDismountNodeList( mv->GetAbsOrigin(), 100.0f, ladder, nearbyDismounts );

	int i;

	for ( i = nearbyDismounts.FirstInorder(); i != nearbyDismounts.InvalidIndex() ; i = nearbyDismounts.NextInorder( i ) )
	{
		CInfoLadderDismount *spot = nearbyDismounts[ i ].dismount;
		if ( !spot )
		{
			Assert( !"What happened to the spot!!!" );
			continue;
		}

		// See if it's valid to put the player there...
		Vector org = spot->GetAbsOrigin() + Vector( 0, 0, 1 );

		trace_t tr;
		UTIL_TraceHull(
			org, 
			org, 
			GetPlayerMins( ( player->GetFlags() & FL_DUCKING ) ? true : false ),
			GetPlayerMaxs( ( player->GetFlags() & FL_DUCKING ) ? true : false ),
			MASK_PLAYERSOLID,
			player,
			COLLISION_GROUP_PLAYER_MOVEMENT,
			&tr );

		// Nope...
		if ( tr.startsolid )
		{
			continue;
		}

		// Find the best dot product
		Vector vecToSpot = org - ( mv->GetAbsOrigin() + player->GetViewOffset() );
		vecToSpot.z = 0.0f;
		float d = VectorNormalize( vecToSpot );

		float dot = vecToSpot.Dot( m_vecForward );

		// We're not facing at it...ignore
		if ( dot < 0.5f )
		{
			if( useAlternate && d < alternateDist )
			{
				alternateDest = org;
				alternateDist = d;
				foundAlternate = true;
			}

			continue;
		}

		if ( dot > bestDot )
		{
			bestDest = org;
			bestDistance = d;
			bestDot = dot;
			found = true;
		}
	}

	if ( found )
	{
		// Require a more specific 
		if ( strict && 
			( ( bestDot < 0.7f ) || ( bestDistance > 40.0f ) ) )
		{
			return false;
		}

		StartForcedMove( false, player->MaxSpeed(), bestDest, NULL );
		return true;
	}

	if( useAlternate )
	{
		// Desperate. Don't refuse to let a person off of a ladder if it can be helped. Use the
		// alternate dismount if there is one.
		if( foundAlternate && alternateDist <= 60.0f )
		{
			StartForcedMove( false, player->MaxSpeed(), alternateDest, NULL );
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : bOnLadder - 
//-----------------------------------------------------------------------------
void CTFGameMovement::FullLadderMove()
{
	if (!GetLadder())
	{
		BaseClass::FullLadderMove();
		return;
	}

#if !defined( CLIENT_DLL )
	CFuncLadder *ladder = GetLadder();
	Assert( ladder );
	if ( !ladder )
	{
		return;
	}

	CheckWater();

	// Was jump button pressed?  If so, don't do anything here
	if ( mv->m_nButtons & IN_JUMP )
	{
		CheckJumpButton();
		return;
	}
	else
	{
		mv->m_nOldButtons &= ~IN_JUMP;
	}

	player->SetGroundEntity( NULL );

	// Remember old positions in case we cancel this movement
	Vector oldVelocity	= mv->m_vecVelocity;
	Vector oldOrigin	= mv->GetAbsOrigin();

	Vector topPosition;
	Vector bottomPosition;

	ladder->GetTopPosition( topPosition );
	ladder->GetBottomPosition( bottomPosition );

	// Compute parametric distance along ladder vector...
	float oldt;
	CalcDistanceSqrToLine( mv->GetAbsOrigin(), topPosition, bottomPosition, &oldt );
	
	// Perform the move accounting for any base velocity.
	VectorAdd (mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);
	TryPlayerMove();
	VectorSubtract (mv->m_vecVelocity, player->GetBaseVelocity(), mv->m_vecVelocity);

	// Pressed buttons are "changed(xor)" and'ed with the mask of currently held buttons
	int buttonsChanged	= ( mv->m_nOldButtons ^ mv->m_nButtons );	// These buttons have changed this frame
	int buttonsPressed = buttonsChanged & mv->m_nButtons;
	bool pressed_use = ( buttonsPressed & IN_USE ) ? true : false;
	bool pressing_forward_or_side = mv->m_flForwardMove != 0.0f || mv->m_flSideMove != 0.0f;

	Vector ladderVec = topPosition - bottomPosition;
	float LadderLength = VectorNormalize( ladderVec );
	// This test is not perfect by any means, but should help a bit
	bool moving_along_ladder = false;
	if ( pressing_forward_or_side )
	{
		float fwdDot = m_vecForward.Dot( ladderVec );
		if ( fabs( fwdDot ) > 0.9f )
		{
			moving_along_ladder = true;
		}
	}

	// Compute parametric distance along ladder vector...
	float newt;
	CalcDistanceSqrToLine( mv->GetAbsOrigin(), topPosition, bottomPosition, &newt );

	// Fudge of 2 units
	float tolerance = 1.0f / LadderLength;

	bool wouldleaveladder = false;
	// Moving pPast top or bottom?
	if ( newt < -tolerance )
	{
		wouldleaveladder = newt < oldt;
	}
	else if ( newt > ( 1.0f + tolerance ) )
	{
		wouldleaveladder = newt > oldt;
	}

	// See if we are near the top or bottom but not moving
	float dist1sqr, dist2sqr;

	dist1sqr = ( topPosition - mv->GetAbsOrigin() ).LengthSqr();
	dist2sqr = ( bottomPosition - mv->GetAbsOrigin() ).LengthSqr();

	float dist = MIN( dist1sqr, dist2sqr );
	bool neardismountnode = ( dist < 16.0f * 16.0f ) ? true : false;
	float ladderUnitsPerTick = ( MAX_CLIMB_SPEED * gpGlobals->interval_per_tick );
	bool neardismountnode2 = ( dist < ladderUnitsPerTick * ladderUnitsPerTick ) ? true : false;

	// Really close to node, cvar is set, and pressing a key, then simulate a +USE
	bool auto_dismount_use = ( neardismountnode2 && 
								sv_autoladderdismount.GetBool() && 
								pressing_forward_or_side && 
								!moving_along_ladder );

	bool fully_underwater = ( player->GetWaterLevel() == WL_Eyes ) ? true : false;

	// If the user manually pressed use or we're simulating it, then use_dismount will occur
	bool use_dismount = pressed_use || auto_dismount_use;

	if ( fully_underwater && !use_dismount )
	{
		// If fully underwater, we require looking directly at a dismount node 
		///  to "float off" a ladder mid way...
		if ( ExitLadderViaDismountNode( ladder, true ) )
		{
			// See if they +used a dismount point mid-span..
			return;
		}
	}

	// If the movement would leave the ladder and they're not automated or pressing use, disallow the movement
	if ( !use_dismount )
	{
		if ( wouldleaveladder )
		{
			// Don't let them leave the ladder if they were on it
			mv->m_vecVelocity = oldVelocity;
			mv->SetAbsOrigin( oldOrigin );
		}
		return;
	}

	// If the move would not leave the ladder and we're near close to the end, then just accept the move
	if ( !wouldleaveladder && !neardismountnode )
	{
		// Otherwise, if the move would leave the ladder, disallow it.
		if ( pressed_use )
		{
			if ( ExitLadderViaDismountNode( ladder, false, IsX360() ) )
			{
				// See if they +used a dismount point mid-span..
				return;
			}

			player->SetMoveType( MOVETYPE_WALK );
			player->SetMoveCollide( MOVECOLLIDE_DEFAULT );
			SetLadder( NULL );
			//GetTFPlayer()->m_bPlayUseDenySound = false;

			// Dismount with a bit of velocity in facing direction
			VectorScale( m_vecForward, USE_DISMOUNT_SPEED, mv->m_vecVelocity );
			mv->m_vecVelocity.z = 50;
		}
		return;
	}

	// Debounce the use key
	if ( pressed_use )
	{
		SwallowUseKey();
	}

	// Try auto exit, if possible
	if ( ExitLadderViaDismountNode( ladder, false, pressed_use ) )
	{
		return;
	}

	if ( wouldleaveladder )
	{
		// Otherwise, if the move would leave the ladder, disallow it.
		if ( pressed_use )
		{
			player->SetMoveType( MOVETYPE_WALK );
			player->SetMoveCollide( MOVECOLLIDE_DEFAULT );
			SetLadder( NULL );

			// Dismount with a bit of velocity in facing direction
			VectorScale( m_vecForward, USE_DISMOUNT_SPEED, mv->m_vecVelocity );
			mv->m_vecVelocity.z = 50;
		}
		else
		{
			mv->m_vecVelocity = oldVelocity;
			mv->SetAbsOrigin( oldOrigin );
		}
	}
#endif
}

bool CTFGameMovement::CheckLadderAutoMountEndPoint( CFuncLadder *ladder, const Vector& bestOrigin )
{
	// See if we're really near an endpoint
	if ( !ladder )
		return false;

	Vector top, bottom;
	ladder->GetTopPosition( top );
	ladder->GetBottomPosition( bottom );

	float d1, d2;

	d1 = ( top - mv->GetAbsOrigin() ).LengthSqr();
	d2 = ( bottom - mv->GetAbsOrigin() ).LengthSqr();

	if ( d1 > 16 * 16 && d2 > 16 * 16 )
		return false;

	Vector ladderAxis;

	if ( d1 < 16 * 16 )
	{
		// Close to top
		ladderAxis = bottom - top;
	}
	else
	{
		ladderAxis = top - bottom;
	}

	VectorNormalize( ladderAxis );

	if ( ladderAxis.Dot( m_vecForward ) > sv_ladderautomountdot.GetFloat() )
	{
		StartForcedMove( true, player->MaxSpeed(), bestOrigin, ladder );
		return true;
	}

	return false;
}

bool CTFGameMovement::CheckLadderAutoMountCone( CFuncLadder *ladder, const Vector& bestOrigin, float maxAngleDelta, float maxDistToLadder )
{
	// Never 'back' onto ladders or stafe onto ladders
	if ( ladder != NULL && 
		( mv->m_flForwardMove > 0.0f ) )
	{
		Vector top, bottom;
		ladder->GetTopPosition( top );
		ladder->GetBottomPosition( bottom );

		Vector ladderAxis = top - bottom;
		VectorNormalize( ladderAxis );

		Vector probe = mv->GetAbsOrigin();

		Vector closest;
		CalcClosestPointOnLineSegment( probe, bottom, top, closest, NULL );

		Vector vecToLadder = closest - probe;

		float dist = VectorNormalize( vecToLadder );

		Vector flatLadder = vecToLadder;
		flatLadder.z = 0.0f;
		Vector flatForward = m_vecForward;
		flatForward.z = 0.0f;

		VectorNormalize( flatLadder );
		VectorNormalize( flatForward );

		float facingDot = flatForward.Dot( flatLadder );
		float angle = acos( facingDot ) * 180 / M_PI;

		bool closetoladder = ( dist != 0.0f && dist < maxDistToLadder ) ? true : false;
		bool reallyclosetoladder = ( dist != 0.0f && dist < 4.0f ) ? true : false;

		bool facingladderaxis = ( angle < maxAngleDelta ) ? true : false;
		bool facingalongaxis = ( (float)fabs( ladderAxis.Dot( m_vecForward ) ) > sv_ladderautomountdot.GetFloat() ) ? true : false;
#if 0
		Msg( "close %i length %.3f maxdist %.3f facing %.3f dot %.3f ang %.3f\n",
			closetoladder ? 1 : 0,
			dist,
			maxDistToLadder,
			(float)fabs( ladderAxis.Dot( m_vecForward ) ),
			facingDot, 
			angle);
#endif

		// Tracker 21776:  Don't mount ladders this way if strafing
		bool strafing = ( fabs( mv->m_flSideMove ) < 1.0f ) ? false : true;

		if ( ( ( facingDot > 0.0f && !strafing ) || facingalongaxis  ) && 
			( facingladderaxis || reallyclosetoladder ) && 
			closetoladder )
		{
			StartForcedMove( true, player->MaxSpeed(), bestOrigin, ladder );
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: Must be facing toward ladder
// Input  : *ladder - 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFGameMovement::LookingAtLadder( CFuncLadder *ladder )
{
	if ( !ladder )
	{
		return false;
	}

	// Get ladder end points
	Vector top, bottom;
	ladder->GetTopPosition( top );
	ladder->GetBottomPosition( bottom );

	// Find closest point on ladder to player (could be an endpoint)
	Vector closest;
	CalcClosestPointOnLineSegment( mv->GetAbsOrigin(), bottom, top, closest, NULL );

	// Flatten our view direction to 2D
	Vector flatForward = m_vecForward;
	flatForward.z = 0.0f;

	// Because the ladder itself is not a solid, the player's origin may actually be 
	// permitted to pass it, and that will screw up our dot product.
	// So back up the player's origin a bit to do the facing calculation.
	Vector vecAdjustedOrigin = mv->GetAbsOrigin() - 8.0f * flatForward;

	// Figure out vector from player to closest point on ladder
	Vector vecToLadder = closest - vecAdjustedOrigin;

	// Flatten it to 2D
	Vector flatLadder = vecToLadder;
	flatLadder.z = 0.0f;

	// Normalize the vectors (unnecessary)
	VectorNormalize( flatLadder );
	VectorNormalize( flatForward );

	// Compute dot product to see if forward is in same direction as vec to ladder
	float facingDot = flatForward.Dot( flatLadder );

	float requiredDot = ( sv_ladder_useonly.GetBool() ) ? -0.99 : 0.0;

	// Facing same direction if dot > = requiredDot...
	bool facingladder = ( facingDot >= requiredDot );

	return facingladder;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &trace - 
//-----------------------------------------------------------------------------
bool CTFGameMovement::CheckLadderAutoMount( CFuncLadder *ladder, const Vector& bestOrigin )
{
#if !defined( CLIENT_DLL )

	if ( ladder != NULL )
	{
		StartForcedMove( true, player->MaxSpeed(), bestOrigin, ladder );
		return true;
	}

#endif
	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFGameMovement::LadderMove( void )
{

	if ( player->GetMoveType() == MOVETYPE_NOCLIP )
	{
		SetLadder( NULL );
		return false;
	}

	// If being forced to mount/dismount continue to act like we are on the ladder
	if ( IsForceMoveActive() && ContinueForcedMove() )
	{
		return true;
	}

	CFuncLadder *bestLadder = NULL;
	Vector bestOrigin( 0, 0, 0 );

	CFuncLadder *ladder = GetLadder();

	// Something 1) deactivated the ladder...  or 2) something external applied
	//  a force to us.  In either case  make the player fall, etc.
	if ( ladder && 
		 ( !ladder->IsEnabled() ||
		 ( player->GetBaseVelocity().LengthSqr() > 1.0f ) ) )
	{
		GetTFPlayer()->ExitLadder();
		ladder = NULL;
	}

	if ( !ladder )
	{
		Findladder( 64.0f, &bestLadder, bestOrigin, NULL );
	}

#if !defined (CLIENT_DLL)
	if( !ladder && bestLadder && sv_ladder_useonly.GetBool() )
	{
		//GetTFPlayer()->DisplayLadderHudHint();
	}
#endif

	int buttonsChanged	= ( mv->m_nOldButtons ^ mv->m_nButtons );	// These buttons have changed this frame
	int buttonsPressed = buttonsChanged & mv->m_nButtons;
	bool pressed_use = ( buttonsPressed & IN_USE ) ? true : false;

	// If I'm already moving on a ladder, use the previous ladder direction
	if ( !ladder && !pressed_use )
	{
		// If flying through air, allow mounting ladders if we are facing < 15 degress from the ladder and we are close
		if ( !ladder && !sv_ladder_useonly.GetBool() )
		{
			// Tracker 6625:  Don't need to be leaping to auto mount using this method...
			// But if we are on the ground, then we must not be backing into the ladder (Tracker 12961)
			bool onground = player->GetGroundEntity() ? true : false;
			if ( !onground || ( mv->m_flForwardMove > 0.0f ) )
			{
				if ( CheckLadderAutoMountCone( bestLadder, bestOrigin, 15.0f, 32.0f ) )
				{
					return true;
				}
			}
			
			// Pressing forward while looking at ladder and standing (or floating) near a mounting point
			if ( mv->m_flForwardMove > 0.0f )
			{
				if ( CheckLadderAutoMountEndPoint( bestLadder, bestOrigin ) )
				{
					return true;
				}
			}
		}

		return BaseClass::LadderMove();
	}

	if ( !ladder && 
		LookingAtLadder( bestLadder ) &&
		CheckLadderAutoMount( bestLadder, bestOrigin ) )
	{
		return true;
	}

	// Reassign the ladder
	ladder = GetLadder();
	if ( !ladder )
	{
		return BaseClass::LadderMove();
	}

	// Don't play the deny sound
	if ( pressed_use )
	{
		//GetTFPlayer()->m_bPlayUseDenySound = false;
	}

	// Make sure we are on the ladder
	player->SetMoveType( MOVETYPE_LADDER );
	player->SetMoveCollide( MOVECOLLIDE_DEFAULT );

	player->SetGravity( 0.0f );
	
	float forwardSpeed = 0.0f;
	float rightSpeed = 0.0f;

	float speed = player->MaxSpeed();


	if ( mv->m_nButtons & IN_BACK )
	{
		forwardSpeed -= speed;
	}
	
	if ( mv->m_nButtons & IN_FORWARD )
	{
		forwardSpeed += speed;
	}
	
	if ( mv->m_nButtons & IN_MOVELEFT )
	{
		rightSpeed -= speed;
	}
	
	if ( mv->m_nButtons & IN_MOVERIGHT )
	{
		rightSpeed += speed;
	}
	
	if ( mv->m_nButtons & IN_JUMP )
	{
		player->SetMoveType( MOVETYPE_WALK );
		// Remove from ladder
		SetLadder( NULL );

		// Jump in view direction
		Vector jumpDir = m_vecForward;

		// unless pressing backward or something like that
		if ( mv->m_flForwardMove < 0.0f )
		{
			jumpDir = -jumpDir;
		}

		VectorNormalize( jumpDir );

		VectorScale( jumpDir, MAX_CLIMB_SPEED, mv->m_vecVelocity );
		// Tracker 13558:  Don't add any extra z velocity if facing downward at all
		if ( m_vecForward.z >= 0.0f )
		{
			mv->m_vecVelocity.z = mv->m_vecVelocity.z + 50;
		}
		return false;
	}

	if ( forwardSpeed != 0 || rightSpeed != 0 )
	{
		// See if the player is looking toward the top or the bottom
		Vector velocity;

		VectorScale( m_vecForward, forwardSpeed, velocity );
		VectorMA( velocity, rightSpeed, m_vecRight, velocity );

		VectorNormalize( velocity );

		Vector ladderUp;
		ladder->ComputeLadderDir( ladderUp );
		VectorNormalize( ladderUp );

		Vector topPosition;
		Vector bottomPosition;

		ladder->GetTopPosition( topPosition );
		ladder->GetBottomPosition( bottomPosition );

		// Check to see if we've mounted the ladder in a bogus spot and, if so, just fall off the ladder...
		float dummyt = 0.0f;
		float distFromLadderSqr = CalcDistanceSqrToLine( mv->GetAbsOrigin(), topPosition, bottomPosition, &dummyt );
		if ( distFromLadderSqr > 36.0f )
		{
			// Uh oh, we fell off zee ladder...
			player->SetMoveType( MOVETYPE_WALK );
			// Remove from ladder
			SetLadder( NULL );
			return false;
		}

		bool ishorizontal = fabs( topPosition.z - bottomPosition.z ) < 64.0f ? true : false;

		float changeover = ishorizontal ? 0.0f : 0.3f;

		float factor = 1.0f;
		if ( velocity.z >= 0 )
		{
			float dotTop = ladderUp.Dot( velocity );
			if ( dotTop < -changeover )
			{
				// Aimed at bottom
				factor = -1.0f;
			}
		}
		else
		{
			float dotBottom = -ladderUp.Dot( velocity );
			if ( dotBottom > changeover )
			{
				factor = -1.0f;
			}
		}

#ifdef _XBOX
		if( sv_ladders_useonly.GetBool() )
		{
			// Stick up climbs up, stick down climbs down. No matter which way you're looking.
			if ( mv->m_nButtons & IN_FORWARD )
			{
				factor = 1.0f;
			}
			else if( mv->m_nButtons & IN_BACK )
			{
				factor = -1.0f;
			}
		}
#endif//_XBOX

		mv->m_vecVelocity = MAX_CLIMB_SPEED * factor * ladderUp;
	}
	else
	{
		mv->m_vecVelocity.Init();
	}

	return true;
}