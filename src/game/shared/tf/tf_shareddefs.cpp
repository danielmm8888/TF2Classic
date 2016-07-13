//====== Copyright © 1996-2004, Valve Corporation, All rights reserved. =======
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "tf_shareddefs.h"
#include "KeyValues.h"
#include "takedamageinfo.h"
#include "tf_gamerules.h"

//-----------------------------------------------------------------------------
// Teams.
//-----------------------------------------------------------------------------
const char *g_aTeamNames[TF_TEAM_COUNT] =
{
	"Unassigned",
	"Spectator",
	"Red",
	"Blue",
	"Green",
	"Yellow"
};

const char *g_aTeamNamesShort[TF_TEAM_COUNT] =
{
	"red", // Unassigned
	"red", // Spectator
	"red",
	"blu",
	"grn",
	"ylw"
};

const char *g_aTeamParticleNames[TF_TEAM_COUNT] =
{
	"red",
	"red",
	"red",
	"blue",
	"green",
	"yellow"
};

const char *GetTeamParticleName( int iTeam, bool bDeathmatchOverride /*= false*/, const char **pNames/* = g_aTeamParticleNames*/ )
{
	if ( bDeathmatchOverride && TFGameRules() && TFGameRules()->IsDeathmatch() )
	{
		return "dm";
	}

	return pNames[iTeam];
}

const char *ConstructTeamParticle( const char *pszFormat, int iTeam, bool bDeathmatchOverride /*= false*/, const char **pNames/* = g_aTeamParticleNames*/ )
{
	static char szParticleName[128];

	V_snprintf( szParticleName, sizeof( szParticleName ), pszFormat, GetTeamParticleName( iTeam, bDeathmatchOverride, pNames ) );
	return szParticleName;
}

void PrecacheTeamParticles( const char *pszFormat, bool bDeathmatchOverride /*= false*/, const char **pNames/* = g_aTeamParticleNames*/ )
{
	for ( int i = FIRST_GAME_TEAM; i < TF_TEAM_COUNT; i++ )
	{
		const char *pszParticle = ConstructTeamParticle( pszFormat, i, false, pNames );
		PrecacheParticleSystem( pszParticle );
	}

	if ( bDeathmatchOverride )
	{
		char szParticle[128];
		V_snprintf( szParticle, sizeof( szParticle ), pszFormat, "dm" );
		PrecacheParticleSystem( szParticle );
	}
}

color32 g_aTeamColors[TF_TEAM_COUNT] = 
{
	{ 0, 0, 0, 0 }, // Unassigned
	{ 0, 0, 0, 0 }, // Spectator
	{ 255, 0, 0, 0 }, // Red
	{ 0, 0, 255, 0 }, // Blue
	{ 0, 255, 0, 0 }, // Green
	{ 255, 255, 0, 0 } // Yellow
};

bool IsGameTeam( int iTeam )
{
	return ( iTeam > LAST_SHARED_TEAM && iTeam < TF_TEAM_COUNT ); 
}

//-----------------------------------------------------------------------------
// Classes.
//-----------------------------------------------------------------------------

const char *g_aPlayerClassNames[] =
{
	"#TF_Class_Name_Undefined",
	"#TF_Class_Name_Scout",
	"#TF_Class_Name_Sniper",
	"#TF_Class_Name_Soldier",
	"#TF_Class_Name_Demoman",
	"#TF_Class_Name_Medic",
	"#TF_Class_Name_HWGuy",
	"#TF_Class_Name_Pyro",
	"#TF_Class_Name_Spy",
	"#TF_Class_Name_Engineer",
	"#TF_Class_Name_Civilian",
	"#TF_Class_Name_Mercenary"
};

const char *g_aPlayerClassEmblems[] =
{
	"../hud/leaderboard_class_scout",
	"../hud/leaderboard_class_sniper",
	"../hud/leaderboard_class_soldier",
	"../hud/leaderboard_class_demo",
	"../hud/leaderboard_class_medic",
	"../hud/leaderboard_class_heavy",
	"../hud/leaderboard_class_pyro",
	"../hud/leaderboard_class_spy",
	"../hud/leaderboard_class_engineer",
	"../hud/leaderboard_class_civilian",
};

const char *g_aPlayerClassEmblemsDead[] =
{
	"../hud/leaderboard_class_scout_d",
	"../hud/leaderboard_class_sniper_d",
	"../hud/leaderboard_class_soldier_d",
	"../hud/leaderboard_class_demo_d",
	"../hud/leaderboard_class_medic_d",
	"../hud/leaderboard_class_heavy_d",
	"../hud/leaderboard_class_pyro_d",
	"../hud/leaderboard_class_spy_d",
	"../hud/leaderboard_class_engineer_d",
	"../hud/leaderboard_class_civilian_d",
};

const char *g_aPlayerClassNames_NonLocalized[] =
{
	"Undefined",
	"Scout",
	"Sniper",
	"Soldier",
	"Demoman",
	"Medic",
	"Heavy",
	"Pyro",
	"Spy",
	"Engineer",
	"Civilian",
	"Mercenary"
};

//-----------------------------------------------------------------------------
// Gametypes.
//-----------------------------------------------------------------------------
const char *g_aGameTypeNames[] =
{
	"Undefined",
	"#Gametype_CTF",
	"#Gametype_CP",
	"#Gametype_Escort",
	"#Gametype_Arena",
	"#Gametype_RobotDestruction",
	"#GameType_Passtime",
	"#GameType_PlayerDestruction",
	"#Gametype_MVM",
	"#Gametype_DM",
	"#Gametype_VIP",
};

//-----------------------------------------------------------------------------
// Weapon Types
//-----------------------------------------------------------------------------
const char *g_AnimSlots[] =
{
	"primary",
	"secondary",
	"melee",
	"grenade",
	"building",
	"pda",
	"item1",
	"item2",
	"MELEE_ALLCLASS",
	"secondary2",
	"primary2"
};

const char *g_LoadoutSlots[] =
{
	"primary",
	"secondary",
	"melee",
	"pda",
	"pda2",
	"building",
	"head",
	"misc",
	"action"
};

//-----------------------------------------------------------------------------
// Ammo.
//-----------------------------------------------------------------------------
const char *g_aAmmoNames[] =
{
	"DUMMY AMMO",
	"TF_AMMO_PRIMARY",
	"TF_AMMO_SECONDARY",
	"TF_AMMO_METAL",
	"TF_AMMO_GRENADES1",
	"TF_AMMO_GRENADES2"
};

struct pszWpnEntTranslationListEntry
{
	const char *weapon_name;
	const char *padding;
	const char *weapon_scout;
	const char *weapon_sniper;
	const char *weapon_soldier;
	const char *weapon_demoman;
	const char *weapon_medic;
	const char *weapon_heavyweapons;
	const char *weapon_pyro;
	const char *weapon_spy;
	const char *weapon_engineer;
	const char *weapon_civilian;
	const char *weapon_mercenary;
};
static pszWpnEntTranslationListEntry pszWpnEntTranslationList[] =
{
	"tf_weapon_shotgun",			// Base weapon to translate
	NULL,
	"tf_weapon_shotgun_primary",	// Scout
	"tf_weapon_shotgun_primary",	// Sniper
	"tf_weapon_shotgun_soldier",	// Soldier
	"tf_weapon_shotgun_primary",	// Demoman
	"tf_weapon_shotgun_primary",	// Medic
	"tf_weapon_shotgun_hwg",		// Heavy
	"tf_weapon_shotgun_pyro",		// Pyro
	"tf_weapon_shotgun_primary",	// Spy
	"tf_weapon_shotgun_primary",	// Engineer
	"tf_weapon_shotgun_primary",	// Civilian
	"tf_weapon_shotgun_soldier",	// Mercenary

	"tf_weapon_pistol",				// Base weapon to translate
	NULL,
	"tf_weapon_pistol_scout",		// Scout
	"tf_weapon_pistol",				// Sniper
	"tf_weapon_pistol",				// Soldier
	"tf_weapon_pistol",				// Demoman
	"tf_weapon_pistol",				// Medic
	"tf_weapon_pistol",				// Heavy
	"tf_weapon_pistol",				// Pyro
	"tf_weapon_pistol",				// Spy
	"tf_weapon_pistol",				// Engineer
	"tf_weapon_pistol",				// Civilian
	"tf_weapon_pistol",				// Mercenary

	"tf_weapon_shovel",				// Base weapon to translate
	NULL,
	"tf_weapon_shovel",				// Scout
	"tf_weapon_shovel",				// Sniper
	"tf_weapon_shovel",				// Soldier
	"tf_weapon_bottle",				// Demoman
	"tf_weapon_shovel",				// Medic
	"tf_weapon_shovel",				// Heavy
	"tf_weapon_shovel",				// Pyro
	"tf_weapon_shovel",				// Spy
	"tf_weapon_shovel",				// Engineer
	"tf_weapon_shovel",				// Civilian
	"tf_weapon_shovel",				// Mercenary

	"tf_weapon_bottle",				// Base weapon to translate
	NULL,
	"tf_weapon_bottle",				// Scout
	"tf_weapon_bottle",				// Sniper
	"tf_weapon_shovel",				// Soldier
	"tf_weapon_bottle",				// Demoman
	"tf_weapon_bottle",				// Medic
	"tf_weapon_bottle",				// Heavy
	"tf_weapon_bottle",				// Pyro
	"tf_weapon_bottle",				// Spy
	"tf_weapon_bottle",				// Engineer
	"tf_weapon_bottle",				// Civilian
	"tf_weapon_bottle",				// Mercenary

	"saxxy",						// Base weapon to translate
	NULL,
	"tf_weapon_bat",				// Scout
	"tf_weapon_club",				// Sniper
	"tf_weapon_shovel",				// Soldier
	"tf_weapon_bottle",				// Demoman
	"tf_weapon_bonesaw",			// Medic
	"tf_weapon_fireaxe",			// Heavy
	"tf_weapon_fireaxe",			// Pyro
	"tf_weapon_knife",				// Spy
	"tf_weapon_wrench",				// Engineer
	"tf_weapon_umbrella",			// Civilian
	"tf_weapon_crowbar",			// Mercenary

	"tf_weapon_throwable",			// Base weapon to translate
	NULL,
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88D0
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88B2
	"tf_weapon_throwable", //UNK_10D88B2

	"tf_weapon_parachute",			// Base weapon to translate
	NULL,
	"tf_weapon_parachute_secondary",	// Scout
	"tf_weapon_parachute_secondary",	// Sniper
	"tf_weapon_parachute_primary",		// Soldier
	"tf_weapon_parachute_secondary",	// Demoman
	"tf_weapon_parachute_secondary",	// Medic
	"tf_weapon_parachute_secondary",	// Heavy
	"tf_weapon_parachute_secondary",	// Pyro
	"tf_weapon_parachute_secondary",	// Spy
	0,									// Engineer
	"tf_weapon_parachute_secondary",	// Civilian
	"tf_weapon_parachute_secondary",	// Mercenary

	"tf_weapon_revolver",			// Base weapon to translate
	NULL,
	"tf_weapon_revolver",			// Scout
	"tf_weapon_revolver",			// Sniper
	"tf_weapon_revolver",			// Soldier
	"tf_weapon_revolver",			// Demoman
	"tf_weapon_revolver",			// Medic
	"tf_weapon_revolver",			// Heavy
	"tf_weapon_revolver",			// Pyro
	"tf_weapon_revolver",			// Spy
	"tf_weapon_revolver_secondary",	// Engineer
	"tf_weapon_revolver",			// Civilian
	"tf_weapon_revolver",			// Mercenary
};

//-----------------------------------------------------------------------------
// Weapons.
//-----------------------------------------------------------------------------
const char *g_aWeaponNames[] =
{
	"TF_WEAPON_NONE",
	"TF_WEAPON_BAT",
	"TF_WEAPON_BOTTLE", 
	"TF_WEAPON_FIREAXE",
	"TF_WEAPON_CLUB",
	"TF_WEAPON_CROWBAR",
	"TF_WEAPON_KNIFE",
	"TF_WEAPON_FISTS",
	"TF_WEAPON_SHOVEL",
	"TF_WEAPON_WRENCH",
	"TF_WEAPON_BONESAW",
	"TF_WEAPON_SHOTGUN_PRIMARY",
	"TF_WEAPON_SHOTGUN_SOLDIER",
	"TF_WEAPON_SHOTGUN_HWG",
	"TF_WEAPON_SHOTGUN_PYRO",
	"TF_WEAPON_SCATTERGUN",
	"TF_WEAPON_SNIPERRIFLE",
	"TF_WEAPON_MINIGUN",
	"TF_WEAPON_SMG",
	"TF_WEAPON_SYRINGEGUN_MEDIC",
	"TF_WEAPON_TRANQ",
	"TF_WEAPON_ROCKETLAUNCHER",
	"TF_WEAPON_GRENADELAUNCHER",
	"TF_WEAPON_PIPEBOMBLAUNCHER",
	"TF_WEAPON_FLAMETHROWER",
	"TF_WEAPON_GRENADE_NORMAL",
	"TF_WEAPON_GRENADE_CONCUSSION",
	"TF_WEAPON_GRENADE_NAIL",
	"TF_WEAPON_GRENADE_MIRV",
	"TF_WEAPON_GRENADE_MIRV_DEMOMAN",
	"TF_WEAPON_GRENADE_NAPALM",
	"TF_WEAPON_GRENADE_GAS",
	"TF_WEAPON_GRENADE_EMP",
	"TF_WEAPON_GRENADE_CALTROP",
	"TF_WEAPON_GRENADE_PIPEBOMB",
	"TF_WEAPON_GRENADE_SMOKE_BOMB",
	"TF_WEAPON_GRENADE_HEAL",
	"TF_WEAPON_PISTOL",
	"TF_WEAPON_PISTOL_SCOUT",
	"TF_WEAPON_REVOLVER",
	"TF_WEAPON_NAILGUN",
	"TF_WEAPON_PDA",
	"TF_WEAPON_PDA_ENGINEER_BUILD",
	"TF_WEAPON_PDA_ENGINEER_DESTROY",
	"TF_WEAPON_PDA_SPY",
	"TF_WEAPON_BUILDER",
	"TF_WEAPON_MEDIGUN",
	"TF_WEAPON_GRENADE_MIRVBOMB",
	"TF_WEAPON_FLAMETHROWER_ROCKET",
	"TF_WEAPON_GRENADE_DEMOMAN",
	"TF_WEAPON_SENTRY_BULLET",
	"TF_WEAPON_SENTRY_ROCKET",
	"TF_WEAPON_DISPENSER",
	"TF_WEAPON_INVIS",
	"TF_WEAPON_FLAG",
	"TF_WEAPON_FLAREGUN",
	"TF_WEAPON_LUNCHBOX",
	"TF_WEAPON_COMPOUND_BOW",
	// ADD TF2C WEAPONS AFTER THIS
	"TF_WEAPON_HUNTERRIFLE",
	"TF_WEAPON_UMBRELLA",
	"TF_WEAPON_HAMMERFISTS",
	"TF_WEAPON_CHAINSAW",
	"TF_WEAPON_HEAVYARTILLERY",
	"TF_WEAPON_ASSAULTRIFLE",
	"TF_WEAPON_DISPLACER",

	"TF_WEAPON_COUNT",	// end marker, do not add below here
};

int g_aWeaponDamageTypes[] =
{
	DMG_GENERIC,	// TF_WEAPON_NONE
	DMG_CLUB,		// TF_WEAPON_BAT,
	DMG_CLUB,		// TF_WEAPON_BOTTLE, 
	DMG_CLUB,		// TF_WEAPON_FIREAXE,
	DMG_CLUB,		// TF_WEAPON_CLUB,
	DMG_CLUB,		// TF_WEAPON_CROWBAR,
	DMG_SLASH,		// TF_WEAPON_KNIFE,
	DMG_CLUB,		// TF_WEAPON_FISTS,
	DMG_CLUB,		// TF_WEAPON_SHOVEL,
	DMG_CLUB,		// TF_WEAPON_WRENCH,
	DMG_SLASH,		// TF_WEAPON_BONESAW,
	DMG_BUCKSHOT | DMG_USEDISTANCEMOD,	// TF_WEAPON_SHOTGUN_PRIMARY,
	DMG_BUCKSHOT | DMG_USEDISTANCEMOD,	// TF_WEAPON_SHOTGUN_SOLDIER,
	DMG_BUCKSHOT | DMG_USEDISTANCEMOD,	// TF_WEAPON_SHOTGUN_HWG,
	DMG_BUCKSHOT | DMG_USEDISTANCEMOD,	// TF_WEAPON_SHOTGUN_PYRO,
	DMG_BUCKSHOT | DMG_USEDISTANCEMOD,  // TF_WEAPON_SCATTERGUN,
	DMG_BULLET | DMG_USE_HITLOCATIONS,	// TF_WEAPON_SNIPERRIFLE,
	DMG_BULLET | DMG_USEDISTANCEMOD,		// TF_WEAPON_MINIGUN,
	DMG_BULLET | DMG_USEDISTANCEMOD,		// TF_WEAPON_SMG,
	DMG_BULLET | DMG_USEDISTANCEMOD | DMG_NOCLOSEDISTANCEMOD | DMG_PREVENT_PHYSICS_FORCE,		// TF_WEAPON_SYRINGEGUN_MEDIC,
	DMG_BULLET | DMG_USEDISTANCEMOD | DMG_PREVENT_PHYSICS_FORCE | DMG_PARALYZE,		// TF_WEAPON_TRANQ,
	DMG_BLAST | DMG_HALF_FALLOFF | DMG_USEDISTANCEMOD,		// TF_WEAPON_ROCKETLAUNCHER,
	DMG_BLAST | DMG_HALF_FALLOFF | DMG_USEDISTANCEMOD,		// TF_WEAPON_GRENADELAUNCHER,
	DMG_BLAST | DMG_HALF_FALLOFF,		// TF_WEAPON_PIPEBOMBLAUNCHER,
	DMG_IGNITE | DMG_PREVENT_PHYSICS_FORCE | DMG_PREVENT_PHYSICS_FORCE,		// TF_WEAPON_FLAMETHROWER,
	DMG_BLAST | DMG_HALF_FALLOFF,		// TF_WEAPON_GRENADE_NORMAL,
	DMG_SONIC | DMG_HALF_FALLOFF,		// TF_WEAPON_GRENADE_CONCUSSION,
	DMG_BULLET | DMG_HALF_FALLOFF,		// TF_WEAPON_GRENADE_NAIL,
	DMG_BLAST | DMG_HALF_FALLOFF,		// TF_WEAPON_GRENADE_MIRV,
	DMG_BLAST | DMG_HALF_FALLOFF,		// TF_WEAPON_GRENADE_MIRV_DEMOMAN,
	DMG_BURN | DMG_RADIUS_MAX,		// TF_WEAPON_GRENADE_NAPALM,
	DMG_POISON | DMG_HALF_FALLOFF,		// TF_WEAPON_GRENADE_GAS,
	DMG_BLAST | DMG_HALF_FALLOFF | DMG_PREVENT_PHYSICS_FORCE,		// TF_WEAPON_GRENADE_EMP,
	DMG_GENERIC,	// TF_WEAPON_GRENADE_CALTROP,
	DMG_BLAST | DMG_HALF_FALLOFF,		// TF_WEAPON_GRENADE_PIPEBOMB,
	DMG_GENERIC,	// TF_WEAPON_GRENADE_SMOKE_BOMB,
	DMG_GENERIC,	// TF_WEAPON_GRENADE_HEAL
	DMG_BULLET | DMG_USEDISTANCEMOD,		// TF_WEAPON_PISTOL,
	DMG_BULLET | DMG_USEDISTANCEMOD,		// TF_WEAPON_PISTOL_SCOUT,
	DMG_BULLET | DMG_USEDISTANCEMOD,		// TF_WEAPON_REVOLVER,
	DMG_BULLET | DMG_USEDISTANCEMOD | DMG_NOCLOSEDISTANCEMOD | DMG_PREVENT_PHYSICS_FORCE,		// TF_WEAPON_NAILGUN,
	DMG_BULLET,		// TF_WEAPON_PDA,
	DMG_BULLET,		// TF_WEAPON_PDA_ENGINEER_BUILD,
	DMG_BULLET,		// TF_WEAPON_PDA_ENGINEER_DESTROY,
	DMG_BULLET,		// TF_WEAPON_PDA_SPY,
	DMG_BULLET,		// TF_WEAPON_BUILDER
	DMG_BULLET,		// TF_WEAPON_MEDIGUN
	DMG_BLAST,		// TF_WEAPON_GRENADE_MIRVBOMB
	DMG_BLAST | DMG_IGNITE | DMG_RADIUS_MAX,		// TF_WEAPON_FLAMETHROWER_ROCKET
	DMG_BLAST | DMG_HALF_FALLOFF,					// TF_WEAPON_GRENADE_DEMOMAN
	DMG_GENERIC,	// TF_WEAPON_SENTRY_BULLET
	DMG_GENERIC,	// TF_WEAPON_SENTRY_ROCKET
	DMG_GENERIC,	// TF_WEAPON_DISPENSER
	DMG_GENERIC,	// TF_WEAPON_INVIS
	DMG_GENERIC,	// TF_WEAPON_FLAG
	DMG_IGNITE,		// TF_WEAPON_FLAREGUN,
	DMG_GENERIC,	// TF_WEAPON_LUNCHBOX,
	DMG_BULLET,		// TF_WEAPON_COMPOUND_BOW
	// ADD TF2C WEAPONS AFTER THIS
	DMG_BULLET | DMG_USE_HITLOCATIONS,	//TF_WEAPON_HUNTERRIFLE
	DMG_CLUB, // TF_WEAPON_UMBRELLA
	DMG_CLUB,		// TF_WEAPON_HAMMERFISTS
	DMG_SLASH,		// TF_WEAPON_CHAINSAW
	DMG_BULLET | DMG_USEDISTANCEMOD,		// TF_WEAPON_HEAVYARTILLERY
	DMG_BULLET | DMG_USEDISTANCEMOD,		// TF_WEAPON_ASSAULTRIFLE
	DMG_BLAST | DMG_HALF_FALLOFF | DMG_USEDISTANCEMOD,	// TF_WEAPON_DISPLACER

	// This is a special entry that must match with TF_WEAPON_COUNT
	// to protect against updating the weapon list without updating this list
	TF_DMG_SENTINEL_VALUE
};

// Spread pattern for tf_use_fixed_weaponspreads.
const Vector g_vecFixedWpnSpreadPellets[] =
{
	Vector( 0, 0, 0 ),
	Vector( 1, 0, 0 ),
	Vector( -1, 0, 0 ),
	Vector( 0, -1, 0 ),
	Vector( 0, 1, 0 ),
	Vector( 0.85, -0.85, 0 ),
	Vector( 0.85, 0.85, 0 ),
	Vector( -0.85, -0.85, 0 ),
	Vector( -0.85, 0.85, 0 ),
	Vector( 0, 0, 0 ),
};

const char *g_szProjectileNames[] =
{
	"",
	"projectile_bullet",
	"projectile_rocket",
	"projectile_pipe",
	"projectile_pipe_remote",
	"projectile_syringe",
	"projectile_flare",
	"projectile_jar",
	"projectile_arrow",
	"projectile_flame_rocket",
	"projectile_jar_milk",
	"projectile_healing_bolt",
	"projectile_energy_ball",
	"projectile_energy_ring",
	"projectile_pipe_remote_practice",
	"projectile_cleaver",
	"projectile_sticky_ball",
	"projectile_cannonball",
	"projectile_building_repair_bolt",
	"projectile_festive_arrow",
	"projectile_throwable",
	"projectile_spellfireball",
	"projectile_festive_urine",
	"projectile_festive_healing_bolt",
	"projectfile_breadmonster_jarate",
	"projectfile_breadmonster_madmilk",
	"projectile_grapplinghook",
	"projectile_sentry_rocket",
	"projectile_bread_monster",
	// Add new projectiles here.
	"projectile_nail",
	"projectile_dart",
	"projectile_mirv",
	"projectile_plasma",
};

// these map to the projectiles named in g_szProjectileNames
int g_iProjectileWeapons[] = 
{
	TF_WEAPON_NONE,
	TF_WEAPON_PISTOL,
	TF_WEAPON_ROCKETLAUNCHER,
	TF_WEAPON_PIPEBOMBLAUNCHER,
	TF_WEAPON_GRENADELAUNCHER,
	TF_WEAPON_SYRINGEGUN_MEDIC,
	TF_WEAPON_NAILGUN,
	TF_WEAPON_TRANQ,
	TF_WEAPON_FLAREGUN,
};

const char *g_pszHintMessages[] =
{
	"#Hint_spotted_a_friend",
	"#Hint_spotted_an_enemy",
	"#Hint_killing_enemies_is_good",
	"#Hint_out_of_ammo",
	"#Hint_turn_off_hints",
	"#Hint_pickup_ammo",
	"#Hint_Cannot_Teleport_With_Flag",
	"#Hint_Cannot_Cloak_With_Flag",
	"#Hint_Cannot_Disguise_With_Flag",
	"#Hint_Cannot_Attack_While_Cloaked",
	"#Hint_ClassMenu",

// Grenades
	"#Hint_gren_caltrops",
	"#Hint_gren_concussion",
	"#Hint_gren_emp",
	"#Hint_gren_gas",
	"#Hint_gren_mirv",
	"#Hint_gren_nail",
	"#Hint_gren_napalm",
	"#Hint_gren_normal",

// Altfires
	"#Hint_altfire_sniperrifle",
	"#Hint_altfire_flamethrower",
	"#Hint_altfire_grenadelauncher",
	"#Hint_altfire_pipebomblauncher",
	"#Hint_altfire_rotate_building",

// Soldier
	"#Hint_Soldier_rpg_reload",

// Engineer
	"#Hint_Engineer_use_wrench_onown",
	"#Hint_Engineer_use_wrench_onother",
	"#Hint_Engineer_use_wrench_onfriend",
	"#Hint_Engineer_build_sentrygun",
	"#Hint_Engineer_build_dispenser",
	"#Hint_Engineer_build_teleporters",
	"#Hint_Engineer_pickup_metal",
	"#Hint_Engineer_repair_object",
	"#Hint_Engineer_metal_to_upgrade",
	"#Hint_Engineer_upgrade_sentrygun",

	"#Hint_object_has_sapper",

	"#Hint_object_your_object_sapped",
	"#Hint_enemy_using_dispenser",
	"#Hint_enemy_using_tp_entrance",
	"#Hint_enemy_using_tp_exit",
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int GetWeaponId( const char *pszWeaponName )
{
	// if this doesn't match, you need to add missing weapons to the array
	assert( ARRAYSIZE( g_aWeaponNames ) == ( TF_WEAPON_COUNT + 1 ) );

	for ( int iWeapon = 0; iWeapon < ARRAYSIZE( g_aWeaponNames ); ++iWeapon )
	{
		if ( !Q_stricmp( pszWeaponName, g_aWeaponNames[iWeapon] ) )
			return iWeapon;
	}

	return TF_WEAPON_NONE;
}


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *WeaponIdToAlias( int iWeapon )
{
	// if this doesn't match, you need to add missing weapons to the array
	assert( ARRAYSIZE( g_aWeaponNames ) == ( TF_WEAPON_COUNT + 1 ) );

	if ( ( iWeapon >= ARRAYSIZE( g_aWeaponNames ) ) || ( iWeapon < 0 ) )
		return NULL;

	return g_aWeaponNames[iWeapon];
}

//-----------------------------------------------------------------------------
// Purpose: Entity classnames need to be in lower case. Use this whenever
// you're spawning a weapon.
//-----------------------------------------------------------------------------
const char *WeaponIdToClassname( int iWeapon )
{
	const char *pszWeaponAlias = WeaponIdToAlias( iWeapon );

	if ( pszWeaponAlias == NULL )
		return NULL;

	static char szEntName[256];
	V_strcpy( szEntName, pszWeaponAlias );
	V_strlower( szEntName );

	return szEntName;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
const char *TranslateWeaponEntForClass( const char *pszName, int iClass )
{
	if ( pszName )
	{
		for ( int i = 0; i < ARRAYSIZE( pszWpnEntTranslationList ); i++ )
		{
			if ( V_stricmp( pszName, pszWpnEntTranslationList[i].weapon_name ) == 0 )
			{
				return ( (const char **)&( pszWpnEntTranslationList[i] ) )[1 + iClass];
			}
		}
	}
	return pszName;
}

#ifdef GAME_DLL

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int GetWeaponFromDamage( const CTakeDamageInfo &info )
{
	int iWeapon = TF_WEAPON_NONE;

	// Work out what killed the player, and send a message to all clients about it
	TFGameRules()->GetKillingWeaponName( info, NULL, iWeapon );

	return iWeapon;
}

#endif

//-----------------------------------------------------------------------------
// Conditions stuff.
//-----------------------------------------------------------------------------
int condition_to_attribute_translation[] =
{
	TF_COND_BURNING,
	TF_COND_AIMING,
	TF_COND_ZOOMED,
	TF_COND_DISGUISING,
	TF_COND_DISGUISED,
	TF_COND_STEALTHED,
	TF_COND_INVULNERABLE,
	TF_COND_TELEPORTED,
	TF_COND_TAUNTING,
	TF_COND_INVULNERABLE_WEARINGOFF,
	TF_COND_STEALTHED_BLINK,
	TF_COND_SELECTED_TO_TELEPORT,
	TF_COND_CRITBOOSTED,
	TF_COND_TMPDAMAGEBONUS,
	TF_COND_FEIGN_DEATH,
	TF_COND_PHASE,
	TF_COND_STUNNED,
	TF_COND_HEALTH_BUFF,
	TF_COND_HEALTH_OVERHEALED,
	TF_COND_URINE,
	TF_COND_ENERGY_BUFF,
	TF_COND_LAST
};

int g_aPowerupConds[] =
{
	TF_COND_POWERUP_CRITDAMAGE,
	TF_COND_POWERUP_SHIELD,
	TF_COND_POWERUP_SPEEDBOOST,
	TF_COND_POWERUP_CLOAK,
	TF_COND_POWERUP_RAGEMODE,
	TF_COND_LAST
};

const char *g_aPowerupNames[] =
{
	"item_powerup_critdamage", // TF_COND_POWERUP_CRITDAMAGE,
	"item_powerup_shield", // TF_COND_POWERUP_SHIELD,
	"item_powerup_speedboost", // TF_COND_POWERUP_SPEEDBOOST,
	"item_powerup_cloak", // TF_COND_POWERUP_CLOAK,
	"item_powerup_ragemode", // TF_COND_POWERUP_RAGEMODE,
};

COMPILE_TIME_ASSERT( ( ARRAYSIZE( g_aPowerupConds ) - 1 ) == ARRAYSIZE( g_aPowerupNames ) );

bool ConditionExpiresFast( int nCond )
{
	// Damaging conds
	if ( nCond == TF_COND_BURNING ||
		nCond == TF_COND_BLEEDING )
		return true;

	// Liquids
	if ( nCond == TF_COND_URINE ||
		nCond == TF_COND_MAD_MILK )
		return true;

	// Tranq
	if ( nCond == TF_COND_SLOWED )
		return true;

	return false;
}


//-----------------------------------------------------------------------------
// Mediguns.
//-----------------------------------------------------------------------------
MedigunEffects_t g_MedigunEffects[] =
{
	{ TF_COND_INVULNERABLE, TF_COND_INVULNERABLE_WEARINGOFF, "TFPlayer.InvulnerableOn", "TFPlayer.InvulnerableOff" },
	{ TF_COND_CRITBOOSTED, TF_COND_LAST, "TFPlayer.CritBoostOn", "TFPlayer.CritBoostOff" },
	{ TF_COND_MEGAHEAL, TF_COND_LAST, "TFPlayer.QuickFixInvulnerableOn", "TFPlayer.MegaHealOff" },
	{ TF_COND_MEDIGUN_UBER_BULLET_RESIST, TF_COND_LAST, "WeaponMedigun_Vaccinator.InvulnerableOn", "WeaponMedigun_Vaccinator.InvulnerableOff" },
	{ TF_COND_MEDIGUN_UBER_BLAST_RESIST, TF_COND_LAST, "WeaponMedigun_Vaccinator.InvulnerableOn", "WeaponMedigun_Vaccinator.InvulnerableOff" },
	{ TF_COND_MEDIGUN_UBER_FIRE_RESIST, TF_COND_LAST, "WeaponMedigun_Vaccinator.InvulnerableOn", "WeaponMedigun_Vaccinator.InvulnerableOff" },
};

// ------------------------------------------------------------------------------------------------ //
// CObjectInfo tables.
// ------------------------------------------------------------------------------------------------ //

CObjectInfo::CObjectInfo( char *pObjectName )
{
	m_pObjectName = pObjectName;
	m_pClassName = NULL;
	m_flBuildTime = -9999;
	m_nMaxObjects = -9999;
	m_Cost = -9999;
	m_CostMultiplierPerInstance = -999;
	m_UpgradeCost = -9999;
	m_flUpgradeDuration = -9999;
	m_MaxUpgradeLevel = -9999;
	m_pBuilderWeaponName = NULL;
	m_pBuilderPlacementString = NULL;
	m_SelectionSlot = -9999;
	m_SelectionPosition = -9999;
	m_bSolidToPlayerMovement = false;
	m_pIconActive = NULL;
	m_pIconInactive = NULL;
	m_pIconMenu = NULL;
	m_pViewModel = NULL;
	m_pPlayerModel = NULL;
	m_iDisplayPriority = 0;
	m_bVisibleInWeaponSelection = true;
	m_pExplodeSound = NULL;
	m_pExplosionParticleEffect = NULL;
	m_bAutoSwitchTo = false;
	m_pUpgradeSound = NULL;
}


CObjectInfo::~CObjectInfo()
{
	delete [] m_pClassName;
	delete [] m_pStatusName;
	delete [] m_pBuilderWeaponName;
	delete [] m_pBuilderPlacementString;
	delete [] m_pIconActive;
	delete [] m_pIconInactive;
	delete [] m_pIconMenu;
	delete [] m_pViewModel;
	delete [] m_pPlayerModel;
	delete [] m_pExplodeSound;
	delete [] m_pExplosionParticleEffect;
	delete [] m_pUpgradeSound;
}

CObjectInfo g_ObjectInfos[OBJ_LAST] =
{
	CObjectInfo( "OBJ_DISPENSER" ),
	CObjectInfo( "OBJ_TELEPORTER" ),
	CObjectInfo( "OBJ_SENTRYGUN" ),
	CObjectInfo( "OBJ_ATTACHMENT_SAPPER" ),
};

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
int GetBuildableId( const char *pszBuildableName )
{
	for ( int iBuildable = 0; iBuildable < OBJ_LAST; ++iBuildable )
	{
		if ( !Q_stricmp( pszBuildableName, g_ObjectInfos[iBuildable].m_pObjectName ) )
			return iBuildable;
	}

	return OBJ_LAST;
}

bool AreObjectInfosLoaded()
{
	return g_ObjectInfos[0].m_pClassName != NULL;
}


void LoadObjectInfos( IBaseFileSystem *pFileSystem )
{
	const char *pFilename = "scripts/objects.txt";

	// Make sure this stuff hasn't already been loaded.
	Assert( !AreObjectInfosLoaded() );

	KeyValues *pValues = new KeyValues( "Object descriptions" );
	if ( !pValues->LoadFromFile( pFileSystem, pFilename, "GAME" ) )
	{
		Error( "Can't open %s for object info.", pFilename );
		pValues->deleteThis();
		return;
	}

	// Now read each class's information in.
	for ( int iObj=0; iObj < ARRAYSIZE( g_ObjectInfos ); iObj++ )
	{
		CObjectInfo *pInfo = &g_ObjectInfos[iObj];
		KeyValues *pSub = pValues->FindKey( pInfo->m_pObjectName );
		if ( !pSub )
		{
			Error( "Missing section '%s' from %s.", pInfo->m_pObjectName, pFilename );
			pValues->deleteThis();
			return;
		}

		// Read all the info in.
		if ( (pInfo->m_flBuildTime = pSub->GetFloat( "BuildTime", -999 )) == -999 ||
			(pInfo->m_nMaxObjects = pSub->GetInt( "MaxObjects", -999 )) == -999 ||
			(pInfo->m_Cost = pSub->GetInt( "Cost", -999 )) == -999 ||
			(pInfo->m_CostMultiplierPerInstance = pSub->GetFloat( "CostMultiplier", -999 )) == -999 ||
			(pInfo->m_UpgradeCost = pSub->GetInt( "UpgradeCost", -999 )) == -999 ||
			(pInfo->m_flUpgradeDuration = pSub->GetFloat( "UpgradeDuration", -999)) == -999 ||
			(pInfo->m_MaxUpgradeLevel = pSub->GetInt( "MaxUpgradeLevel", -999 )) == -999 ||
			(pInfo->m_SelectionSlot = pSub->GetInt( "SelectionSlot", -999 )) == -999 ||
			(pInfo->m_BuildCount = pSub->GetInt( "BuildCount", -999 )) == -999 ||
			(pInfo->m_SelectionPosition = pSub->GetInt( "SelectionPosition", -999 )) == -999 )
		{
			Error( "Missing data for object '%s' in %s.", pInfo->m_pObjectName, pFilename );
			pValues->deleteThis();
			return;
		}

		pInfo->m_pClassName = ReadAndAllocStringValue( pSub, "ClassName", pFilename );
		pInfo->m_pStatusName = ReadAndAllocStringValue( pSub, "StatusName", pFilename );
		pInfo->m_pBuilderWeaponName = ReadAndAllocStringValue( pSub, "BuilderWeaponName", pFilename );
		pInfo->m_pBuilderPlacementString = ReadAndAllocStringValue( pSub, "BuilderPlacementString", pFilename );
		pInfo->m_bSolidToPlayerMovement = pSub->GetInt( "SolidToPlayerMovement", 0 ) ? true : false;
		pInfo->m_pIconActive = ReadAndAllocStringValue( pSub, "IconActive", pFilename );
		pInfo->m_pIconInactive = ReadAndAllocStringValue( pSub, "IconInactive", pFilename );
		pInfo->m_pIconMenu = ReadAndAllocStringValue( pSub, "IconMenu", pFilename );
		pInfo->m_bUseItemInfo = pSub->GetInt( "UseItemInfo", 0 ) ? true : false;
		pInfo->m_pViewModel = ReadAndAllocStringValue( pSub, "Viewmodel", pFilename );
		pInfo->m_pPlayerModel = ReadAndAllocStringValue( pSub, "Playermodel", pFilename );
		pInfo->m_iDisplayPriority = pSub->GetInt( "DisplayPriority", 0 );
		pInfo->m_pHudStatusIcon = ReadAndAllocStringValue( pSub, "HudStatusIcon", pFilename );
		pInfo->m_bVisibleInWeaponSelection = ( pSub->GetInt( "VisibleInWeaponSelection", 1 ) > 0 );
		pInfo->m_pExplodeSound = ReadAndAllocStringValue( pSub, "ExplodeSound", pFilename );
		pInfo->m_pUpgradeSound = ReadAndAllocStringValue( pSub, "UpgradeSound", pFilename );
		pInfo->m_pExplosionParticleEffect = ReadAndAllocStringValue( pSub, "ExplodeEffect", pFilename );
		pInfo->m_bAutoSwitchTo = ( pSub->GetInt( "autoswitchto", 0 ) > 0 );

		pInfo->m_iMetalToDropInGibs = pSub->GetInt( "MetalToDropInGibs", 0 );
		pInfo->m_bRequiresOwnBuilder = pSub->GetBool( "RequiresOwnBuilder", 0 );
		// PistonMiner: Added Object Mode key
		KeyValues *pAltModes = pSub->FindKey("AltModes");
		if (pAltModes)
		{
			for (int i = 0; i < 4; ++i) // load at most 4 object modes
			{
				char altModeBuffer[256]; // Max size of 0x100
				V_snprintf(altModeBuffer, ARRAYSIZE(altModeBuffer), "AltMode%d", i);
				KeyValues *pCurAltMode = pAltModes->FindKey(altModeBuffer);
				if (!pCurAltMode)
					break;

				// Save logic here
				pInfo->m_AltModes.AddToTail(ReadAndAllocStringValue( pCurAltMode, "StatusName", pFilename ));
				pInfo->m_AltModes.AddToTail(ReadAndAllocStringValue( pCurAltMode, "ModeName", pFilename ));
				pInfo->m_AltModes.AddToTail(ReadAndAllocStringValue( pCurAltMode, "IconMenu", pFilename ));
			}
		}
	}

	pValues->deleteThis();
}


const CObjectInfo* GetObjectInfo( int iObject )
{
	Assert( iObject >= 0 && iObject < OBJ_LAST );
	Assert( AreObjectInfosLoaded() );
	return &g_ObjectInfos[iObject];
}

ConVar tf_cheapobjects( "tf_cheapobjects","0", FCVAR_CHEAT | FCVAR_DEVELOPMENTONLY | FCVAR_REPLICATED, "Set to 1 and all objects will cost 0" );

//-----------------------------------------------------------------------------
// Purpose: Return the cost of another object of the specified type
//			If bLast is set, return the cost of the last built object of the specified type
// 
// Note: Used to contain logic from tf2 that multiple instances of the same object
//       cost different amounts. See tf2/game_shared/tf_shareddefs.cpp for details
//-----------------------------------------------------------------------------
int CalculateObjectCost( int iObjectType )
{
	if ( tf_cheapobjects.GetInt() )
	{
		return 0;
	}

	int iCost = GetObjectInfo( iObjectType )->m_Cost;

	return iCost;
}

//-----------------------------------------------------------------------------
// Purpose: Calculate the cost to upgrade an object of a specific type
//-----------------------------------------------------------------------------
int	CalculateObjectUpgrade( int iObjectType, int iObjectLevel )
{
	// Max level?
	if ( iObjectLevel >= GetObjectInfo( iObjectType )->m_MaxUpgradeLevel )
		return 0;

	int iCost = GetObjectInfo( iObjectType )->m_UpgradeCost;
	for ( int i = 0; i < (iObjectLevel - 1); i++ )
	{
		iCost *= OBJECT_UPGRADE_COST_MULTIPLIER_PER_LEVEL;
	}

	return iCost;
}

//-----------------------------------------------------------------------------
// Purpose: Return true if the specified class is allowed to build the specified object type
//-----------------------------------------------------------------------------
bool ClassCanBuild( int iClass, int iObjectType )
{
	/*
	for ( int i = 0; i < OBJ_LAST; i++ )
	{
		// Hit the end?
		if ( g_TFClassInfos[iClass].m_pClassObjects[i] == OBJ_LAST )
			return false;

		// Found it?
		if ( g_TFClassInfos[iClass].m_pClassObjects[i] == iObjectType )
			return true;
	}

	return false;
	*/

	return ( iClass == TF_CLASS_ENGINEER );
}

float g_flTeleporterRechargeTimes[] =
{
	10.0,
	5.0,
	3.0
};

float g_flDispenserAmmoRates[] =
{
	0.2,
	0.3,
	0.4
};

float g_flDispenserHealRates[] =
{
	10.0,
	15.0,
	20.0
};
