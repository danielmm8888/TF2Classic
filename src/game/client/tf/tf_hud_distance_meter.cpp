//=====================================================================================//
//
// Purpose: 
//
//=====================================================================================//

#include "cbase.h"
#include "view.h"
#include <vgui/IVGui.h>
#include "VGuiMatSurface/IMatSystemSurface.h"
#include <vgui_controls/Controls.h>
#include <vgui/IScheme.h>
#include <vgui_controls/Panel.h>
#include "hudelement.h"
#include "iclientmode.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

static ConVar cl_showdist( "cl_showdist", "0", FCVAR_CHEAT );

//-----------------------------------------------------------------------------
// Purpose: Distance meter panel.
//-----------------------------------------------------------------------------
class CDistanceMeter : public CHudElement, public vgui::Panel
{
	DECLARE_CLASS_SIMPLE( CDistanceMeter, vgui::Panel );

public:
	CDistanceMeter( const char *pElementName );
	~CDistanceMeter( void );

	virtual void	ApplySchemeSettings( vgui::IScheme *pScheme );
	virtual void	Paint();

	virtual bool	ShouldDraw( void );

	vgui::HFont		m_hFont;
};

DECLARE_HUDELEMENT( CDistanceMeter );

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : *parent - 
//-----------------------------------------------------------------------------
CDistanceMeter::CDistanceMeter( const char *pElementName ) : CHudElement( pElementName ), BaseClass( NULL, "HudDistanceMeter" )
{
	Panel *pParent = g_pClientMode->GetViewport();
	SetParent( pParent );

	m_hFont = 0;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CDistanceMeter::~CDistanceMeter( void )
{
}

void CDistanceMeter::ApplySchemeSettings( vgui::IScheme *pScheme )
{
	BaseClass::ApplySchemeSettings( pScheme );

	m_hFont = pScheme->GetFont( "Default" );

	SetPaintBackgroundEnabled( false );
	SetSize( ScreenWidth(), ScreenHeight() );
}

//-----------------------------------------------------------------------------
// Purpose: 
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CDistanceMeter::ShouldDraw( void )
{
	if ( cl_showdist.GetBool() )
		return true;

	return false;
}

void CDistanceMeter::Paint( void )
{
	int nShowDist = cl_showdist.GetInt();

	if ( !nShowDist )
		return;

	BaseClass::Paint();

	int vx, vy, vw, vh;
	vgui::surface()->GetFullscreenViewport( vx, vy, vw, vh );

	// Draw it to the left and above the center of the screen.
	int x = ( vw / 2 ) + 20;
	int y = ( vh / 2 ) - 20;

	// Calculate the distance from player's eyes to his aiming point.
	Vector vecSrc, vecDir, vecEnd;

	vecSrc = MainViewOrigin();
	vecDir = MainViewForward();

	C_BasePlayer *pPlayer = C_BasePlayer::GetLocalPlayer();

	if ( nShowDist == 2 )
	{
		if ( pPlayer )
		{
			pPlayer->EyePositionAndVectors( &vecSrc, &vecDir, NULL, NULL );
		}
	}

	vecEnd = vecSrc + vecDir * MAX_TRACE_LENGTH;

	trace_t tr;
	UTIL_TraceLine( vecSrc, vecEnd, MASK_SOLID, pPlayer, COLLISION_GROUP_NONE, &tr );
	vecDir = tr.endpos - vecSrc;

	g_pMatSystemSurface->DrawColoredText( m_hFont, x, y, 255, 255, 255, 255, "%.02f", vecDir.Length() );
}
