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

} account_delta_t;

#define NUM_ACCOUNT_DELTA_ITEMS 10

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CAccountPanel : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CAccountPanel, EditablePanel );

public:
	CAccountPanel( const char *pElementName );

	virtual void	ApplySchemeSettings( IScheme *scheme );
	virtual void	LevelInit( void );

	virtual void	Paint( void );

	void OnAccountValueChanged( int iOldValue, int iNewValue );

	virtual const char *GetResFilename( void ) { return "resource/UI/HudAccountPanel.res"; }

private:

	int iAccountDeltaHead;
	account_delta_t m_AccountDeltaItems[NUM_ACCOUNT_DELTA_ITEMS];

	CPanelAnimationVarAliasType( float, m_flDeltaItemStartPos, "delta_item_start_y", "100", "proportional_float" );
	CPanelAnimationVarAliasType( float, m_flDeltaItemEndPos, "delta_item_end_y", "0", "proportional_float" );

	CPanelAnimationVarAliasType( float, m_flDeltaItemX, "delta_item_x", "0", "proportional_float" );

	CPanelAnimationVar( Color, m_DeltaPositiveColor, "PositiveColor", "0 255 0 255" );
	CPanelAnimationVar( Color, m_DeltaNegativeColor, "NegativeColor", "255 0 0 255" );

	CPanelAnimationVar( float, m_flDeltaLifetime, "delta_lifetime", "2.0" );

	CPanelAnimationVar( vgui::HFont, m_hDeltaItemFont, "delta_item_font", "Default" );
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CAccountPanel::CAccountPanel( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, pElementName )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	iAccountDeltaHead = 0;

	SetDialogVariable( "metal", 0 );

	for ( int i = 0; i < NUM_ACCOUNT_DELTA_ITEMS; i++ )
	{
		m_AccountDeltaItems[i].m_flDieTime = 0.0f;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAccountPanel::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( GetResFilename() );

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: called whenever a new level's starting
//-----------------------------------------------------------------------------
void CAccountPanel::LevelInit( void )
{
	iAccountDeltaHead = 0;

	for ( int i = 0; i < NUM_ACCOUNT_DELTA_ITEMS; i++ )
	{
		m_AccountDeltaItems[i].m_flDieTime = 0.0f;
	}

	CHudElement::LevelInit();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CAccountPanel::OnAccountValueChanged( int iOldValue, int iNewValue )
{
	// update the account value
	SetDialogVariable( "metal", iNewValue );

	int iDelta = iNewValue - iOldValue;

	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( iDelta != 0 && pPlayer && pPlayer->IsAlive() )
	{
		// create a delta item that floats off the top
		account_delta_t *pNewDeltaItem = &m_AccountDeltaItems[iAccountDeltaHead];

		iAccountDeltaHead++;
		iAccountDeltaHead %= NUM_ACCOUNT_DELTA_ITEMS;

		pNewDeltaItem->m_flDieTime = gpGlobals->curtime + m_flDeltaLifetime;
		pNewDeltaItem->m_iAmount = iDelta;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Paint the deltas
//-----------------------------------------------------------------------------
void CAccountPanel::Paint( void )
{
	BaseClass::Paint();

	for ( int i = 0; i < NUM_ACCOUNT_DELTA_ITEMS; i++ )
	{
		// update all the valid delta items
		if ( m_AccountDeltaItems[i].m_flDieTime > gpGlobals->curtime )
		{
			// position and alpha are determined from the lifetime
			// color is determined by the delta - green for positive, red for negative

			Color c = ( m_AccountDeltaItems[i].m_iAmount > 0 ) ? m_DeltaPositiveColor : m_DeltaNegativeColor;

			float flLifetimePercent = ( m_AccountDeltaItems[i].m_flDieTime - gpGlobals->curtime ) / m_flDeltaLifetime;

			// fade out after half our lifetime
			if ( flLifetimePercent < 0.5 )
			{
				c[3] = (int)( 255.0f * ( flLifetimePercent / 0.5 ) );
			}

			float flHeight = ( m_flDeltaItemStartPos - m_flDeltaItemEndPos );
			float flYPos = m_flDeltaItemEndPos + flLifetimePercent * flHeight;

			vgui::surface()->DrawSetTextFont( m_hDeltaItemFont );
			vgui::surface()->DrawSetTextColor( c );
			vgui::surface()->DrawSetTextPos( m_flDeltaItemX, (int)flYPos );

			wchar_t wBuf[20];

			if ( m_AccountDeltaItems[i].m_iAmount > 0 )
			{
				_snwprintf( wBuf, sizeof( wBuf ) / sizeof( wchar_t ), L"+%d", m_AccountDeltaItems[i].m_iAmount );
			}
			else
			{
				_snwprintf( wBuf, sizeof( wBuf ) / sizeof( wchar_t ), L"%d", m_AccountDeltaItems[i].m_iAmount );
			}

			vgui::surface()->DrawPrintText( wBuf, wcslen( wBuf ), FONT_DRAW_NONADDITIVE );
		}
	}
}


//-----------------------------------------------------------------------------
// Purpose: Metal account
//-----------------------------------------------------------------------------
class CHudAccountPanel : public CAccountPanel
{
public:
	CHudAccountPanel( const char *pElementName );

	virtual bool	ShouldDraw( void );
	virtual void	FireGameEvent( IGameEvent *event );
};

DECLARE_HUDELEMENT( CHudAccountPanel );

CHudAccountPanel::CHudAccountPanel( const char *pElementName ) : CAccountPanel( pElementName )
{
	ListenForGameEvent( "player_account_changed" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudAccountPanel::ShouldDraw( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer || !pPlayer->IsAlive() || !pPlayer->IsPlayerClass( TF_CLASS_ENGINEER ) )
	{
		return false;
	}

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudAccountPanel::FireGameEvent( IGameEvent *event )
{
	const char *type = event->GetName();

	if ( V_strcmp( type, "player_account_changed" ) == 0 )
	{
		int iOldValue = event->GetInt( "old_account" );
		int iNewValue = event->GetInt( "new_account" );
		OnAccountValueChanged( iOldValue, iNewValue );
	}
	else
	{
		CHudElement::FireGameEvent( event );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Health account
//-----------------------------------------------------------------------------
class CHealthAccountPanel : public CAccountPanel
{
public:
	CHealthAccountPanel( const char *pElementName );

	virtual bool	ShouldDraw( void );
	virtual void	FireGameEvent( IGameEvent *event );

	virtual const char *GetResFilename( void ) { return "resource/UI/HudHealthAccount.res"; }
};

DECLARE_HUDELEMENT( CHealthAccountPanel );

CHealthAccountPanel::CHealthAccountPanel( const char *pElementName ) : CAccountPanel( pElementName )
{
	ListenForGameEvent( "player_healonhit" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHealthAccountPanel::ShouldDraw( void )
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
void CHealthAccountPanel::FireGameEvent( IGameEvent *event )
{
	const char *type = event->GetName();

	if ( V_strcmp( type, "player_healonhit" ) == 0 )
	{
		int iAmount = event->GetInt( "amount" );
		int iPlayer = event->GetInt( "entindex" );
		C_TFPlayer *pPlayer = ToTFPlayer( UTIL_PlayerByIndex( iPlayer ) );

		if ( !pPlayer )
			return;

		if ( pPlayer->IsLocalPlayer() )
		{
			// If this is a local player show the number in HUD.
			OnAccountValueChanged( 0, iAmount );
		}
		else
		{
			// Show a particle to indicate that player got healed.
			int iTeam = pPlayer->GetTeamNumber();

			if ( pPlayer->IsEnemyPlayer() )
			{
				// Don't give away cloaked spies.
				if ( pPlayer->m_Shared.IsStealthed() )
					return;

				// Show their disguise team color.
				if ( pPlayer->m_Shared.InCond( TF_COND_DISGUISED ) )
					iTeam = pPlayer->m_Shared.GetDisguiseTeam();
			}

			const char *pszFormat = NULL;
			if ( iAmount < 0 )
			{
				pszFormat = "healthlost_%s";
			}
			else
			{
				pszFormat = iAmount < 100 ? "healthgained_%s" : "healthgained_%s_large";
			}

			const char *pszParticle = ConstructTeamParticle( pszFormat, iTeam, false, g_aTeamNamesShort );

			pPlayer->ParticleProp()->Create( pszParticle, PATTACH_POINT, "head" );
		}
	}
	else
	{
		CHudElement::FireGameEvent( event );
	}
}
