//========= Copyright © 1996-2007, Valve Corporation, All rights reserved. ============//
//
// Purpose:
//
//=============================================================================//

#include "cbase.h"
#include "hudelement.h"
#include "iclientmode.h"
#include <vgui_controls/EditablePanel.h>
#include <vgui_controls/Label.h>

#include "tf_hud_condstatus.h"
#include "tf_hud_freezepanel.h"
#include "tf_hud_objectivestatus.h"
#include "tf_gamerules.h"

using namespace vgui;

DECLARE_HUDELEMENT(CTFHudCondStatus);

#define POWERUP_FIRST TF_COND_POWERUP_CRITDAMAGE
#define POWERUP_LAST  TF_COND_POWERUP_RAGEMODE
#define POWERUP_SIZE  ( POWERUP_LAST - POWERUP_FIRST + 1 )

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFHudCondStatus::CTFHudCondStatus( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudCondStatus" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	for ( int i = POWERUP_FIRST; i <= POWERUP_LAST; i++ )
	{
		m_pPowerups.AddToTail( new EditablePanel( this, "PowerupPanel" ) );
		powerupinfo_t powerup( i, 0.0f, 0.0f );
		powerups.AddToTail( powerup );
	}

	vgui::ivgui()->AddTickSignal( GetVPanel(), 100 );
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFHudCondStatus::~CTFHudCondStatus()
{
	m_pPowerups.RemoveAll();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CTFHudCondStatus::ShouldDraw( void )
{
	if( IsTakingAFreezecamScreenshot() )
		return false;

	if ( !TFGameRules() || !TFGameRules()->IsDeathmatch() )
		return false;

	C_TFPlayer *pLocalTFPlayer = C_TFPlayer::GetLocalTFPlayer();
	if ( !pLocalTFPlayer )
		return false;

	for ( int i = POWERUP_FIRST; i <= POWERUP_LAST; i++ )
	{
		if ( pLocalTFPlayer->m_Shared.InCond( i ) )
		{
			UpdateStatus();
			return true;
		}
	}

	return false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudCondStatus::ApplySchemeSettings( IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	// load control settings...
	LoadControlSettings( "resource/UI/HudCondStatus.res" );
}

void CTFHudCondStatus::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

	for ( KeyValues *pData = inResourceData->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey() )
	{
		if ( !Q_stricmp( pData->GetName(), "PowerupPanel" ) )
		{
			for ( int i = 0; i < POWERUP_SIZE; i++ )
			{
				m_pPowerups[i]->ApplySettings( pData );
			}
		}
	}

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudCondStatus::PerformLayout()
{
	BaseClass::PerformLayout();

	int wide = (float)GetWide() / (float)POWERUP_SIZE;
	int tall = GetTall();
	for ( int i = 0; i < POWERUP_SIZE; i++ )
	{
		m_pPowerups[i]->SetBounds( i * wide, 0, wide, tall );
	}
}

int DurationSort( const powerupinfo_t *a, const powerupinfo_t *b )
{
	return ( a->fDuration < b->fDuration );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudCondStatus::UpdateStatus( void )
{
	C_TFPlayer *pLocalTFPlayer = C_TFPlayer::GetLocalTFPlayer();

	for ( int i = POWERUP_FIRST; i <= POWERUP_LAST; i++ )
	{
		for ( int j = 0; j < POWERUP_SIZE; j++ )
		{
			if ( powerups[j].ID == i )
			{
				powerups[j].fDuration = pLocalTFPlayer->m_Shared.GetConditionDuration(i);
				if ( powerups[j].fDuration > powerups[j].fInitDuration )
				{
					powerups[j].fInitDuration = pLocalTFPlayer->m_Shared.GetConditionDuration(i);
				}
			}
		}
	}
	powerups.Sort( DurationSort );

	for ( int i = 0; i < POWERUP_SIZE; i++ )
	{
		EditablePanel *m_pPowerupPanel = m_pPowerups[i];
		if ( powerups[i].fDuration == 0.0 )
		{ 
			m_pPowerupPanel->SetVisible( false );
			continue;
		}

		m_pPowerupPanel->SetVisible( true );

		int ID = powerups[i].ID - POWERUP_FIRST;
		float fDuration = powerups[i].fDuration;
		float fInitDuration = powerups[i].fInitDuration;

		CTFProgressBar *m_pProgressBar = dynamic_cast<CTFProgressBar *>( m_pPowerupPanel->FindChildByName( "TimePanelProgressBar" ) );
		if ( m_pProgressBar )
		{
			char szIcon[64];
			Q_snprintf( szIcon, sizeof( szIcon ), "hud/powerup_%i", ID );
			m_pProgressBar->SetIcon( szIcon );
			m_pProgressBar->SetPercentage( (fInitDuration - fDuration) / fInitDuration );
		}
	}
}