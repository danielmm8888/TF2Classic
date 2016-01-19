//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "cbase.h"
#include "hud.h"
#include "hudelement.h"
#include "c_tf_player.h"
#include "iclientmode.h"
#include "ienginevgui.h"
#include <vgui/ILocalize.h>
#include <vgui/ISurface.h>
#include <vgui/IVGui.h>
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/ProgressBar.h>
#include "engine/IEngineSound.h"
#include <vgui_controls/AnimationController.h>
#include "iclientmode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

// Floating delta text items, float off the top of the head to 
// show damage done
typedef struct
{
	// amount of delta
	int m_iAmount;

	// die time
	float m_flDieTime;

	EHANDLE m_hEntity;

	// position of damaged player
	Vector m_vDamagePos;

	bool bCrit;
} dmg_account_delta_t;

#define NUM_ACCOUNT_DELTA_ITEMS 10

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CDamageAccountPanel : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CDamageAccountPanel, EditablePanel );

public:
	CDamageAccountPanel( const char *pElementName );

	virtual void	ApplySchemeSettings( IScheme *scheme );
	virtual void	LevelInit( void );
	virtual bool	ShouldDraw( void );
	virtual void	Paint( void );

	virtual void	FireGameEvent( IGameEvent *event );
	void			OnDamaged( IGameEvent *event );

private:

	float m_flLastHitSound;

	int iAccountDeltaHead;
	dmg_account_delta_t m_AccountDeltaItems[NUM_ACCOUNT_DELTA_ITEMS];

	//CPanelAnimationVarAliasType( float, m_flDeltaItemStartPos, "delta_item_start_y", "100", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDeltaItemEndPos, "delta_item_end_y", "0", "proportional_float" );

	//CPanelAnimationVarAliasType( float, m_flDeltaItemX, "delta_item_x", "0", "proportional_float" );

	CPanelAnimationVar( Color, m_DeltaPositiveColor, "PositiveColor", "0 255 0 255" );
	CPanelAnimationVar( Color, m_DeltaNegativeColor, "NegativeColor", "255 0 0 255" );

	CPanelAnimationVar( float, m_flDeltaLifetime, "delta_lifetime", "2.0" );

	CPanelAnimationVar( vgui::HFont, m_hDeltaItemFont, "delta_item_font", "Default" );
	CPanelAnimationVar( vgui::HFont, m_hDeltaItemFontBig, "delta_item_font_big", "Default" );
};

DECLARE_HUDELEMENT( CDamageAccountPanel );

ConVar hud_combattext( "hud_combattext", "0", FCVAR_ARCHIVE, "" );
ConVar hud_combattext_batching( "hud_combattext_batching", "0", FCVAR_ARCHIVE, "If set to 1, numbers that are too close together are merged." );
ConVar hud_combattext_batching_window( "hud_combattext_batching_window", "0.2", FCVAR_ARCHIVE, "Maximum delay between damage events in order to batch numbers." );

ConVar tf_dingalingaling( "tf_dingalingaling", "0", FCVAR_ARCHIVE, "If set to 1, play a sound everytime you injure an enemy. The sound can be customized by replacing the 'tf/sound/ui/hitsound.wav' file." );
ConVar tf_dingaling_volume( "tf_dingaling_volume", "0.75", FCVAR_ARCHIVE, "Desired volume of the hit sound.", true, 0.0, true, 1.0 );
ConVar tf_dingaling_pitchmindmg( "tf_dingaling_pitchmindmg", "100", FCVAR_ARCHIVE, "Desired pitch of the hit sound when a minimal damage hit (<= 10 health) is done.", true, 1, true, 255 );
ConVar tf_dingaling_pitchmaxdmg( "tf_dingaling_pitchmaxdmg", "100", FCVAR_ARCHIVE, "Desired pitch of the hit sound when a maximum damage hit (>= 150 health) is done.", true, 1, true, 255 );
ConVar tf_dingalingaling_repeat_delay( "tf_dingalingaling_repeat_delay", "0", FCVAR_ARCHIVE, "Desired repeat delay of the hit sound. Set to 0 to play a sound for every instance of damage dealt." );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CDamageAccountPanel::CDamageAccountPanel( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "CDamageAccountPanel" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	m_flLastHitSound = 0.0f;

	iAccountDeltaHead = 0;

	for ( int i = 0; i<NUM_ACCOUNT_DELTA_ITEMS; i++ )
	{
		m_AccountDeltaItems[i].m_flDieTime = 0.0f;
	}

	ListenForGameEvent( "player_hurt" );
	ListenForGameEvent( "player_healed" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDamageAccountPanel::FireGameEvent( IGameEvent *event )
{
	// For future reference, live TF2 apparently uses player_healed for green medic numbers.
	const char * type = event->GetName();

	if ( Q_strcmp( type, "player_hurt" ) == 0 )
	{
		OnDamaged( event );
	}
	else
	{
		CHudElement::FireGameEvent( event );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDamageAccountPanel::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( "resource/UI/HudDamageAccount.res" );

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: called whenever a new level's starting
//-----------------------------------------------------------------------------
void CDamageAccountPanel::LevelInit( void )
{
	iAccountDeltaHead = 0;

	CHudElement::LevelInit();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CDamageAccountPanel::ShouldDraw( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer || !pPlayer->IsAlive() )
	{
		return false;
	}

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CDamageAccountPanel::OnDamaged( IGameEvent *event )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( pPlayer && pPlayer->IsAlive() ) 
	{
		int iAttacker = event->GetInt( "attacker" );
		int iVictim = event->GetInt( "userid" );
		int iDmgAmount = event->GetInt( "damageamount" );

		// Did we shoot the guy?
		if ( iAttacker != pPlayer->GetUserID() )
			return;

		// No self-damage notifications.
		if ( iAttacker == iVictim )
			return;

		// Don't show anything if no damage was done.
		if ( iDmgAmount == 0 )
			return;

		// Currently only supporting players.
		C_TFPlayer *pVictim = ToTFPlayer( UTIL_PlayerByUserId( iVictim ) );

		if ( !pVictim )
			return;

		// Don't show damage notifications for spies disguised as our team.
		if ( pVictim->m_Shared.InCond( TF_COND_DISGUISED ) && pVictim->m_Shared.GetDisguiseTeam() == pPlayer->GetTeamNumber() )
			return;

		// Play hit sound, if appliable.
		if ( tf_dingalingaling.GetBool() )
		{
			float flRepeatDelay = tf_dingalingaling_repeat_delay.GetFloat();
			if ( flRepeatDelay <= 0 || gpGlobals->curtime - m_flLastHitSound > flRepeatDelay )
			{
				EmitSound_t params;

				params.m_pSoundName = "ui/hitsound.wav";

				params.m_flVolume = tf_dingaling_volume.GetFloat();

				float flPitchMin = tf_dingaling_pitchmindmg.GetFloat();
				float flPitchMax = tf_dingaling_pitchmaxdmg.GetFloat();
				params.m_nPitch = RemapValClamped( (float)iDmgAmount, 10, 150, flPitchMin, flPitchMax );

				CLocalPlayerFilter filter;

				C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, params ); // Ding!

				m_flLastHitSound = gpGlobals->curtime;
			}
		}

		// Leftover from old code?
		g_pClientMode->GetViewportAnimationController()->StartAnimationSequence( "DamagedPlayer" );

		// Stop here if we chose not to show hit numbers.
		if ( !hud_combattext.GetBool() )
			return;

		// Don't show the numbers if we can't see the victim.
		trace_t tr;
		UTIL_TraceLine( pPlayer->EyePosition(), pVictim->WorldSpaceCenter(), MASK_VISIBLE, NULL, COLLISION_GROUP_NONE, &tr );
		if ( tr.fraction != 1.0f )
			return;

		if ( hud_combattext_batching.GetBool() )
		{
			// Cycle through deltas and search for one that belongs to this player.
			for ( int i = 0; i < NUM_ACCOUNT_DELTA_ITEMS; i++ )
			{
				if ( m_AccountDeltaItems[i].m_hEntity.Get() == pVictim )
				{
					// See if it's lifetime is inside batching window.
					float flCreateTime = m_AccountDeltaItems[i].m_flDieTime - m_flDeltaLifetime;
					if ( gpGlobals->curtime - flCreateTime < hud_combattext_batching_window.GetFloat() )
					{
						// Update it's die time and damage.
						m_AccountDeltaItems[i].m_flDieTime = gpGlobals->curtime + m_flDeltaLifetime;
						m_AccountDeltaItems[i].m_iAmount += iDmgAmount;
						m_AccountDeltaItems[i].m_vDamagePos = pVictim->EyePosition() + Vector( 0, 0, 18 );
						m_AccountDeltaItems[i].bCrit = event->GetInt( "crit" );
						return;
					}
				}
			}
		}

		// create a delta item that floats off the top
		dmg_account_delta_t *pNewDeltaItem = &m_AccountDeltaItems[iAccountDeltaHead];

		iAccountDeltaHead++;
		iAccountDeltaHead %= NUM_ACCOUNT_DELTA_ITEMS;

		pNewDeltaItem->m_flDieTime = gpGlobals->curtime + m_flDeltaLifetime;
		pNewDeltaItem->m_iAmount = iDmgAmount;
		pNewDeltaItem->m_hEntity = pVictim;
		pNewDeltaItem->m_vDamagePos = pVictim->EyePosition() + Vector( 0, 0, 18 );
		pNewDeltaItem->bCrit = event->GetInt( "crit" );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Paint the deltas
//-----------------------------------------------------------------------------
void CDamageAccountPanel::Paint( void )
{
	BaseClass::Paint();

	for ( int i = 0; i<NUM_ACCOUNT_DELTA_ITEMS; i++ )
	{
		// update all the valid delta items
		if ( m_AccountDeltaItems[i].m_flDieTime > gpGlobals->curtime )
		{
			// position and alpha are determined from the lifetime
			// color is determined by the delta - green for positive, red for negative

			Color c = m_DeltaNegativeColor;

			float flLifetimePercent = ( m_AccountDeltaItems[i].m_flDieTime - gpGlobals->curtime ) / m_flDeltaLifetime;

			// fade out after half our lifetime
			if ( flLifetimePercent < 0.5 )
			{
				c[3] = (int)( 255.0f * ( flLifetimePercent / 0.5 ) );
			}

			int iX, iY;
			bool bOnscreen = GetVectorInScreenSpace( m_AccountDeltaItems[i].m_vDamagePos, iX, iY );

			if ( !bOnscreen )
				continue;

			float flHeight = 50.0f;
			float flYPos = (float)iY - ( 1.0 - flLifetimePercent ) * flHeight;

			// Use BIGGER font for crits.
			vgui::surface()->DrawSetTextFont( m_AccountDeltaItems[i].bCrit ? m_hDeltaItemFontBig : m_hDeltaItemFont );
			vgui::surface()->DrawSetTextColor( c );
			vgui::surface()->DrawSetTextPos( iX, (int)flYPos );

			wchar_t wBuf[20];

			_snwprintf( wBuf, sizeof( wBuf ) / sizeof( wchar_t ), L"-%d", m_AccountDeltaItems[i].m_iAmount );

			vgui::surface()->DrawPrintText( wBuf, wcslen( wBuf ), FONT_DRAW_NONADDITIVE );
		}
	}
}