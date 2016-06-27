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
#include "tf_weaponbase.h"
#include "engine/IEngineSound.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CHudDemomanChargeMeter : public CHudElement, public EditablePanel
{
	DECLARE_CLASS_SIMPLE( CHudDemomanChargeMeter, EditablePanel );

public:
	CHudDemomanChargeMeter( const char *pElementName );

	virtual void	ApplySchemeSettings( IScheme *scheme );
	virtual bool	ShouldDraw( void );
	virtual void	OnTick( void );

private:
	vgui::ContinuousProgressBar *m_pChargeMeter;

	bool m_bCharging;
	int m_iChargupSound;
};

DECLARE_HUDELEMENT( CHudDemomanChargeMeter );

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CHudDemomanChargeMeter::CHudDemomanChargeMeter( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudDemomanCharge" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_pChargeMeter = new ContinuousProgressBar( this, "ChargeMeter" );

	SetHiddenBits( HIDEHUD_MISCSTATUS );

	vgui::ivgui()->AddTickSignal( GetVPanel() );

	m_bCharging = false;
	m_iChargupSound = -1;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudDemomanChargeMeter::ApplySchemeSettings( IScheme *pScheme )
{
	// load control settings...
	LoadControlSettings( "resource/UI/HudDemomanCharge.res" );

	BaseClass::ApplySchemeSettings( pScheme );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CHudDemomanChargeMeter::ShouldDraw( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer || !pPlayer->IsAlive() )
	{
		return false;
	}

	CTFWeaponBase *pWpn = pPlayer->GetActiveTFWeapon();

	if ( !pWpn )
	{
		return false;
	}

	if ( !pWpn->IsWeapon( TF_WEAPON_PIPEBOMBLAUNCHER ) &&
		!pWpn->IsWeapon( TF_WEAPON_GRENADE_MIRV ) &&
		!pWpn->IsWeapon( TF_WEAPON_COMPOUND_BOW ) )
	{
		return false;
	}

	return CHudElement::ShouldDraw();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CHudDemomanChargeMeter::OnTick( void )
{
	C_TFPlayer *pPlayer = C_TFPlayer::GetLocalTFPlayer();

	if ( !pPlayer )
		return;

	CTFWeaponBase *pWpn = pPlayer->GetActiveTFWeapon();
	ITFChargeUpWeapon *pChargeupWeapon = dynamic_cast< ITFChargeUpWeapon *>( pWpn );

	if ( !pWpn || !pChargeupWeapon )
	{
		if ( m_bCharging )
		{
			if ( m_iChargupSound != -1 )
			{
				enginesound->StopSoundByGuid( m_iChargupSound );
				m_iChargupSound = -1;
			}

			m_bCharging = false;
		}
		return;
	}

	if ( m_pChargeMeter )
	{
		float flChargeMaxTime = pChargeupWeapon->GetChargeMaxTime();

		if ( flChargeMaxTime != 0 )
		{
			float flChargeBeginTime = pChargeupWeapon->GetChargeBeginTime();

			if ( flChargeBeginTime > 0 )
			{
				float flTimeCharged = max( 0, gpGlobals->curtime - flChargeBeginTime );
				float flPercentCharged = min( 1.0, flTimeCharged / flChargeMaxTime );

				m_pChargeMeter->SetProgress( flPercentCharged );

				if ( !m_bCharging )
				{
					if ( pChargeupWeapon->GetChargeSound() != NULL )
					{
						CLocalPlayerFilter filter;
						C_BaseEntity::EmitSound( filter, pPlayer->entindex(), pChargeupWeapon->GetChargeSound() );
						m_iChargupSound = enginesound->GetGuidForLastSoundEmitted();
					}

					m_bCharging = true;
				}
			}
			else
			{
				m_pChargeMeter->SetProgress( 0.0f );

				if ( m_bCharging )
				{
					if ( m_iChargupSound != -1 )
					{
						enginesound->StopSoundByGuid( m_iChargupSound );
						m_iChargupSound = -1;
					}

					m_bCharging = false;
				}
			}
		}
	}
}