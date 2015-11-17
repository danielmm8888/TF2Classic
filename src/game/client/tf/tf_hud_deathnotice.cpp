//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: Draws CSPort's death notices
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "hudelement.h"
#include "hud_macros.h"
#include "c_playerresource.h"
#include "iclientmode.h"
#include <vgui_controls/Controls.h>
#include <vgui_controls/Panel.h>
#include <vgui/ISurface.h>
#include <vgui/ILocalize.h>
#include <KeyValues.h>
#include "c_baseplayer.h"
#include "c_team.h"

#include "hud_basedeathnotice.h"

#include "tf_shareddefs.h"
#include "clientmode_tf.h"
#include "c_tf_player.h"
#include "c_tf_playerresource.h"
#include "tf_hud_freezepanel.h"
#include "engine/IEngineSound.h"
#include "tf_gamerules.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

// Must match resource/tf_objects.txt!!!
const char *szLocalizedObjectNames[OBJ_LAST] =
{
	"#TF_Object_Dispenser",
	"#TF_Object_Tele",
	"#TF_Object_Sentry",
	"#TF_object_sapper"			
};

class CTFHudDeathNotice : public CHudBaseDeathNotice
{
	DECLARE_CLASS_SIMPLE( CTFHudDeathNotice, CHudBaseDeathNotice );
public:
	CTFHudDeathNotice( const char *pElementName ) : CHudBaseDeathNotice( pElementName ) {};
	virtual void ApplySchemeSettings( vgui::IScheme *scheme );
	virtual bool IsVisible( void );
	virtual void Paint( void );

	void PlayRivalrySounds( int iKillerIndex, int iVictimIndex, int iType  );
	virtual Color GetInfoTextColor(int iDeathNoticeMsg, bool bLocalPlayerInvolved){ return bLocalPlayerInvolved ? Color(0, 0, 0, 255) : Color(255, 255, 255, 255); }

protected:	
	virtual void OnGameEvent( IGameEvent *event, int iDeathNoticeMsg );
	virtual Color GetTeamColor( int iTeamNumber, bool bLocalPlayerInvolved /* = false */ );

private:
	void AddAdditionalMsg( int iKillerID, int iVictimID, const char *pMsgKey );

	CHudTexture		*m_iconDomination;

	CPanelAnimationVar( Color, m_clrBlueText, "TeamBlue", "153 204 255 255" );
	CPanelAnimationVar( Color, m_clrRedText, "TeamRed", "255 64 64 255" );
	CPanelAnimationVar( Color, m_clrGreenText, "TeamGreen", "8 174 0 255" );
	CPanelAnimationVar( Color, m_clrYellowText, "TeamYellow", "255 160 0 255" );

};

DECLARE_HUDELEMENT( CTFHudDeathNotice );

void CTFHudDeathNotice::ApplySchemeSettings( vgui::IScheme *scheme )
{
	BaseClass::ApplySchemeSettings( scheme );

	m_iconDomination = gHUD.GetIcon( "leaderboard_dominated" );
}

bool CTFHudDeathNotice::IsVisible( void )
{
	if ( IsTakingAFreezecamScreenshot() )
		return false;

	return BaseClass::IsVisible();
}

void CTFHudDeathNotice::PlayRivalrySounds( int iKillerIndex, int iVictimIndex, int iType )
{
	int iLocalPlayerIndex = GetLocalPlayerIndex();

	//We're not involved in this kill
	if ( iKillerIndex != iLocalPlayerIndex && iVictimIndex != iLocalPlayerIndex )
		return;

	// Stop any sounds that are already playing to avoid ear rape in case of
	// multiple dominations at once.
	C_BaseEntity::StopSound( SOUND_FROM_LOCAL_PLAYER, "Game.Domination" );
	C_BaseEntity::StopSound( SOUND_FROM_LOCAL_PLAYER, "Game.Nemesis" );
	C_BaseEntity::StopSound( SOUND_FROM_LOCAL_PLAYER, "Game.Revenge" );

	const char *pszSoundName = NULL;

	if ( iType == TF_DEATH_DOMINATION )
	{
		if ( iKillerIndex == iLocalPlayerIndex )
		{
			pszSoundName = "Game.Domination";
		}
		else if ( iVictimIndex == iLocalPlayerIndex )
		{
			pszSoundName = "Game.Nemesis";
		}
	}
	else if ( iType == TF_DEATH_REVENGE )
	{
		pszSoundName = "Game.Revenge";
	}

	CLocalPlayerFilter filter;
	C_BaseEntity::EmitSound( filter, SOUND_FROM_LOCAL_PLAYER, pszSoundName );
}

//-----------------------------------------------------------------------------
// Purpose: Called when a game event happens and a death notice is about to be 
//			displayed.  This method can examine the event and death notice and
//			make game-specific tweaks to it before it is displayed
//-----------------------------------------------------------------------------
void CTFHudDeathNotice::OnGameEvent(IGameEvent *event, int iDeathNoticeMsg)
{
	const char *pszEventName = event->GetName();

	if ( FStrEq( pszEventName, "player_death" ) || FStrEq( pszEventName, "object_destroyed" ) )
	{
		bool bIsObjectDestroyed = FStrEq( pszEventName, "object_destroyed" );
		int iCustomDamage = event->GetInt( "customkill" );
		int iLocalPlayerIndex = GetLocalPlayerIndex();

		m_DeathNotices[iDeathNoticeMsg].Killer.iPlayerID = engine->GetPlayerForUserID(event->GetInt("attacker"));
		m_DeathNotices[iDeathNoticeMsg].Victim.iPlayerID = engine->GetPlayerForUserID(event->GetInt("userid"));

		// if there was an assister, put both the killer's and assister's names in the death message
		int iAssisterID = engine->GetPlayerForUserID( event->GetInt( "assister" ) );
		m_DeathNotices[iDeathNoticeMsg].Assister.iPlayerID = iAssisterID;
		const char *assister_name = ( iAssisterID > 0 ? g_PR->GetPlayerName( iAssisterID ) : NULL );
		if ( assister_name )
		{
			// Base TF2 assumes that the assister and killer are the same team, thus it 
			// writes both of the same string, which in turn gives them both the killers team color
			// whether or not the assister is on the killers team or not. -danielmm8888
			m_DeathNotices[iDeathNoticeMsg].Assister.iTeam = (iAssisterID > 0) ? g_PR->GetTeam(iAssisterID) : 0;
			char szKillerBuf[MAX_PLAYER_NAME_LENGTH];
			Q_snprintf(szKillerBuf, ARRAYSIZE(szKillerBuf), "%s", assister_name);
			Q_strncpy(m_DeathNotices[iDeathNoticeMsg].Assister.szName, szKillerBuf, ARRAYSIZE(m_DeathNotices[iDeathNoticeMsg].Assister.szName));
			if (iLocalPlayerIndex == iAssisterID)
			{
				m_DeathNotices[iDeathNoticeMsg].bLocalPlayerInvolved = true;
			}

			// This is the old code used for assister handling
			/*
			char szKillerBuf[MAX_PLAYER_NAME_LENGTH*2];
			Q_snprintf(szKillerBuf, ARRAYSIZE(szKillerBuf), "%s + %s", m_DeathNotices[iDeathNoticeMsg].Killer.szName, assister_name);
			Q_strncpy(m_DeathNotices[iDeathNoticeMsg].Killer.szName, szKillerBuf, ARRAYSIZE(m_DeathNotices[iDeathNoticeMsg].Killer.szName));
			if ( iLocalPlayerIndex == iAssisterID )
			{
				m_DeathNotices[iDeathNoticeMsg].bLocalPlayerInvolved = true;
			}*/
		}

		if ( !bIsObjectDestroyed )
		{
			// if this death involved a player dominating another player or getting revenge on another player, add an additional message
			// mentioning that
			int iKillerID = engine->GetPlayerForUserID( event->GetInt( "attacker" ) );
			int iVictimID = engine->GetPlayerForUserID( event->GetInt( "userid" ) );
			int nDeathFlags = event->GetInt( "death_flags" );
		
			if ( nDeathFlags & TF_DEATH_DOMINATION )
			{
				AddAdditionalMsg( iKillerID, iVictimID, "#Msg_Dominating" );
				PlayRivalrySounds( iKillerID, iVictimID, TF_DEATH_DOMINATION );
			}
			if ( ( nDeathFlags & TF_DEATH_ASSISTER_DOMINATION ) && ( iAssisterID > 0 ) )
			{
				AddAdditionalMsg( iAssisterID, iVictimID, "#Msg_Dominating" );
				PlayRivalrySounds( iAssisterID, iVictimID, TF_DEATH_DOMINATION );
			}
			if ( nDeathFlags & TF_DEATH_REVENGE )
			{
				AddAdditionalMsg( iKillerID, iVictimID, "#Msg_Revenge" );
				PlayRivalrySounds( iKillerID, iVictimID, TF_DEATH_REVENGE );
			}
			if ( ( nDeathFlags & TF_DEATH_ASSISTER_REVENGE ) && ( iAssisterID > 0 ) )
			{
				AddAdditionalMsg( iAssisterID, iVictimID, "#Msg_Revenge" );
				PlayRivalrySounds( iAssisterID, iVictimID, TF_DEATH_REVENGE );
			}
		}
		else
		{
			// if this is an object destroyed message, set the victim name to "<object type> (<owner>)"
			int iObjectType = event->GetInt( "objecttype" );
			if ( iObjectType >= 0 && iObjectType < OBJ_LAST )
			{
				// get the localized name for the object
				char szLocalizedObjectName[MAX_PLAYER_NAME_LENGTH];
				szLocalizedObjectName[ 0 ] = 0;
				const wchar_t *wszLocalizedObjectName = g_pVGuiLocalize->Find( szLocalizedObjectNames[iObjectType] );
				if ( wszLocalizedObjectName )
				{
					g_pVGuiLocalize->ConvertUnicodeToANSI( wszLocalizedObjectName, szLocalizedObjectName, ARRAYSIZE( szLocalizedObjectName ) );
				}
				else
				{
					Warning( "Couldn't find localized object name for '%s'\n", szLocalizedObjectNames[iObjectType] );
					Q_strncpy( szLocalizedObjectName, szLocalizedObjectNames[iObjectType], sizeof( szLocalizedObjectName ) );
				}

				// compose the string
				if (m_DeathNotices[iDeathNoticeMsg].Victim.szName[0])
				{
					char szVictimBuf[MAX_PLAYER_NAME_LENGTH*2];
					Q_snprintf(szVictimBuf, ARRAYSIZE(szVictimBuf), "%s (%s)", szLocalizedObjectName, m_DeathNotices[iDeathNoticeMsg].Victim.szName);
					Q_strncpy(m_DeathNotices[iDeathNoticeMsg].Victim.szName, szVictimBuf, ARRAYSIZE(m_DeathNotices[iDeathNoticeMsg].Victim.szName));
				}
				else
				{
					Q_strncpy(m_DeathNotices[iDeathNoticeMsg].Victim.szName, szLocalizedObjectName, ARRAYSIZE(m_DeathNotices[iDeathNoticeMsg].Victim.szName));
				}
				
			}
			else
			{
				Assert( false ); // invalid object type
			}
		}

		const wchar_t *pMsg = NULL;
		switch ( iCustomDamage )
		{
		case TF_DMG_CUSTOM_BACKSTAB:
			Q_strncpy(m_DeathNotices[iDeathNoticeMsg].szIcon, "d_backstab", ARRAYSIZE(m_DeathNotices[iDeathNoticeMsg].szIcon));
			break;
		case TF_DMG_CUSTOM_HEADSHOT:
			Q_strncpy(m_DeathNotices[iDeathNoticeMsg].szIcon, "d_headshot", ARRAYSIZE(m_DeathNotices[iDeathNoticeMsg].szIcon));
			break;
		case TF_DMG_CUSTOM_SUICIDE:
			{
				// display a different message if this was suicide, or assisted suicide (suicide w/recent damage, kill awarded to damager)
				bool bAssistedSuicide = event->GetInt( "userid" ) != event->GetInt( "attacker" );
				pMsg = g_pVGuiLocalize->Find( bAssistedSuicide ? "#DeathMsg_AssistedSuicide" : "#DeathMsg_Suicide" );
				if ( pMsg )
				{
					V_wcsncpy(m_DeathNotices[iDeathNoticeMsg].wzInfoText, pMsg, sizeof(m_DeathNotices[iDeathNoticeMsg].wzInfoText));
				}			
				break;
			}
		default:
			break;
		}
	} 
	else if ( FStrEq( "teamplay_point_captured", pszEventName ) || FStrEq( "teamplay_capture_blocked", pszEventName ) || 
		FStrEq( "teamplay_flag_event", pszEventName ) )
	{
		bool bDefense = ( FStrEq( "teamplay_capture_blocked", pszEventName ) || ( FStrEq( "teamplay_flag_event", pszEventName ) &&
			TF_FLAGEVENT_DEFEND == event->GetInt( "eventtype" ) ) );

		const char *szCaptureIcons[] = { "d_redcapture", "d_bluecapture", "d_greencapture", "d_yellowcapture" };
		const char *szDefenseIcons[] = { "d_reddefend", "d_bluedefend", "d_greendefend", "d_yellowdefend" };
		
		int iTeam = m_DeathNotices[iDeathNoticeMsg].Killer.iTeam;
		Assert( iTeam >= FIRST_GAME_TEAM );
		Assert( iTeam < FIRST_GAME_TEAM + TF_TEAM_COUNT );
		if ( iTeam < FIRST_GAME_TEAM || iTeam >= FIRST_GAME_TEAM + TF_TEAM_COUNT )
			return;

		int iIndex = m_DeathNotices[iDeathNoticeMsg].Killer.iTeam - FIRST_GAME_TEAM;
		Assert( iIndex < ARRAYSIZE( szCaptureIcons ) );

		Q_strncpy(m_DeathNotices[iDeathNoticeMsg].szIcon, bDefense ? szDefenseIcons[iIndex] : szCaptureIcons[iIndex], ARRAYSIZE(m_DeathNotices[iDeathNoticeMsg].szIcon));
	}
}

//-----------------------------------------------------------------------------
// Purpose: Adds an additional death message
//-----------------------------------------------------------------------------
void CTFHudDeathNotice::AddAdditionalMsg( int iKillerID, int iVictimID, const char *pMsgKey )
{
	DeathNoticeItem &msg2 = m_DeathNotices[AddDeathNoticeItem()];
	Q_strncpy( msg2.Killer.szName, g_PR->GetPlayerName( iKillerID ), ARRAYSIZE( msg2.Killer.szName ) );
	Q_strncpy( msg2.Victim.szName, g_PR->GetPlayerName( iVictimID ), ARRAYSIZE( msg2.Victim.szName ) );
	
	msg2.Killer.iTeam = g_PR->GetTeam(iKillerID);
	msg2.Victim.iTeam = g_PR->GetTeam(iVictimID);

	msg2.Killer.iPlayerID = iKillerID;
	msg2.Victim.iPlayerID = iVictimID;

	const wchar_t *wzMsg =  g_pVGuiLocalize->Find( pMsgKey );
	if ( wzMsg )
	{
		V_wcsncpy( msg2.wzInfoText, wzMsg, sizeof( msg2.wzInfoText ) );
	}
	msg2.iconDeath = m_iconDomination;
	int iLocalPlayerIndex = GetLocalPlayerIndex();
	if ( iLocalPlayerIndex == iVictimID || iLocalPlayerIndex == iKillerID )
	{
		msg2.bLocalPlayerInvolved = true;
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFHudDeathNotice::Paint()
{
	// Retire any death notices that have expired
	RetireExpiredDeathNotices();

	CBaseViewport *pViewport = dynamic_cast<CBaseViewport *>( GetClientModeNormal()->GetViewport() );
	int yStart = pViewport->GetDeathMessageStartHeight();

	surface()->DrawSetTextFont( m_hTextFont );

	int xMargin = XRES( 10 );
	int xSpacing = UTIL_ComputeStringWidth( m_hTextFont, L" " );

	int iCount = m_DeathNotices.Count();
	for ( int i = 0; i < iCount; i++ )
	{
		DeathNoticeItem &msg = m_DeathNotices[i];
		
		CHudTexture *icon = msg.iconDeath;
		CHudTexture *iconPostKillerName = msg.iconPostKillerName;
		CHudTexture *iconPreKillerName = msg.iconPreKillerName;
		CHudTexture *iconPostVictimName = msg.iconPostVictimName;

		wchar_t victim[256] = L"";
		wchar_t killer[256] = L"";
		wchar_t assister[256] = L"";

		// TEMP - print the death icon name if we don't have a material for it

		g_pVGuiLocalize->ConvertANSIToUnicode( msg.Victim.szName, victim, sizeof( victim ) );
		g_pVGuiLocalize->ConvertANSIToUnicode( msg.Killer.szName, killer, sizeof( killer ) );
		g_pVGuiLocalize->ConvertANSIToUnicode( msg.Assister.szName, assister, sizeof(assister) );

		int iVictimTextWide = UTIL_ComputeStringWidth( m_hTextFont, victim ) + xSpacing;
		int iDeathInfoTextWide= msg.wzInfoText[0] ? UTIL_ComputeStringWidth( m_hTextFont, msg.wzInfoText ) + xSpacing : 0;
		int iDeathInfoEndTextWide= msg.wzInfoTextEnd[0] ? UTIL_ComputeStringWidth( m_hTextFont, msg.wzInfoTextEnd ) + xSpacing : 0;

		int iKillerTextWide = killer[0] ? UTIL_ComputeStringWidth( m_hTextFont, killer ) + xSpacing : 0;
		int iLineTall = m_flLineHeight;
		int iTextTall = surface()->GetFontTall( m_hTextFont );
		int iconWide = 0, iconTall = 0, iDeathInfoOffset = 0, iVictimTextOffset = 0, iconActualWide = 0;

		int iPreKillerTextWide = msg.wzPreKillerText[0] ? UTIL_ComputeStringWidth( m_hTextFont, msg.wzPreKillerText ) - xSpacing : 0;
		
		int iconPrekillerWide = 0, iconPrekillerActualWide = 0, iconPrekillerTall = 0;
		int iconPostkillerWide = 0, iconPostkillerActualWide = 0, iconPostkillerTall = 0;

		int iconPostVictimWide = 0, iconPostVictimActualWide = 0, iconPostVictimTall = 0;

		int iAssisterTextWide = assister[0] ? UTIL_ComputeStringWidth(m_hTextFont, assister) + xSpacing : 0;

		int iPlusIconWide = assister[0] ? UTIL_ComputeStringWidth(m_hTextFont, "+") + xSpacing : 0;

		// Get the local position for this notice
		if ( icon )
		{			
			iconActualWide = icon->EffectiveWidth( 1.0f );
			iconWide = iconActualWide + xSpacing;
			iconTall = icon->EffectiveHeight( 1.0f );
			
			int iconTallDesired = iLineTall-YRES(2);
			Assert( 0 != iconTallDesired );
			float flScale = (float) iconTallDesired / (float) iconTall;

			iconActualWide *= flScale;
			iconTall *= flScale;
			iconWide *= flScale;
		}

		if ( iconPreKillerName )
		{
			iconPrekillerActualWide = iconPreKillerName->EffectiveWidth( 1.0f );
			iconPrekillerWide = iconPrekillerActualWide;
			iconPrekillerTall = iconPreKillerName->EffectiveHeight( 1.0f );

			int iconTallDesired = iLineTall - YRES( 2 );
			Assert( 0 != iconTallDesired );
			float flScale = (float)iconTallDesired / (float)iconPrekillerTall;

			iconPrekillerActualWide *= flScale;
			iconPrekillerTall *= flScale;
			iconPrekillerWide *= flScale;
		}

		if ( iconPostKillerName )
		{
			iconPostkillerActualWide = iconPostKillerName->EffectiveWidth( 1.0f );
			iconPostkillerWide = iconPostkillerActualWide;
			iconPostkillerTall = iconPostKillerName->EffectiveHeight( 1.0f );

			int iconTallDesired = iLineTall-YRES(2);
			Assert( 0 != iconTallDesired );
			float flScale = (float) iconTallDesired / (float) iconPostkillerTall;

			iconPostkillerActualWide *= flScale;
			iconPostkillerTall *= flScale;
			iconPostkillerWide *= flScale;
		}
		
		if ( iconPostVictimName )
		{
			iconPostVictimActualWide = iconPostVictimName->EffectiveWidth( 1.0f );
			iconPostVictimWide = iconPostVictimActualWide;
			iconPostVictimTall = iconPostVictimName->EffectiveHeight( 1.0f );

			int iconTallDesired = iLineTall - YRES( 2 );
			Assert( 0 != iconTallDesired );
			float flScale = (float)iconTallDesired / (float)iconPostVictimTall;

			iconPostVictimActualWide *= flScale;
			iconPostVictimTall *= flScale;
			iconPostVictimWide *= flScale;
		}

		int iTotalWide = iKillerTextWide + iPlusIconWide + iAssisterTextWide + iconWide + iVictimTextWide + iDeathInfoTextWide + iDeathInfoEndTextWide + ( xMargin * 2 );
		iTotalWide += iconPrekillerWide + iconPostkillerWide + iPreKillerTextWide + iconPostVictimWide;

		int y = yStart + ( ( iLineTall + m_flLineSpacing ) * i );				
		int yText = y + ( ( iLineTall - iTextTall ) / 2 );
		int yIcon = y + ( ( iLineTall - iconTall ) / 2 );

		int x=0;
		if ( m_bRightJustify )
		{
			x =	GetWide() - iTotalWide;
		}

		// draw a background panel for the message
		Vertex_t vert[NUM_BACKGROUND_COORD];
		GetBackgroundPolygonVerts( x, y+1, x+iTotalWide, y+iLineTall-1, ARRAYSIZE( vert ), vert );		
		surface()->DrawSetTexture( -1 );
		surface()->DrawSetColor( GetBackgroundColor ( i ) );
		surface()->DrawTexturedPolygon( ARRAYSIZE( vert ), vert );

		x += xMargin;

		C_TF_PlayerResource *tf_PR = dynamic_cast<C_TF_PlayerResource *>(g_PR);

		// prekiller icon
		if ( iconPreKillerName )
		{
			int yPreIconTall = y + ( ( iLineTall - iconPrekillerTall ) / 2 );
			iconPreKillerName->DrawSelf( x, yPreIconTall, iconPrekillerActualWide, iconPrekillerTall, m_clrIcon);
			x += iconPrekillerWide + xSpacing;
		}

		if ( killer[0] )
		{
			// Draw killer's name
			Color clr = TFGameRules()->IsDeathmatch() ? tf_PR->GetPlayerColor(msg.Killer.iPlayerID) : GetTeamColor(msg.Killer.iTeam, msg.bLocalPlayerInvolved);
			DrawText( x, yText, m_hTextFont, clr, killer );
			x += iKillerTextWide;
		}

		if ( assister[0] )
		{
			// Draw a + between the names
			DrawText( x, yText, m_hTextFont, GetInfoTextColor( i, msg.bLocalPlayerInvolved ), L"+" );
			x += iPlusIconWide;

			// Draw assister's name
			Color clr = TFGameRules()->IsDeathmatch() ? tf_PR->GetPlayerColor(msg.Assister.iPlayerID) : GetTeamColor(msg.Assister.iTeam, msg.bLocalPlayerInvolved);
			DrawText(x, yText, m_hTextFont, clr, assister);
			x += iAssisterTextWide;
		}

		// prekiller text
		if ( msg.wzPreKillerText[0] )
		{
			x += xSpacing;
			DrawText( x + iDeathInfoOffset, yText, m_hTextFont, GetInfoTextColor( i, msg.bLocalPlayerInvolved ), msg.wzPreKillerText );
			x += iPreKillerTextWide;
		}

		// postkiller icon
		if ( iconPostKillerName )
		{
			int yPreIconTall = y + ( ( iLineTall - iconPostkillerTall ) / 2 );
			iconPostKillerName->DrawSelf( x, yPreIconTall, iconPostkillerActualWide, iconPostkillerTall, m_clrIcon );
			x += iconPostkillerWide + xSpacing;
		}

		// Draw glow behind weapon icon to show it was a crit death
		if (msg.bCrit && msg.iconCritDeath)
		{
			msg.iconCritDeath->DrawSelf(x, yIcon, iconActualWide, iconTall, m_clrIcon);
		}

		// Draw death icon
		if ( icon )
		{
			icon->DrawSelf( x, yIcon, iconActualWide, iconTall, m_clrIcon );
			x += iconWide;
		}

		// Draw additional info text next to death icon 
		if ( msg.wzInfoText[0] )
		{
			if ( msg.bSelfInflicted )
			{
				iDeathInfoOffset += iVictimTextWide;
				iVictimTextOffset -= iDeathInfoTextWide;
			}

			DrawText( x + iDeathInfoOffset, yText, m_hTextFont, GetInfoTextColor( i, msg.bLocalPlayerInvolved ), msg.wzInfoText );
			x += iDeathInfoTextWide;
		}

		// Draw victims name
		Color clr = TFGameRules()->IsDeathmatch() ? tf_PR->GetPlayerColor(msg.Victim.iPlayerID) : GetTeamColor(msg.Victim.iTeam, msg.bLocalPlayerInvolved);
		DrawText(x + iVictimTextOffset, yText, m_hTextFont, clr, victim);
		x += iVictimTextWide;

		// postkiller icon
		if ( iconPostVictimName )
		{
			int yPreIconTall = y + ( ( iLineTall - iconPostVictimTall ) / 2 );
			iconPostVictimName->DrawSelf( x, yPreIconTall, iconPostVictimActualWide, iconPostVictimTall, m_clrIcon );
			x += iconPostkillerWide + xSpacing;
		}

		// Draw Additional Text on the end of the victims name
		if ( msg.wzInfoTextEnd[0] )
		{
			DrawText( x , yText, m_hTextFont, GetInfoTextColor( i, msg.bLocalPlayerInvolved ), msg.wzInfoTextEnd );
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: returns the color to draw text in for this team.  
//-----------------------------------------------------------------------------
Color CTFHudDeathNotice::GetTeamColor( int iTeamNumber, bool bLocalPlayerInvolved /* = false */ )
{
	switch ( iTeamNumber )
	{
	case TF_TEAM_BLUE:
		return m_clrBlueText;
		break;
	case TF_TEAM_RED:
		return m_clrRedText;
		break;
	case TF_TEAM_GREEN:
		return m_clrGreenText;
		break;
	case TF_TEAM_YELLOW:
		return m_clrYellowText;
		break;
	case TEAM_UNASSIGNED:		
		return bLocalPlayerInvolved ? Color(0, 0, 0, 255) : Color(255, 255, 255, 255);
		break;
	default:
		AssertOnce( false );	// invalid team
		return bLocalPlayerInvolved ? Color(0, 0, 0, 255) : Color(255, 255, 255, 255);
		break;
	}
}
