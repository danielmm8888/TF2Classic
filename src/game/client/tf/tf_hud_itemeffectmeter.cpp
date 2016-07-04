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
#include "tf_controls.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

class CHudItemEffects;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudItemEffectMeter : public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudItemEffectMeter, EditablePanel );

public:
	CHudItemEffectMeter( Panel *pParent, const char *pElementName );

	virtual void	ApplySchemeSettings( IScheme *scheme );
	virtual void	UpdateStatus( void );

	int				GetSlot( void ) { return m_iSlot; }
	void			SetSlot( int iSlot ) { m_iSlot = iSlot; }
	void			SetWeapon( C_TFWeaponBase *pWeapon );

private:
	ContinuousProgressBar *m_pEffectMeter;
	CExLabel *m_pEffectMeterLabel;

	int m_iSlot;
	C_TFWeaponBase *m_pWeapon;
	float m_flOldCharge;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudItemEffectMeter::CHudItemEffectMeter( Panel *pParent, const char *pElementName ) : EditablePanel( pParent, pElementName )
{
	m_pEffectMeter = new ContinuousProgressBar( this, "ItemEffectMeter" );
	m_pEffectMeterLabel = new CExLabel( this, "ItemEffectMeterLabel", "" );
	m_iSlot = 0;
	m_flOldCharge = 1.0f;
	m_pWeapon = NULL;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudItemEffectMeter::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/HudItemEffectMeter.res" );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudItemEffectMeter::UpdateStatus( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer || !pPlayer->IsAlive() )
	{
		m_flOldCharge = 1.0f;
		return;
	}

	// Find a weapon in the loadout slot we're tied to.
	C_EconEntity *pEntity = pPlayer->GetEntityForLoadoutSlot( m_iSlot );
	if ( pEntity && pEntity->IsBaseCombatWeapon() )
	{
		if ( pEntity != m_pWeapon )
		{
			// Weapon changed, reset the label and progress.
			m_pWeapon = static_cast<C_TFWeaponBase *>( pEntity );
			m_flOldCharge = m_pWeapon->GetEffectBarProgress();

			if ( m_pEffectMeterLabel )
			{
				wchar_t *pszLocalized = g_pVGuiLocalize->Find( m_pWeapon->GetEffectLabelText() );
				if ( pszLocalized )
				{
					m_pEffectMeterLabel->SetText( pszLocalized );
				}
				else
				{
					m_pEffectMeterLabel->SetText( m_pWeapon->GetEffectLabelText() );
				}
			}
		}
	}
	else
	{
		m_pWeapon = NULL;
	}

	if ( !m_pWeapon || !m_pWeapon->HasChargeBar() )
	{
		m_flOldCharge = 1.0f;
		if ( IsVisible() )
			SetVisible( false );

		return;
	}

	if ( !IsVisible() )
		SetVisible( true );

	if ( m_pEffectMeter )
	{
		float flCharge = m_pWeapon->GetEffectBarProgress();
		m_pEffectMeter->SetProgress( flCharge );
		
		// Play a ding when full charged.
		if ( m_flOldCharge < 1.0f && flCharge == 1.0f && !m_pWeapon->IsWeapon( TF_WEAPON_INVIS ) )
		{
			CLocalPlayerFilter filter;
			C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, "TFPlayer.Recharged" );
		}

		m_flOldCharge = flCharge;
	}
}


class CHudItemEffects : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudItemEffects, EditablePanel );

public:
	CHudItemEffects( const char *pElementName );
	~CHudItemEffects();

	virtual void PerformLayout( void );
	virtual bool ShouldDraw( void );
	virtual void OnTick( void );

private:
	CUtlVector<CHudItemEffectMeter *> m_pEffectBars;
	
	CPanelAnimationVarAliasType( int, m_iXOffset, "x_offset", "50", "proportional_int" );
	CPanelAnimationVarAliasType( int, m_iYOffset, "y_offset", "0", "proportional_int" );
};

DECLARE_HUDELEMENT( CHudItemEffects );

CHudItemEffects::CHudItemEffects( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudItemEffects" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	// Create effect bars for primary, secondary and melee slots.
	for ( int i = 0; i < TF_PLAYER_WEAPON_COUNT; i++ )
	{
		CHudItemEffectMeter *pMeter = new CHudItemEffectMeter( this, "HudItemEffectMeter" );
		pMeter->SetSlot( i );
		m_pEffectBars.AddToTail( pMeter );
	}

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	vgui::ivgui()->AddTickSignal( GetVPanel() );
}

CHudItemEffects::~CHudItemEffects()
{
	m_pEffectBars.PurgeAndDeleteElements();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudItemEffects::ShouldDraw( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pPlayer || !pPlayer->IsAlive() )
		return false;

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: Sort meters by visiblity and loadout slot.
//-----------------------------------------------------------------------------
static int EffectBarsSort( CHudItemEffectMeter * const *pMeter1, CHudItemEffectMeter * const *pMeter2 )
{
	// Visible to the right.
	if ( !( *pMeter1 )->IsVisible() && ( *pMeter2 )->IsVisible() )
		return -1;

	if ( ( *pMeter1 )->IsVisible() && !( *pMeter2 )->IsVisible() )
		return 1;

	return ( ( *pMeter1 )->GetSlot() - ( *pMeter2 )->GetSlot() );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudItemEffects::PerformLayout( void )
{
	m_pEffectBars.Sort( EffectBarsSort );

	// Set panel offsets based on visibility.
	int count = m_pEffectBars.Count();
	for ( int i = 0; i < count; i++ )
	{
		m_pEffectBars[i]->SetPos( m_iXOffset * i, m_iYOffset * i );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudItemEffects::OnTick( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pPlayer )
		return;

	bool bUpdateLayout = false;
	for ( int i = 0; i < m_pEffectBars.Count(); i++ )
	{
		CHudItemEffectMeter *pMeter = m_pEffectBars[i];

		bool bWasVisible = pMeter->IsVisible();
		pMeter->UpdateStatus();
		bool bVisible = pMeter->IsVisible();

		if ( bVisible != bWasVisible )
		{
			bUpdateLayout = true;
		}
	}

	if ( bUpdateLayout )
	{
		InvalidateLayout( true );
	}
}
