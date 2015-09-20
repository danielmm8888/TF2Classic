//========= Copyright © 1996-2003, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
//=============================================================================

#include "cbase.h"
#include "usermessages.h"
#include "shake.h"
#include "voice_gamemgr.h"

// NVNT include to register in haptic user messages
#include "haptics/haptic_msgs.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

void RegisterUserMessages()
{
	usermessages->Register("Geiger", 1);		// geiger info data
	usermessages->Register("Train", 1);		// train control data
	usermessages->Register("HudText", -1);
	usermessages->Register("SayText", -1);
	usermessages->Register("SayText2", -1);
	usermessages->Register("TextMsg", -1);
	usermessages->Register("ResetHUD", 1);	// called every respawn
	usermessages->Register("GameTitle", 0);	// show game title
	usermessages->Register("ItemPickup", -1);	// for item history on screen
	usermessages->Register("ShowMenu", -1);	// show hud menu
	usermessages->Register("Shake", 13);		// shake view
	usermessages->Register("Fade", 10);		// fade HUD in/out
	usermessages->Register("VGUIMenu", -1);	// Show VGUI menu
	usermessages->Register("Rumble", 3);	// Send a rumble to a controller
	usermessages->Register("CloseCaption", -1); // Show a caption (by string id number)(duration in 10th of a second)

	usermessages->Register("SendAudio", -1);	// play radion command

	usermessages->Register("VoiceMask", VOICE_MAX_PLAYERS_DW * 4 * 2 + 1);
	usermessages->Register("RequestState", 0);

	usermessages->Register("Damage", -1);		// for HUD damage indicators
	usermessages->Register("HintText", -1);	// Displays hint text display
	usermessages->Register("KeyHintText", -1);	// Displays hint text display

	usermessages->Register("HudMsg", -1);
	usermessages->Register("AmmoDenied", 2);
	usermessages->Register("AchievementEvent", -1);

	usermessages->Register("UpdateRadar", -1);

	usermessages->Register("VoiceSubtitle", 3);

	usermessages->Register("HudNotify", 1);
	usermessages->Register("HudNotifyCustom", -1);

	usermessages->Register("PlayerStatsUpdate", -1);

	usermessages->Register("PlayerIgnited", 3);
	usermessages->Register("PlayerIgnitedInv", 3);
	usermessages->Register("HudArenaNotify", 2);
	usermessages->Register("UpdateAchievement", -1);
	usermessages->Register("TrainingMsg", -1);
	usermessages->Register("TrainingObjective", -1);
	usermessages->Register("DamageDodged", -1);
	usermessages->Register("PlayerJarated", 2);
	usermessages->Register("PlayerExtinguished", 2);
	usermessages->Register("PlayerJaratedFade", 2);
	usermessages->Register("PlayerShieldBlocked", 2);
	usermessages->Register("BreakModel", -1);
	usermessages->Register("CheapBreakModel", -1);
	usermessages->Register("BreakModel_Pumpkin", -1);
	usermessages->Register("BreakModelRocketDud", -1);

	usermessages->Register("CallVoteFailed", -1);
	usermessages->Register("VoteStart", -1);
	usermessages->Register("VotePass", -1);
	usermessages->Register("VoteFailed", 2);
	usermessages->Register("VoteSetup", -1);

	usermessages->Register("PlayerBonusPoints", 3);
	usermessages->Register("RDTeamPointsChanged", 4);
	usermessages->Register("SpawnFlyingBird", -1);
	usermessages->Register("PlayerGodRayEffect", -1);
	usermessages->Register("PlayerTeleportHomeEffect", -1);
	usermessages->Register("MVMStatsReset", -1);
	usermessages->Register("MVMPlayerEvent", -1);
	usermessages->Register("MVMResetPlayerStats", -1);
	usermessages->Register("MVMWaveFailed", 0);
	usermessages->Register("MVMAnnouncement", 2);
	usermessages->Register("MVMPlayerUpgradedEvent", 9);
	usermessages->Register("MVMVictory", 2);
	usermessages->Register("MVMWaveChange", 15);
	usermessages->Register("MVMLocalPlayerUpgradesClear", 1);
	usermessages->Register("MVMLocalPlayerUpgradesValue", 6);
	usermessages->Register("MVMResetPlayerWaveSpendingStats", 1);
	usermessages->Register("MVMLocalPlayerWaveSpendingValue", 12);
	usermessages->Register("MVMResetPlayerUpgradeSpending", -1);
	usermessages->Register("MVMServerKickTimeUpdate", 1);
	usermessages->Register("PlayerLoadoutUpdated", -1);
	usermessages->Register("PlayerTauntSoundLoopStart", -1);
	usermessages->Register("PlayerTauntSoundLoopEnd", -1);
	usermessages->Register("WarKilledOther", -1);
	usermessages->Register("ForcePlayerViewAngles", -1);
	usermessages->Register("BonusDucks", 2);
	usermessages->Register("EOTLDuckEvent", 7);
	usermessages->Register("PlayerPickupWeapon", -1);
	usermessages->Register("QuestObjectiveCompleted", 14);
#ifndef _X360
	// NVNT register haptic user messages
	RegisterHapticMessages();
#endif
}

