//========= Copyright Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#include "cbase.h"
#include "vehicle_base.h"
#include "engine/IEngineSound.h"
#include "in_buttons.h"
#include "ammodef.h"
#include "IEffects.h"
#include "beam_shared.h"
#include "soundenvelope.h"
#include "decals.h"
#include "soundent.h"
#include "te_effect_dispatch.h"
#include "ndebugoverlay.h"
#include "movevars_shared.h"
#include "bone_setup.h"
#include "ai_basenpc.h"
#include "ai_hint.h"
#include "globalstate.h"
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

#define	VEHICLE_HITBOX_DRIVER		1
#define LOCK_SPEED					10
#define JEEP_GUN_YAW				"vehicle_weapon_yaw"
#define JEEP_GUN_PITCH				"vehicle_weapon_pitch"
#define JEEP_GUN_SPIN				"gun_spin"
#define	JEEP_GUN_SPIN_RATE			20

#define CANNON_MAX_UP_PITCH			20
#define CANNON_MAX_DOWN_PITCH		20
#define CANNON_MAX_LEFT_YAW			90
#define CANNON_MAX_RIGHT_YAW		90

#define OVERTURNED_EXIT_WAITTIME	2.0f

#define JEEP_AMMOCRATE_HITGROUP		5
#define JEEP_WHEEL_COUNT			4

#define JEEP_STEERING_SLOW_ANGLE	50.0f
#define JEEP_STEERING_FAST_ANGLE	15.0f

#define VEHICLE_ROLE_PASSENGER 2

#define	JEEP_AMMO_CRATE_CLOSE_DELAY	2.0f

#define JEEP_DELTA_LENGTH_MAX	12.0f			// 1 foot
#define JEEP_FRAMETIME_MIN		1e-6

ConVar	g_jeepexitspeed( "g_jeepexitspeed", "100", FCVAR_CHEAT );

//=============================================================================
//
// Jeep water data.
//
struct JeepWaterData_t
{
	bool		m_bWheelInWater[JEEP_WHEEL_COUNT];
	bool		m_bWheelWasInWater[JEEP_WHEEL_COUNT];
	Vector		m_vecWheelContactPoints[JEEP_WHEEL_COUNT];
	float		m_flNextRippleTime[JEEP_WHEEL_COUNT];
	bool		m_bBodyInWater;
	bool		m_bBodyWasInWater;

	DECLARE_SIMPLE_DATADESC();
};

BEGIN_SIMPLE_DATADESC( JeepWaterData_t )
	DEFINE_ARRAY( m_bWheelInWater,			FIELD_BOOLEAN,	JEEP_WHEEL_COUNT ),
	DEFINE_ARRAY( m_bWheelWasInWater,			FIELD_BOOLEAN,	JEEP_WHEEL_COUNT ),
	DEFINE_ARRAY( m_vecWheelContactPoints,	FIELD_VECTOR,	JEEP_WHEEL_COUNT ),
	DEFINE_ARRAY( m_flNextRippleTime,			FIELD_TIME,		JEEP_WHEEL_COUNT ),
	DEFINE_FIELD( m_bBodyInWater,				FIELD_BOOLEAN ),
	DEFINE_FIELD( m_bBodyWasInWater,			FIELD_BOOLEAN ),
END_DATADESC()	

//-----------------------------------------------------------------------------
// Purpose: Four wheel physics vehicle server vehicle with weaponry
//-----------------------------------------------------------------------------
class CJeepFourWheelServerVehicle : public CFourWheelServerVehicle
{
	typedef CFourWheelServerVehicle BaseClass;

public:
	int			GetExitAnimToUse( Vector &vecEyeExitEndpoint, bool &bAllPointsBlocked );
	virtual void	ItemPostFrame( CBasePlayer *player );
	virtual void	HandlePassengerEntry( CBaseCombatCharacter *pPassenger, bool bAllowEntryOutsideZone = false );
	//virtual bool	HandlePassengerExit( CBaseCombatCharacter *pPassenger );
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CTFVehicle : public CPropVehicleDriveable
{
	DECLARE_CLASS( CTFVehicle, CPropVehicleDriveable );

public:

	DECLARE_SERVERCLASS();
	DECLARE_DATADESC();

	CTFVehicle( void );

	// CPropVehicle
	virtual void	ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMoveData );
	virtual void	DriveVehicle( float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased );
	virtual void	SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move );
	virtual void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	virtual void	DampenEyePosition( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles );
	virtual bool	AllowBlockedExit( CBasePlayer *pPlayer, int nRole ) { return false; }
	virtual bool	CanEnterVehicle( CBaseEntity *pEntity );
	virtual bool	CanExitVehicle( CBaseEntity *pEntity );
	virtual bool	IsVehicleBodyInWater() { return m_WaterData.m_bBodyInWater; }

	// CBaseEntity
	void			Think(void);
	void			Precache( void );
	void			Spawn( void ); 
	void			OnRestore( void );


	virtual void	Event_Killed( const CTakeDamageInfo &info );
	virtual void	CreateServerVehicle( void );
	virtual Vector	BodyTarget( const Vector &posSrc, bool bNoisy = true );
	virtual void	TraceAttack( const CTakeDamageInfo &info, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator );
	virtual int		OnTakeDamage( const CTakeDamageInfo &info );
	virtual void	EnterVehicle( CBasePlayer *pPlayer );
	virtual void	ExitVehicle( int nRole );

	void			AimGunAt( Vector *endPos, float flInterval );
	bool			TauCannonHasBeenCutOff( void ) { return m_bGunHasBeenCutOff; }

	const char		*GetTracerType( void ) { return "AR2Tracer"; }
	void			DoImpactEffect( trace_t &tr, int nDamageType );

	bool HeadlightIsOn( void ) { return m_bHeadlightIsOn; }
	void HeadlightTurnOn( void ) { m_bHeadlightIsOn = true; }
	void HeadlightTurnOff( void ) { m_bHeadlightIsOn = false; }

	// custom passenger stuff
	virtual CBaseEntity *GetDriver( void );
	virtual CBaseEntity *GetPassenger( void );

private:
	CNetworkHandle( CBasePlayer, m_hPlayerPassenger );

	void		FireCannon( void );
	void		ChargeCannon( void );
	void		FireChargedCannon( void );

	void		DrawBeam( const Vector &startPos, const Vector &endPos, float width );
	void		StopChargeSound( void );
	void		GetCannonAim( Vector *resultDir );

	void		InitWaterData( void );
	void		HandleWater( void );
	bool		CheckWater( void );
	void		CheckWaterLevel( void );
	void		CreateSplash( const Vector &vecPosition );
	void		CreateRipple( const Vector &vecPosition );

	void		UpdateSteeringAngle( void );
	void		CreateDangerSounds( void );

	void		ComputePDControllerCoefficients( float *pCoefficientsOut, float flFrequency, float flDampening, float flDeltaTime );
	void		DampenForwardMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime );
	void		DampenUpMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime );

	void		InputStartRemoveTauCannon( inputdata_t &inputdata );
	void		InputFinishRemoveTauCannon( inputdata_t &inputdata );

private:

	bool			m_bGunHasBeenCutOff;
	float			m_flDangerSoundTime;
	int				m_nBulletType;
	bool			m_bCannonCharging;
	float			m_flCannonTime;
	float			m_flCannonChargeStartTime;
	Vector			m_vecGunOrigin;
	CSoundPatch		*m_sndCannonCharge;
	int				m_nSpinPos;
	float			m_aimYaw;
	float			m_aimPitch;
	float			m_throttleDisableTime;
	float			m_flAmmoCrateCloseTime;

	// handbrake after the fact to keep vehicles from rolling
	float			m_flHandbrakeTime;
	bool			m_bInitialHandbrake;
	
	float			m_flOverturnedTime;

	Vector			m_vecLastEyePos;
	Vector			m_vecLastEyeTarget;
	Vector			m_vecEyeSpeed;
	Vector			m_vecTargetSpeed;

	JeepWaterData_t	m_WaterData;

	int				m_iNumberOfEntries;
	int				m_nAmmoType;

	// Seagull perching
	float			m_flPlayerExitedTime;	// Time at which the player last left this vehicle
	float			m_flLastSawPlayerAt;	// Time at which we last saw the player
	EHANDLE			m_hLastPlayerInVehicle;

	CNetworkVar( string_t, m_iszSmokeParticleName );
	CNetworkVar( string_t, m_iszFireParticleName );

	CNetworkVar( bool, m_bHeadlightIsOn );
	CNetworkVar( bool, m_bEnableDamageEffects );
	
	int				m_nMaxHealthOverride;
};

BEGIN_DATADESC( CTFVehicle )
	DEFINE_FIELD( m_bGunHasBeenCutOff, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flDangerSoundTime, FIELD_TIME ),
	DEFINE_FIELD( m_nBulletType, FIELD_INTEGER ),
	DEFINE_FIELD( m_bCannonCharging, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flCannonTime, FIELD_TIME ),
	DEFINE_FIELD( m_flCannonChargeStartTime, FIELD_TIME ),
	DEFINE_FIELD( m_vecGunOrigin, FIELD_POSITION_VECTOR ),
	DEFINE_SOUNDPATCH( m_sndCannonCharge ),
	DEFINE_FIELD( m_nSpinPos, FIELD_INTEGER ),
	DEFINE_FIELD( m_aimYaw, FIELD_FLOAT ),
	DEFINE_FIELD( m_aimPitch, FIELD_FLOAT ),
	DEFINE_FIELD( m_throttleDisableTime, FIELD_TIME ),
	DEFINE_FIELD( m_flHandbrakeTime, FIELD_TIME ),
	DEFINE_FIELD( m_bInitialHandbrake, FIELD_BOOLEAN ),
	DEFINE_FIELD( m_flOverturnedTime, FIELD_TIME ),
	DEFINE_FIELD( m_flAmmoCrateCloseTime, FIELD_FLOAT ),
	DEFINE_FIELD( m_vecLastEyePos, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecLastEyeTarget, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecEyeSpeed, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_vecTargetSpeed, FIELD_POSITION_VECTOR ),
	DEFINE_FIELD( m_bHeadlightIsOn, FIELD_BOOLEAN ),
	DEFINE_EMBEDDED( m_WaterData ),

	DEFINE_FIELD( m_iNumberOfEntries, FIELD_INTEGER ),
	DEFINE_FIELD( m_nAmmoType, FIELD_INTEGER ),

	DEFINE_FIELD( m_flPlayerExitedTime, FIELD_TIME ),
	DEFINE_FIELD( m_flLastSawPlayerAt, FIELD_TIME ),
	DEFINE_FIELD( m_hLastPlayerInVehicle, FIELD_EHANDLE ),

	DEFINE_KEYFIELD( m_nMaxHealthOverride,		FIELD_INTEGER, "vehicle_health"),
	DEFINE_KEYFIELD( m_iszSmokeParticleName,	FIELD_STRING, "smoke_particle"),
	DEFINE_KEYFIELD( m_iszFireParticleName,		FIELD_STRING, "fire_particle"),

	DEFINE_INPUTFUNC( FIELD_VOID, "StartRemoveTauCannon", InputStartRemoveTauCannon ),
	DEFINE_INPUTFUNC( FIELD_VOID, "FinishRemoveTauCannon", InputFinishRemoveTauCannon ),

END_DATADESC()

IMPLEMENT_SERVERCLASS_ST( CTFVehicle, DT_TFVehicle )
	SendPropEHandle( SENDINFO( m_hPlayerPassenger ) ),
	SendPropBool( SENDINFO( m_bHeadlightIsOn ) ),
	SendPropBool( SENDINFO( m_bEnableDamageEffects ) ),
	// I don't like networking strings, but there's nothing else we can really do here
	SendPropStringT( SENDINFO( m_iszSmokeParticleName ) ),
	SendPropStringT( SENDINFO( m_iszFireParticleName ) ),
END_SEND_TABLE();

LINK_ENTITY_TO_CLASS( prop_vehicle_jeep, CTFVehicle );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFVehicle::CTFVehicle( void )
{
	m_bHasGun = true;
	m_bGunHasBeenCutOff = false;
	m_bCannonCharging = false;
	m_flCannonChargeStartTime = 0;
	m_flCannonTime = 0;
	m_nBulletType = -1;
	m_flOverturnedTime = 0.0f;
	m_iNumberOfEntries = 0;

	m_vecEyeSpeed.Init();

	InitWaterData();
	m_nMaxHealthOverride = 100;
	m_bUnableToFire = true;
	m_flAmmoCrateCloseTime = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::CreateServerVehicle( void )
{
	// Create our armed server vehicle
	m_pServerVehicle = new CJeepFourWheelServerVehicle();
	m_pServerVehicle->SetVehicle( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::Precache( void )
{
	PrecacheScriptSound( "PropJeep.AmmoClose" );
	PrecacheScriptSound( "PropJeep.FireCannon" );
	PrecacheScriptSound( "PropJeep.FireChargedCannon" );
	PrecacheScriptSound( "PropJeep.AmmoOpen" );

	if ( m_iszSmokeParticleName.Get() != NULL_STRING )
		PrecacheParticleSystem( STRING( m_iszSmokeParticleName.Get() ) );

	if ( m_iszFireParticleName.Get() != NULL_STRING )
		PrecacheParticleSystem( STRING( m_iszFireParticleName.Get() ) );

	BaseClass::Precache();
}

//------------------------------------------------
// Spawn
//------------------------------------------------
void CTFVehicle::Spawn( void )
{
	// Setup vehicle as a real-wheels car.
	SetVehicleType( VEHICLE_TYPE_CAR_WHEELS );

	BaseClass::Spawn();
	m_flHandbrakeTime = gpGlobals->curtime + 0.1;
	m_bInitialHandbrake = false;

	m_VehiclePhysics.SetHasBrakePedal( false );

	m_flMinimumSpeedToEnterExit = LOCK_SPEED;

	m_nBulletType = GetAmmoDef()->Index("GaussEnergy");

	if ( m_bHasGun )
	{
		SetBodygroup( 1, true );
	}
	else
	{
		SetBodygroup( 1, false );
	}

	// Initialize pose parameters
	SetPoseParameter( JEEP_GUN_YAW, 0 );
	SetPoseParameter( JEEP_GUN_PITCH, 0 );
	m_nSpinPos = 0;
	SetPoseParameter( JEEP_GUN_SPIN, m_nSpinPos );
	m_aimYaw = 0;
	m_aimPitch = 0;

	AddSolidFlags( FSOLID_NOT_STANDABLE );

	CAmmoDef *pAmmoDef = GetAmmoDef();
	m_nAmmoType = pAmmoDef->Index("GaussEnergy");

	m_takedamage = DAMAGE_YES;

	SetMaxHealth( m_nMaxHealthOverride );
	SetHealth( m_nMaxHealthOverride );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &tr - 
//			nDamageType - 
//-----------------------------------------------------------------------------
void CTFVehicle::DoImpactEffect( trace_t &tr, int nDamageType )
{
	//Draw our beam
	DrawBeam( tr.startpos, tr.endpos, 2.4 );

	if ( (tr.surface.flags & SURF_SKY) == false )
	{
		CPVSFilter filter( tr.endpos );
		te->GaussExplosion( filter, 0.0f, tr.endpos, tr.plane.normal, 0 );

		UTIL_ImpactTrace( &tr, m_nBulletType );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::TraceAttack( const CTakeDamageInfo &inputInfo, const Vector &vecDir, trace_t *ptr, CDmgAccumulator *pAccumulator )
{
	CTakeDamageInfo info = inputInfo;
	if ( ptr->hitbox != VEHICLE_HITBOX_DRIVER )
	{
		if ( inputInfo.GetDamageType() & DMG_BULLET )
		{
			info.ScaleDamage( 0.0001 );
		}
	}

	BaseClass::TraceAttack( info, vecDir, ptr, pAccumulator );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CTFVehicle::OnTakeDamage( const CTakeDamageInfo &inputInfo )
{
	BaseClass::OnTakeDamage( inputInfo );

	//Do scaled up physics damage to the car
	CTakeDamageInfo info = inputInfo;
	info.ScaleDamage( 25 );
	
	// HACKHACK: Scale up grenades until we get a better explosion/pressure damage system
	if ( inputInfo.GetDamageType() & DMG_BLAST 
		|| inputInfo.GetDamageType() & DMG_CLUB 
		|| inputInfo.GetDamageType() & DMG_BULLET )
	{
		info.SetDamageForce( inputInfo.GetDamageForce() * 10 );
	}

	VPhysicsTakeDamage( info );

	if ( GetHealth() <= GetMaxHealth() * 0.45f )
		m_bEnableDamageEffects = true;

	// reset the damage
	info.SetDamage( inputInfo.GetDamage() );

	// small amounts of shock damage disrupt the car, but aren't transferred to the player
	if ( info.GetDamageType() == DMG_SHOCK )
	{
		if ( info.GetDamage() <= 10 )
		{
			// take 10% damage and make the engine stall
			info.ScaleDamage( 0.1 );
			m_throttleDisableTime = gpGlobals->curtime + 2;
		}
	}

	//Check to do damage to driver
	if ( GetDriver() )
	{
		//Take no damage from physics damages
		if ( info.GetDamageType() & DMG_CRUSH )
			return 0;

		// Take the damage (strip out the DMG_BLAST)
		info.SetDamageType( info.GetDamageType() & (~DMG_BLAST) );
		GetDriver()->TakeDamage( info );
	}

	return 0;
}

void CTFVehicle::Event_Killed( const CTakeDamageInfo &info )
{
	if ( GetDriver() )
		GetDriver()->Event_Killed( info );

	BaseClass::Event_Killed( info );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
Vector CTFVehicle::BodyTarget( const Vector &posSrc, bool bNoisy )
{
	Vector	shotPos;
	matrix3x4_t	matrix;

	int eyeAttachmentIndex = LookupAttachment("vehicle_driver_eyes");
	GetAttachment( eyeAttachmentIndex, matrix );
	MatrixGetColumn( matrix, 3, shotPos );

	if ( bNoisy )
	{
		shotPos[0] += random->RandomFloat( -8.0f, 8.0f );
		shotPos[1] += random->RandomFloat( -8.0f, 8.0f );
		shotPos[2] += random->RandomFloat( -8.0f, 8.0f );
	}

	return shotPos;
}

//-----------------------------------------------------------------------------
// Purpose: Aim Gun at a target
//-----------------------------------------------------------------------------
void CTFVehicle::AimGunAt( Vector *endPos, float flInterval )
{
	Vector	aimPos = *endPos;

	// See if the gun should be allowed to aim
	if ( IsOverturned() || m_bEngineLocked )
	{
		SetPoseParameter( JEEP_GUN_YAW, 0 );
		SetPoseParameter( JEEP_GUN_PITCH, 0 );
		SetPoseParameter( JEEP_GUN_SPIN, 0 );
		return;

		// Make the gun go limp and look "down"
		Vector	v_forward, v_up;
		AngleVectors( GetLocalAngles(), NULL, &v_forward, &v_up );
		aimPos = WorldSpaceCenter() + ( v_forward * -32.0f ) - Vector( 0, 0, 128.0f );
	}

	matrix3x4_t gunMatrix;
	GetAttachment( LookupAttachment("gun_ref"), gunMatrix );

	// transform the enemy into gun space
	Vector localEnemyPosition;
	VectorITransform( aimPos, gunMatrix, localEnemyPosition );

	// do a look at in gun space (essentially a delta-lookat)
	QAngle localEnemyAngles;
	VectorAngles( localEnemyPosition, localEnemyAngles );
	
	// convert to +/- 180 degrees
	localEnemyAngles.x = UTIL_AngleDiff( localEnemyAngles.x, 0 );	
	localEnemyAngles.y = UTIL_AngleDiff( localEnemyAngles.y, 0 );

	float targetYaw = m_aimYaw + localEnemyAngles.y;
	float targetPitch = m_aimPitch + localEnemyAngles.x;
	
	// Constrain our angles
	float newTargetYaw	= clamp( targetYaw, -CANNON_MAX_LEFT_YAW, CANNON_MAX_RIGHT_YAW );
	float newTargetPitch = clamp( targetPitch, -CANNON_MAX_DOWN_PITCH, CANNON_MAX_UP_PITCH );

	// If the angles have been clamped, we're looking outside of our valid range
	if ( fabs(newTargetYaw-targetYaw) > 1e-4 || fabs(newTargetPitch-targetPitch) > 1e-4 )
	{
		m_bUnableToFire = true;
	}

	targetYaw = newTargetYaw;
	targetPitch = newTargetPitch;

	// Exponentially approach the target
	float yawSpeed = 8;
	float pitchSpeed = 8;

	m_aimYaw = UTIL_Approach( targetYaw, m_aimYaw, yawSpeed );
	m_aimPitch = UTIL_Approach( targetPitch, m_aimPitch, pitchSpeed );

	SetPoseParameter( JEEP_GUN_YAW, -m_aimYaw);
	SetPoseParameter( JEEP_GUN_PITCH, -m_aimPitch );

	InvalidateBoneCache();

	// read back to avoid drift when hitting limits
	// as long as the velocity is less than the delta between the limit and 180, this is fine.
	m_aimPitch = -GetPoseParameter( JEEP_GUN_PITCH );
	m_aimYaw = -GetPoseParameter( JEEP_GUN_YAW );

	// Now draw crosshair for actual aiming point
	Vector	vecMuzzle, vecMuzzleDir;
	QAngle	vecMuzzleAng;

	GetAttachment( "Muzzle", vecMuzzle, vecMuzzleAng );
	AngleVectors( vecMuzzleAng, &vecMuzzleDir );

	trace_t	tr;
	UTIL_TraceLine( vecMuzzle, vecMuzzle + (vecMuzzleDir * MAX_TRACE_LENGTH), MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );

		// see if we hit something, if so, adjust endPos to hit location
	if ( tr.fraction < 1.0 )
	{
		m_vecGunCrosshair = vecMuzzle + ( vecMuzzleDir * MAX_TRACE_LENGTH * tr.fraction );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::InitWaterData( void )
{
	m_WaterData.m_bBodyInWater = false;
	m_WaterData.m_bBodyWasInWater = false;

	for ( int iWheel = 0; iWheel < JEEP_WHEEL_COUNT; ++iWheel )
	{
		m_WaterData.m_bWheelInWater[iWheel] = false;
		m_WaterData.m_bWheelWasInWater[iWheel] = false;
		m_WaterData.m_vecWheelContactPoints[iWheel].Init();
		m_WaterData.m_flNextRippleTime[iWheel] = 0;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::HandleWater( void )
{
	// Only check the wheels and engine in water if we have a driver (player).
	if ( !GetDriver() )
		return;

	// Check to see if we are in water.
	if ( CheckWater() )
	{
		for ( int iWheel = 0; iWheel < JEEP_WHEEL_COUNT; ++iWheel )
		{
			// Create an entry/exit splash!
			if ( m_WaterData.m_bWheelInWater[iWheel] != m_WaterData.m_bWheelWasInWater[iWheel] )
			{
				CreateSplash( m_WaterData.m_vecWheelContactPoints[iWheel] );
				CreateRipple( m_WaterData.m_vecWheelContactPoints[iWheel] );
			}
			
			// Create ripples.
			if ( m_WaterData.m_bWheelInWater[iWheel] && m_WaterData.m_bWheelWasInWater[iWheel] )
			{
				if ( m_WaterData.m_flNextRippleTime[iWheel] < gpGlobals->curtime )
				{
					// Stagger ripple times
					m_WaterData.m_flNextRippleTime[iWheel] = gpGlobals->curtime + RandomFloat( 0.1, 0.3 );
					CreateRipple( m_WaterData.m_vecWheelContactPoints[iWheel] );
				}
			}
		}
	}

	// Save of data from last think.
	for ( int iWheel = 0; iWheel < JEEP_WHEEL_COUNT; ++iWheel )
	{
		m_WaterData.m_bWheelWasInWater[iWheel] = m_WaterData.m_bWheelInWater[iWheel];
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFVehicle::CheckWater( void )
{
	bool bInWater = false;

	// Check all four wheels.
	for ( int iWheel = 0; iWheel < JEEP_WHEEL_COUNT; ++iWheel )
	{
		// Get the current wheel and get its contact point.
		IPhysicsObject *pWheel = m_VehiclePhysics.GetWheel( iWheel );
		if ( !pWheel )
			continue;

		// Check to see if we hit water.
		if ( pWheel->GetContactPoint( &m_WaterData.m_vecWheelContactPoints[iWheel], NULL ) )
		{
			m_WaterData.m_bWheelInWater[iWheel] = ( UTIL_PointContents( m_WaterData.m_vecWheelContactPoints[iWheel] ) & MASK_WATER ) ? true : false;
			if ( m_WaterData.m_bWheelInWater[iWheel] )
			{
				bInWater = true;
			}
		}
	}

	// Check the body and the BONNET.
	int iEngine = LookupAttachment( "vehicle_engine" );
	Vector vecEnginePoint;
	QAngle vecEngineAngles;
	GetAttachment( iEngine, vecEnginePoint, vecEngineAngles );

	m_WaterData.m_bBodyInWater = ( UTIL_PointContents( vecEnginePoint ) & MASK_WATER ) ? true : false;
	if ( m_WaterData.m_bBodyInWater )
	{
		if ( !m_VehiclePhysics.IsEngineDisabled() )
		{
			m_VehiclePhysics.SetDisableEngine( true );
		}
	}
	else
	{
		if ( m_VehiclePhysics.IsEngineDisabled() )
		{
			m_VehiclePhysics.SetDisableEngine( false );
		}
	}

	if ( bInWater )
	{
		// Check the player's water level.
		CheckWaterLevel();
	}

	return bInWater;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::CheckWaterLevel( void )
{
	CBaseEntity *pEntity = GetDriver();
	if ( pEntity && pEntity->IsPlayer() )
	{
		CBasePlayer *pPlayer = static_cast<CBasePlayer*>( pEntity );
		
		Vector vecAttachPoint;
		QAngle vecAttachAngles;
		
		// Check eyes. (vehicle_driver_eyes point)
		int iAttachment = LookupAttachment( "vehicle_driver_eyes" );
		GetAttachment( iAttachment, vecAttachPoint, vecAttachAngles );

		// Add the jeep's Z view offset
		Vector vecUp;
		AngleVectors( vecAttachAngles, NULL, NULL, &vecUp );
		vecUp.z = clamp( vecUp.z, 0.0f, vecUp.z );
		vecAttachPoint.z += r_JeepViewZHeight.GetFloat() * vecUp.z;

		bool bEyes = ( UTIL_PointContents( vecAttachPoint ) & MASK_WATER ) ? true : false;
		if ( bEyes )
		{
			pPlayer->SetWaterLevel( WL_Eyes );
			return;
		}

		// Check waist.  (vehicle_engine point -- see parent function).
		if ( m_WaterData.m_bBodyInWater )
		{
			pPlayer->SetWaterLevel( WL_Waist );
			return;
		}

		// Check feet. (vehicle_feet_passenger0 point)
		iAttachment = LookupAttachment( "vehicle_feet_passenger0" );
		GetAttachment( iAttachment, vecAttachPoint, vecAttachAngles );
		bool bFeet = ( UTIL_PointContents( vecAttachPoint ) & MASK_WATER ) ? true : false;
		if ( bFeet )
		{
			pPlayer->SetWaterLevel( WL_Feet );
			return;
		}

		// Not in water.
		pPlayer->SetWaterLevel( WL_NotInWater );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::CreateSplash( const Vector &vecPosition )
{
	// Splash data.
	CEffectData	data;
	data.m_fFlags = 0;
	data.m_vOrigin = vecPosition;
	data.m_vNormal.Init( 0.0f, 0.0f, 1.0f );
	VectorAngles( data.m_vNormal, data.m_vAngles );
	data.m_flScale = 10.0f + random->RandomFloat( 0, 2 );

	// Create the splash..
	DispatchEffect( "watersplash", data );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::CreateRipple( const Vector &vecPosition )
{
	// Ripple data.
	CEffectData	data;
	data.m_fFlags = 0;
	data.m_vOrigin = vecPosition;
	data.m_vNormal.Init( 0.0f, 0.0f, 1.0f );
	VectorAngles( data.m_vNormal, data.m_vAngles );
	data.m_flScale = 10.0f + random->RandomFloat( 0, 2 );
	if ( GetWaterType() & CONTENTS_SLIME )
	{
		data.m_fFlags |= FX_WATER_IN_SLIME;
	}

	// Create the ripple.
	DispatchEffect( "waterripple", data );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::Think( void )
{
	BaseClass::Think();

	// Water!?
	HandleWater();

	SetSimulationTime( gpGlobals->curtime );
	
	SetNextThink( gpGlobals->curtime );
	SetAnimatedEveryTick( true );

    if ( !m_bInitialHandbrake )	// after initial timer expires, set the handbrake
	{
		m_bInitialHandbrake = true;
		m_VehiclePhysics.SetHandbrake( true );
		m_VehiclePhysics.Think();
	}

	// Check overturned status.
	if ( !IsOverturned() )
	{
		m_flOverturnedTime = 0.0f;
	}
	else
	{
		m_flOverturnedTime += gpGlobals->frametime;
	}

	// spin gun if charging cannon
	//FIXME: Don't bother for E3
	if ( m_bCannonCharging )
	{
		m_nSpinPos += JEEP_GUN_SPIN_RATE;
		SetPoseParameter( JEEP_GUN_SPIN, m_nSpinPos );
	}

	// Aim gun based on the player view direction.
	if ( m_hPlayer && !m_bExitAnimOn && !m_bEnterAnimOn )
	{
		Vector vecEyeDir, vecEyePos;
		m_hPlayer->EyePositionAndVectors( &vecEyePos, &vecEyeDir, NULL, NULL );

		// Trace out from the player's eye point.
		Vector	vecEndPos = vecEyePos + ( vecEyeDir * MAX_TRACE_LENGTH );
		trace_t	trace;
		UTIL_TraceLine( vecEyePos, vecEndPos, MASK_SHOT, this, COLLISION_GROUP_NONE, &trace );

		// See if we hit something, if so, adjust end position to hit location.
		if ( trace.fraction < 1.0 )
		{
   			vecEndPos = vecEyePos + ( vecEyeDir * MAX_TRACE_LENGTH * trace.fraction );
		}

		//m_vecLookCrosshair = vecEndPos;
		AimGunAt( &vecEndPos, 0.1f );
	}

	StudioFrameAdvance();

	// If the enter or exit animation has finished, tell the server vehicle
	if ( IsSequenceFinished() && (m_bExitAnimOn || m_bEnterAnimOn) )
	{
		if ( m_bEnterAnimOn )
		{
			m_VehiclePhysics.ReleaseHandbrake();
			StartEngine();

			// HACKHACK: This forces the jeep to play a sound when it gets entered underwater
			if ( m_VehiclePhysics.IsEngineDisabled() )
			{
				CBaseServerVehicle *pServerVehicle = dynamic_cast<CBaseServerVehicle *>(GetServerVehicle());
				if ( pServerVehicle )
				{
					pServerVehicle->SoundStartDisabled();
				}
			}
		}
		
		// If we're exiting and have had the tau cannon removed, we don't want to reset the animation
		GetServerVehicle()->HandleEntryExitFinish( m_bExitAnimOn, true );
	}

	// See if the ammo crate needs to close
	if ( ( m_flAmmoCrateCloseTime < gpGlobals->curtime ) && ( GetSequence() == LookupSequence( "ammo_open" ) ) )
	{
		m_flAnimTime = gpGlobals->curtime;
		m_flPlaybackRate = 0.0;
		SetCycle( 0 );
		ResetSequence( LookupSequence( "ammo_close" ) );
	}
	else if ( ( GetSequence() == LookupSequence( "ammo_close" ) ) && IsSequenceFinished() )
	{
		m_flAnimTime = gpGlobals->curtime;
		m_flPlaybackRate = 0.0;
		SetCycle( 0 );
		ResetSequence( LookupSequence( "idle" ) );

		CPASAttenuationFilter sndFilter( this, "PropJeep.AmmoClose" );
		EmitSound( sndFilter, entindex(), "PropJeep.AmmoClose" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &startPos - 
//			&endPos - 
//			width - 
//			useMuzzle - 
//-----------------------------------------------------------------------------
void CTFVehicle::DrawBeam( const Vector &startPos, const Vector &endPos, float width )
{
	/*
	//Tracer down the middle
	UTIL_Tracer( startPos, endPos, 0, TRACER_DONT_USE_ATTACHMENT, 6500, false, "GaussTracer" );

	//Draw the main beam shaft
	CBeam *pBeam = CBeam::BeamCreate( GAUSS_BEAM_SPRITE, 0.5 );
	
	pBeam->SetStartPos( startPos );
	pBeam->PointEntInit( endPos, this );
	pBeam->SetEndAttachment( LookupAttachment("Muzzle") );
	pBeam->SetWidth( width );
	pBeam->SetEndWidth( 0.05f );
	pBeam->SetBrightness( 255 );
	pBeam->SetColor( 255, 185+random->RandomInt( -16, 16 ), 40 );
	pBeam->RelinkBeam();
	pBeam->LiveForTime( 0.1f );

	//Draw electric bolts along shaft
	pBeam = CBeam::BeamCreate( GAUSS_BEAM_SPRITE, 3.0f );
	
	pBeam->SetStartPos( startPos );
	pBeam->PointEntInit( endPos, this );
	pBeam->SetEndAttachment( LookupAttachment("Muzzle") );

	pBeam->SetBrightness( random->RandomInt( 64, 255 ) );
	pBeam->SetColor( 255, 255, 150+random->RandomInt( 0, 64 ) );
	pBeam->RelinkBeam();
	pBeam->LiveForTime( 0.1f );
	pBeam->SetNoise( 1.6f );
	pBeam->SetEndWidth( 0.1f ); */
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::FireCannon( void )
{
	//Don't fire again if it's been too soon
	if ( m_flCannonTime > gpGlobals->curtime )
		return;

	if ( m_bUnableToFire )
		return;

	m_flCannonTime = gpGlobals->curtime + 0.2f;
	m_bCannonCharging = false;

	//Find the direction the gun is pointing in
	Vector aimDir;
	GetCannonAim( &aimDir );

	FireBulletsInfo_t	info( 1, m_vecGunOrigin, aimDir, VECTOR_CONE_1DEGREES, MAX_TRACE_LENGTH, m_nAmmoType );

	info.m_nFlags = FIRE_BULLETS_ALLOW_WATER_SURFACE_IMPACTS;
	info.m_pAttacker = m_hPlayer;

	FireBullets( info );

	// Register a muzzleflash for the AI
	if ( m_hPlayer )
	{
		m_hPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5 );
	}

	CPASAttenuationFilter sndFilter( this, "PropJeep.FireCannon" );
	EmitSound( sndFilter, entindex(), "PropJeep.FireCannon" );
	
	// make cylinders of gun spin a bit
	m_nSpinPos += JEEP_GUN_SPIN_RATE;
	//SetPoseParameter( JEEP_GUN_SPIN, m_nSpinPos );	//FIXME: Don't bother with this for E3, won't look right
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::FireChargedCannon( void )
{
/*	bool penetrated = false;

	m_bCannonCharging	= false;
	m_flCannonTime		= gpGlobals->curtime + 0.5f;

	StopChargeSound();

	CPASAttenuationFilter sndFilter( this, "PropJeep.FireChargedCannon" );
	EmitSound( sndFilter, entindex(), "PropJeep.FireChargedCannon" );

	//Find the direction the gun is pointing in
	Vector aimDir;
	GetCannonAim( &aimDir );

	Vector endPos = m_vecGunOrigin + ( aimDir * MAX_TRACE_LENGTH );
	
	//Shoot a shot straight out
	trace_t	tr;
	UTIL_TraceLine( m_vecGunOrigin, endPos, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
	
	ClearMultiDamage();

	//Find how much damage to do
	float flChargeAmount = ( gpGlobals->curtime - m_flCannonChargeStartTime ) / MAX_GAUSS_CHARGE_TIME;

	//Clamp this
	if ( flChargeAmount > 1.0f )
	{
		flChargeAmount = 1.0f;
	}

	//Determine the damage amount
	//FIXME: Use ConVars!
	float flDamage = 15 + ( ( 250 - 15 ) * flChargeAmount );

	CBaseEntity *pHit = tr.m_pEnt;
	
	//Look for wall penetration
	if ( tr.DidHitWorld() && !(tr.surface.flags & SURF_SKY) )
	{
		//Try wall penetration
		UTIL_ImpactTrace( &tr, m_nBulletType, "ImpactJeep" );
		UTIL_DecalTrace( &tr, "RedGlowFade" );

		CPVSFilter filter( tr.endpos );
		te->GaussExplosion( filter, 0.0f, tr.endpos, tr.plane.normal, 0 );
		
		Vector	testPos = tr.endpos + ( aimDir * 48.0f );

		UTIL_TraceLine( testPos, tr.endpos, MASK_SHOT, GetDriver(), COLLISION_GROUP_NONE, &tr );
			
		if ( tr.allsolid == false )
		{
			UTIL_DecalTrace( &tr, "RedGlowFade" );

			penetrated = true;
		}
	}
	else if ( pHit != NULL )
	{
		CTakeDamageInfo dmgInfo( this, GetDriver(), flDamage, DMG_SHOCK );
		CalculateBulletDamageForce( &dmgInfo, GetAmmoDef()->Index("GaussEnergy"), aimDir, tr.endpos, 1.0f + flChargeAmount * 4.0f );

		//Do direct damage to anything in our path
		pHit->DispatchTraceAttack( dmgInfo, aimDir, &tr );
	}

	ApplyMultiDamage();

	//Kick up an effect
	if ( !(tr.surface.flags & SURF_SKY) )
	{
  		UTIL_ImpactTrace( &tr, m_nBulletType, "ImpactJeep" );

		//Do a gauss explosion
		CPVSFilter filter( tr.endpos );
		te->GaussExplosion( filter, 0.0f, tr.endpos, tr.plane.normal, 0 );
	}

	//Show the effect
	DrawBeam( m_vecGunOrigin, tr.endpos, 9.6 );

	// Register a muzzleflash for the AI
	if ( m_hPlayer )
	{
		m_hPlayer->SetMuzzleFlashTime( gpGlobals->curtime + 0.5f );
	}

	//Rock the car
	IPhysicsObject *pObj = VPhysicsGetObject();

	if ( pObj != NULL )
	{
		Vector	shoveDir = aimDir * -( flDamage * 500.0f );

		pObj->ApplyForceOffset( shoveDir, m_vecGunOrigin );
	}

	//Do radius damage if we didn't penetrate the wall
	if ( penetrated == true )
	{
		RadiusDamage( CTakeDamageInfo( this, this, flDamage, DMG_SHOCK ), tr.endpos, 200.0f, CLASS_NONE, NULL );
	} */
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::ChargeCannon( void )
{
	//Don't fire again if it's been too soon
	if ( m_flCannonTime > gpGlobals->curtime )
		return;

	//See if we're starting a charge
	if ( m_bCannonCharging == false )
	{
		m_flCannonChargeStartTime = gpGlobals->curtime;
		m_bCannonCharging = true;

		//Start charging sound
		CPASAttenuationFilter filter( this );
		m_sndCannonCharge = (CSoundEnvelopeController::GetController()).SoundCreate( filter, entindex(), CHAN_STATIC, "Jeep.GaussCharge", ATTN_NORM );

		assert(m_sndCannonCharge!=NULL);
		if ( m_sndCannonCharge != NULL )
		{
			(CSoundEnvelopeController::GetController()).Play( m_sndCannonCharge, 1.0f, 50 );
			(CSoundEnvelopeController::GetController()).SoundChangePitch( m_sndCannonCharge, 250, 3.0f );
		}

		return;
	}

	//TODO: Add muzzle effect?

	//TODO: Check for overcharge and have the weapon simply fire or instead "decharge"?
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::StopChargeSound( void )
{
	if ( m_sndCannonCharge != NULL )
	{
		(CSoundEnvelopeController::GetController()).SoundFadeOut( m_sndCannonCharge, 0.1f );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Finds the true aiming position of the gun (looks at what player 
//			is looking at and adjusts)
// Input  : &resultDir - direction to be calculated
//-----------------------------------------------------------------------------
void CTFVehicle::GetCannonAim( Vector *resultDir )
{
	Vector	muzzleOrigin;
	QAngle	muzzleAngles;

	GetAttachment( LookupAttachment("gun_ref"), muzzleOrigin, muzzleAngles );

	AngleVectors( muzzleAngles, resultDir );
}

//-----------------------------------------------------------------------------
// Purpose: If the player uses the jeep while at the back, he gets ammo from the crate instead
//-----------------------------------------------------------------------------
void CTFVehicle::Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	CBasePlayer *pPlayer = ToBasePlayer( pActivator );
	
	if ( pPlayer == NULL)
		return;

	// Find out if the player's looking at our ammocrate hitbox 
	Vector vecForward;
	pPlayer->EyeVectors( &vecForward, NULL, NULL );

	trace_t tr;
	Vector vecStart = pPlayer->EyePosition();
	UTIL_TraceLine( vecStart, vecStart + vecForward * 1024, MASK_SOLID | CONTENTS_DEBRIS | CONTENTS_HITBOX, pPlayer, COLLISION_GROUP_NONE, &tr );
	
	if ( tr.m_pEnt == this && tr.hitgroup == JEEP_AMMOCRATE_HITGROUP )
	{
		// Player's using the crate.
		// Fill up his SMG ammo.
		pPlayer->GiveAmmo( 300, "SMG1");
		
		if ( ( GetSequence() != LookupSequence( "ammo_open" ) ) && ( GetSequence() != LookupSequence( "ammo_close" ) ) )
		{
			// Open the crate
			m_flAnimTime = gpGlobals->curtime;
			m_flPlaybackRate = 0.0;
			SetCycle( 0 );
			ResetSequence( LookupSequence( "ammo_open" ) );
			
			CPASAttenuationFilter sndFilter( this, "PropJeep.AmmoOpen" );
			EmitSound( sndFilter, entindex(), "PropJeep.AmmoOpen" );
		}

		m_flAmmoCrateCloseTime = gpGlobals->curtime + JEEP_AMMO_CRATE_CLOSE_DELAY;
		return;
	}

	// Fall back and get in the vehicle instead
	BaseClass::Use( pActivator, pCaller, useType, value );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CTFVehicle::GetDriver( void ) 
{ 
	return m_hPlayer; 
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CBaseEntity *CTFVehicle::GetPassenger( void ) 
{ 
	return m_hPlayerPassenger; 
}

//-----------------------------------------------------------------------------
// Purpose: Return true of the player's allowed to enter the vehicle
//-----------------------------------------------------------------------------
bool CTFVehicle::CanEnterVehicle( CBaseEntity *pEntity )
{
	// Only drivers are supported
	Assert( pEntity && pEntity->IsPlayer() );

	// Prevent entering if the vehicle's being driven by an NPC
	if ( GetDriver() && GetDriver() != pEntity )
	{
		if ( GetPassenger() && GetPassenger() != pEntity )
			return false;
	}

	// Can't enter if we're upside-down
	if ( IsOverturned() )
		return false;

	// Prevent entering if the vehicle's locked, or if it's moving too fast.
	return ( !m_bLocked && (m_nSpeed <= m_flMinimumSpeedToEnterExit) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFVehicle::CanExitVehicle( CBaseEntity *pEntity )
{
	return ( !m_bEnterAnimOn && !m_bExitAnimOn && !m_bLocked && (m_nSpeed <= g_jeepexitspeed.GetFloat() ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::DampenEyePosition( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles )
{
	// Get the frametime. (Check to see if enough time has passed to warrent dampening).
	float flFrameTime = gpGlobals->frametime;
	if ( flFrameTime < JEEP_FRAMETIME_MIN )
	{
		vecVehicleEyePos = m_vecLastEyePos;
		DampenUpMotion( vecVehicleEyePos, vecVehicleEyeAngles, 0.0f );
		return;
	}

	// Keep static the sideways motion.

	// Dampen forward/backward motion.
	DampenForwardMotion( vecVehicleEyePos, vecVehicleEyeAngles, flFrameTime );

	// Blend up/down motion.
	DampenUpMotion( vecVehicleEyePos, vecVehicleEyeAngles, flFrameTime );
}

//-----------------------------------------------------------------------------
// Use the controller as follows:
// speed += ( pCoefficientsOut[0] * ( targetPos - currentPos ) + pCoefficientsOut[1] * ( targetSpeed - currentSpeed ) ) * flDeltaTime;
//-----------------------------------------------------------------------------
void CTFVehicle::ComputePDControllerCoefficients( float *pCoefficientsOut,
												  float flFrequency, float flDampening,
												  float flDeltaTime )
{
	float flKs = 9.0f * flFrequency * flFrequency;
	float flKd = 4.5f * flFrequency * flDampening;

	float flScale = 1.0f / ( 1.0f + flKd * flDeltaTime + flKs * flDeltaTime * flDeltaTime );

	pCoefficientsOut[0] = flKs * flScale;
	pCoefficientsOut[1] = ( flKd + flKs * flDeltaTime ) * flScale;
}
 
//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFVehicle::DampenForwardMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime )
{
	// Get forward vector.
	Vector vecForward;
	AngleVectors( vecVehicleEyeAngles, &vecForward);

	// Simulate the eye position forward based on the data from last frame
	// (assumes no acceleration - it will get that from the "spring").
	Vector vecCurrentEyePos = m_vecLastEyePos + m_vecEyeSpeed * flFrameTime;

	// Calculate target speed based on the current vehicle eye position and the last vehicle eye position and frametime.
	Vector vecVehicleEyeSpeed = ( vecVehicleEyePos - m_vecLastEyeTarget ) / flFrameTime;
	m_vecLastEyeTarget = vecVehicleEyePos;	

	// Calculate the speed and position deltas.
	Vector vecDeltaSpeed = vecVehicleEyeSpeed - m_vecEyeSpeed;
	Vector vecDeltaPos = vecVehicleEyePos - vecCurrentEyePos;

	// Clamp.
	if ( vecDeltaPos.Length() > JEEP_DELTA_LENGTH_MAX )
	{
		float flSign = vecForward.Dot( vecVehicleEyeSpeed ) >= 0.0f ? -1.0f : 1.0f;
		vecVehicleEyePos += flSign * ( vecForward * JEEP_DELTA_LENGTH_MAX );
		m_vecLastEyePos = vecVehicleEyePos;
		m_vecEyeSpeed = vecVehicleEyeSpeed;
		return;
	}

	// Generate an updated (dampening) speed for use in next frames position extrapolation.
	float flCoefficients[2];
	ComputePDControllerCoefficients( flCoefficients, r_JeepViewDampenFreq.GetFloat(), r_JeepViewDampenDamp.GetFloat(), flFrameTime );
	m_vecEyeSpeed += ( ( flCoefficients[0] * vecDeltaPos + flCoefficients[1] * vecDeltaSpeed ) * flFrameTime );

	// Save off data for next frame.
	m_vecLastEyePos = vecCurrentEyePos;

	// Move eye forward/backward.
	Vector vecForwardOffset = vecForward * ( vecForward.Dot( vecDeltaPos ) );
	vecVehicleEyePos -= vecForwardOffset;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void CTFVehicle::DampenUpMotion( Vector &vecVehicleEyePos, QAngle &vecVehicleEyeAngles, float flFrameTime )
{
	// Get up vector.
	Vector vecUp;
	AngleVectors( vecVehicleEyeAngles, NULL, NULL, &vecUp );
	vecUp.z = clamp( vecUp.z, 0.0f, vecUp.z );
	vecVehicleEyePos.z += r_JeepViewZHeight.GetFloat() * vecUp.z;

	// NOTE: Should probably use some damped equation here.
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::SetupMove( CBasePlayer *player, CUserCmd *ucmd, IMoveHelper *pHelper, CMoveData *move )
{
	// If we are overturned and hit any key - leave the vehicle (IN_USE is already handled!).
	if ( m_flOverturnedTime > OVERTURNED_EXIT_WAITTIME )
	{
		if ( (ucmd->buttons & (IN_FORWARD|IN_BACK|IN_MOVELEFT|IN_MOVERIGHT|IN_SPEED|IN_JUMP|IN_ATTACK|IN_ATTACK2) ) && !m_bExitAnimOn )
		{
			// Can't exit yet? We're probably still moving. Swallow the keys.
			if ( !CanExitVehicle(player) )
				return;

			if ( !GetServerVehicle()->HandlePassengerExit( m_hPlayer ) && ( m_hPlayer != NULL ) )
			{
				m_hPlayer->PlayUseDenySound();
			}
			return;
		}
	}

	// If the throttle is disabled or we're upside-down, don't allow throttling (including turbo)
	CUserCmd tmp;
	if ( ( m_throttleDisableTime > gpGlobals->curtime ) || ( IsOverturned() ) )
	{
		m_bUnableToFire = true;
		
		tmp = (*ucmd);
		tmp.buttons &= ~(IN_FORWARD|IN_BACK|IN_SPEED);
		ucmd = &tmp;
	}
	
	BaseClass::SetupMove( player, ucmd, pHelper, move );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::DriveVehicle( float flFrameTime, CUserCmd *ucmd, int iButtonsDown, int iButtonsReleased )
{
	int iButtons = ucmd->buttons;

	//Adrian: No headlights on Superfly.
/*	if ( ucmd->impulse == 100 )
	{
		if (HeadlightIsOn())
		{
			HeadlightTurnOff();
		}
        else 
		{
			HeadlightTurnOn();
		}
	}*/
		
	// If we're holding down an attack button, update our state
	if ( IsOverturned() == false )
	{
		if ( iButtons & IN_ATTACK )
		{
			if ( m_bCannonCharging )
			{
				FireChargedCannon();
			}
			else
			{
				FireCannon();
			}
		}
		else if ( iButtons & IN_ATTACK2 )
		{
			ChargeCannon();
		}
	}

	// If we've released our secondary button, fire off our cannon
	if ( ( iButtonsReleased & IN_ATTACK2 ) && ( m_bCannonCharging ) )
	{
		FireChargedCannon();
	}

	BaseClass::DriveVehicle( flFrameTime, ucmd, iButtonsDown, iButtonsReleased );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *pPlayer - 
//			*pMoveData - 
//-----------------------------------------------------------------------------
void CTFVehicle::ProcessMovement( CBasePlayer *pPlayer, CMoveData *pMoveData )
{
	BaseClass::ProcessMovement( pPlayer, pMoveData );

	// Update the steering angles based on speed.
	UpdateSteeringAngle();

	// Create dangers sounds in front of the vehicle.
	CreateDangerSounds();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::UpdateSteeringAngle( void )
{
	float flMaxSpeed = m_VehiclePhysics.GetMaxSpeed();
	float flSpeed = m_VehiclePhysics.GetSpeed();

	float flRatio = 1.0f - ( flSpeed / flMaxSpeed );
	float flSteeringDegrees = JEEP_STEERING_FAST_ANGLE + ( ( JEEP_STEERING_SLOW_ANGLE - JEEP_STEERING_FAST_ANGLE ) * flRatio );
	flSteeringDegrees = clamp( flSteeringDegrees, JEEP_STEERING_FAST_ANGLE, JEEP_STEERING_SLOW_ANGLE );
	m_VehiclePhysics.SetSteeringDegrees( flSteeringDegrees );
}

//-----------------------------------------------------------------------------
// Purpose: Create danger sounds in front of the vehicle.
//-----------------------------------------------------------------------------
void CTFVehicle::CreateDangerSounds( void )
{
	QAngle dummy;
	GetAttachment( "Muzzle", m_vecGunOrigin, dummy );

	if ( m_flDangerSoundTime > gpGlobals->curtime )
		return;

	QAngle vehicleAngles = GetLocalAngles();
	Vector vecStart = GetAbsOrigin();
	Vector vecDir, vecRight;

	GetVectors( &vecDir, &vecRight, NULL );

	const float soundDuration = 0.25;
	float speed = m_VehiclePhysics.GetHLSpeed();
	// Make danger sounds ahead of the jeep
	if ( fabs(speed) > 120 )
	{
		Vector	vecSpot;

		float steering = m_VehiclePhysics.GetSteering();
		if ( steering != 0 )
		{
			if ( speed > 0 )
			{
				vecDir += vecRight * steering * 0.5;
			}
			else
			{
				vecDir -= vecRight * steering * 0.5;
			}
			VectorNormalize(vecDir);
		}
		const float radius = speed * 0.4;
		// 0.3 seconds ahead of the jeep
		vecSpot = vecStart + vecDir * (speed * 0.3f);
		CSoundEnt::InsertSound( SOUND_DANGER, vecSpot, radius, soundDuration, this, 0 );
		CSoundEnt::InsertSound( SOUND_PHYSICS_DANGER, vecSpot, radius, soundDuration, this, 1 );
		//NDebugOverlay::Box(vecSpot, Vector(-radius,-radius,-radius),Vector(radius,radius,radius), 255, 0, 255, 0, soundDuration);

#if 0
		trace_t	tr;
		// put sounds a bit to left and right but slightly closer to Jeep to make a "cone" of sound 
		// in front of it
		vecSpot = vecStart + vecDir * (speed * 0.5f) - vecRight * speed * 0.5;
		UTIL_TraceLine( vecStart, vecSpot, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
		CSoundEnt::InsertSound( SOUND_DANGER, vecSpot, 400, soundDuration, this, 1 );

		vecSpot = vecStart + vecDir * (speed * 0.5f) + vecRight * speed * 0.5;
		UTIL_TraceLine( vecStart, vecSpot, MASK_SHOT, this, COLLISION_GROUP_NONE, &tr );
		CSoundEnt::InsertSound( SOUND_DANGER, vecSpot, 400, soundDuration, this, 2);
#endif
	}

	m_flDangerSoundTime = gpGlobals->curtime + 0.1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::EnterVehicle( CBasePlayer *pPlayer )
{
	if ( !pPlayer )
		return;

	CheckWater();
	BaseClass::EnterVehicle( pPlayer );

	// Start looking for seagulls to land
	m_hLastPlayerInVehicle = m_hPlayer;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::ExitVehicle( int nRole )
{
	HeadlightTurnOff();

	BaseClass::ExitVehicle( nRole );

	//If the player has exited, stop charging
	StopChargeSound();
	m_bCannonCharging = false;

	// Remember when we last saw the player
	m_flPlayerExitedTime = gpGlobals->curtime;
	m_flLastSawPlayerAt = gpGlobals->curtime;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::InputStartRemoveTauCannon( inputdata_t &inputdata )
{
	// Start the gun removal animation
	m_flAnimTime = gpGlobals->curtime;
	m_flPlaybackRate = 0.0;
	SetCycle( 0 );
	ResetSequence( LookupSequence( "tau_levitate" ) );

	m_bGunHasBeenCutOff = true;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::InputFinishRemoveTauCannon( inputdata_t &inputdata )
{
	// Remove & hide the gun
	SetBodygroup( 1, false );
	m_bHasGun = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVehicle::OnRestore( void )
{
	IServerVehicle *pServerVehicle = GetServerVehicle();
	if ( pServerVehicle != NULL )
	{
		// Restore the passenger information we're holding on to
		pServerVehicle->RestorePassengerInfo();
	}
}

//========================================================================================================================================
// JEEP FOUR WHEEL PHYSICS VEHICLE SERVER VEHICLE
//========================================================================================================================================

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : &vecEyeExitEndpoint - 
// Output : int
//-----------------------------------------------------------------------------
int CJeepFourWheelServerVehicle::GetExitAnimToUse( Vector &vecEyeExitEndpoint, bool &bAllPointsBlocked )
{
	bAllPointsBlocked = false;

	if ( !m_bParsedAnimations )
	{
		// Load the entry/exit animations from the vehicle
		ParseEntryExitAnims();
		m_bParsedAnimations = true;
	}

	CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
	// If we don't have the gun anymore, we want to get out using the "gun-less" animation
	if ( pAnimating )
	{
		// HACK: We know the tau-cannon removed exit anim uses the first upright anim's exit details
		trace_t tr;

		// Convert our offset points to worldspace ones
		Vector vehicleExitOrigin = m_ExitAnimations[0].vecExitPointLocal;
		QAngle vehicleExitAngles = m_ExitAnimations[0].vecExitAnglesLocal;
		UTIL_ParentToWorldSpace( pAnimating, vehicleExitOrigin, vehicleExitAngles );

		// Ensure the endpoint is clear by dropping a point down from above
		vehicleExitOrigin -= VEC_VIEW;
		Vector vecMove = Vector(0,0,64);
		Vector vecStart = vehicleExitOrigin + vecMove;
		Vector vecEnd = vehicleExitOrigin - vecMove;
  		UTIL_TraceHull( vecStart, vecEnd, VEC_HULL_MIN, VEC_HULL_MAX, MASK_SOLID, NULL, COLLISION_GROUP_NONE, &tr );

		Assert( !tr.startsolid && tr.fraction < 1.0 );
		m_vecCurrentExitEndPoint = vecStart + ((vecEnd - vecStart) * tr.fraction);
		vecEyeExitEndpoint = m_vecCurrentExitEndPoint + VEC_VIEW;
		m_iCurrentExitAnim = 0;
		return pAnimating->LookupSequence( "exit_tauremoved" );
	}

	return BaseClass::GetExitAnimToUse( vecEyeExitEndpoint, bAllPointsBlocked );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CJeepFourWheelServerVehicle::ItemPostFrame( CBasePlayer *player )
{
	Assert( player == GetDriver() );

	GetDrivableVehicle()->ItemPostFrame( player );

	if ( player->m_nButtons & IN_USE )
	{
		if ( GetDrivableVehicle()->CanExitVehicle(player) )
		{
			if ( !HandlePassengerExit( player ) && ( player != NULL ) )
			{
				player->PlayUseDenySound();
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CJeepFourWheelServerVehicle::HandlePassengerEntry( CBaseCombatCharacter *pPassenger, bool bAllowEntryOutsideZone )
{
	CBasePlayer *pPlayer = ToBasePlayer( pPassenger );
	if ( pPlayer != NULL )
	{
		// Find out which hitbox the player's eyepoint is within
		int iEntryAnim = GetEntryAnimForPoint( pPlayer->EyePosition() );

		// Get this interface for animation queries
		CBaseAnimating *pAnimating = dynamic_cast<CBaseAnimating *>(m_pVehicle);
		if ( !pAnimating )
			return;

		// Are we in an entrypoint zone? 
		if ( iEntryAnim == ACTIVITY_NOT_AVAILABLE )
		{
			// Normal get in refuses to allow entry
			if ( !bAllowEntryOutsideZone )
				return;

			// We failed to find a valid entry anim, but we've got to get back in because the player's
			// got stuck exiting the vehicle. For now, just use the first get in anim
			// UNDONE: We need a better solution for this.
			iEntryAnim = pAnimating->LookupSequence( m_EntryAnimations[0].szAnimName );
		}

		// Check to see if this vehicle can be controlled or if it's locked
		if ( GetDrivableVehicle()->CanEnterVehicle( pPlayer ) )
		{
			// Make sure the passenger can get in as well
			if ( pPlayer->CanEnterVehicle( this, VEHICLE_ROLE_DRIVER ) )
			{
				// Setup the "enter" vehicle sequence and skip the animation if it isn't present.
				pAnimating->SetCycle( 0 );
				pAnimating->m_flAnimTime = gpGlobals->curtime;
				pAnimating->ResetSequence( iEntryAnim );
				pAnimating->ResetClientsideFrame();
				pAnimating->InvalidateBoneCache();	// This is necessary because we need to query attachment points this frame for blending!
				GetDrivableVehicle()->SetVehicleEntryAnim( true );

				pPlayer->GetInVehicle( this, VEHICLE_ROLE_DRIVER );
			}
			else if ( pPlayer->CanEnterVehicle( this, VEHICLE_ROLE_PASSENGER ) )
			{
				// Setup the "enter" vehicle sequence and skip the animation if it isn't present.
				pAnimating->SetCycle( 0 );
				pAnimating->m_flAnimTime = gpGlobals->curtime;
				pAnimating->ResetSequence( iEntryAnim );
				pAnimating->ResetClientsideFrame();
				pAnimating->InvalidateBoneCache();	// This is necessary because we need to query attachment points this frame for blending!
				GetDrivableVehicle()->SetVehicleEntryAnim( true );

				pPlayer->GetInVehicle( this, VEHICLE_ROLE_PASSENGER );
			}
		}
	}
	else
	{
		// NPCs handle transitioning themselves, they should NOT call this function
		Assert( 0 );
	}
}

