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

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

// Floating delta text items, float off the top of the frame to 
// show changes to the metal account value
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
	void			OnTick( IGameEvent *event );

private:

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

ConVar tf_dingalingaling( "tf_dingalingaling", "0", FCVAR_ARCHIVE, "" );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CDamageAccountPanel::CDamageAccountPanel( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "CDamageAccountPanel" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	iAccountDeltaHead = 0;

	SetDialogVariable( "metal", 0 );

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
		OnTick( event );
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
void CDamageAccountPanel::OnTick( IGameEvent *event )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( pPlayer && pPlayer->IsAlive() && C_TFPlayer::GetLocalTFPlayer()->GetUserID() == event->GetInt( "attacker" ) ) // Did we shoot the guy?
	{
		int iAttacker = engine->GetPlayerForUserID( event->GetInt( "attacker" ) );
		int iVictim = engine->GetPlayerForUserID( event->GetInt( "userid" ) );

		// No self-damage notifications.
		if ( iAttacker == iVictim )
			return;

		// Play hit sound, if appliable
		if ( tf_dingalingaling.GetBool() )
			vgui::surface()->PlaySound( "ui/hitsound.wav" ); // Ding!

		// Stop here if we chose not to show hit numbers
		if ( !hud_combattext.GetBool() )
			return;

		CBasePlayer *pVictim = UTIL_PlayerByIndex( iVictim );

		if ( !pVictim )
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
						m_AccountDeltaItems[i].m_iAmount += event->GetInt( "damageamount" );
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
		pNewDeltaItem->m_iAmount = event->GetInt( "damageamount" );
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