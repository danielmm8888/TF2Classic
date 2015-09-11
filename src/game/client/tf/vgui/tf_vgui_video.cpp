//====== Copyright © 1996-2007, Valve Corporation, All rights reserved. =======
//
// Purpose: VGUI panel which can play back video, in-engine
//
//=============================================================================

#include "cbase.h"
#include <vgui/IVGui.h>
#include <vgui/ISurface.h>
#include <KeyValues.h>
#include "vgui_video.h"
#include "tf_vgui_video.h"
#include "engine/IEngineSound.h"


// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


DECLARE_BUILD_FACTORY( CTFVideoPanel );

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
CTFVideoPanel::CTFVideoPanel( vgui::Panel *parent, const char *panelName ) : VideoPanel( 0, 0, 50, 50 )
{
	SetParent( parent );
	SetProportional( true );
	SetKeyBoardInputEnabled( false );

	SetBlackBackground( false );

	m_flStartAnimDelay = 0.0f;
	m_flEndAnimDelay = 0.0f;
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
CTFVideoPanel::~CTFVideoPanel()
{
	ReleaseVideo();
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CTFVideoPanel::ReleaseVideo()
{
	enginesound->NotifyEndMoviePlayback();

	// Destroy any previously allocated video
	if (m_VideoMaterial != NULL)
	{
		g_pVideo->DestroyVideoMaterial(m_VideoMaterial);
		m_VideoMaterial = NULL;
	}
}

//-----------------------------------------------------------------------------
// 
//-----------------------------------------------------------------------------
void CTFVideoPanel::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

	SetExitCommand( inResourceData->GetString( "command", "" ) );
	m_flStartAnimDelay = inResourceData->GetFloat( "start_delay", 0.0 );
	m_flEndAnimDelay = inResourceData->GetFloat( "end_delay", 0.0 );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVideoPanel::GetPanelPos( int &xpos, int &ypos )
{
	vgui::ipanel()->GetAbsPos( GetVPanel(), xpos, ypos );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVideoPanel::OnVideoOver()
{
	BaseClass::OnVideoOver();
	PostMessage( GetParent(), new KeyValues( "IntroFinished" ) );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVideoPanel::OnClose()
{
	// Fire an exit command if we're asked to do so
	if ( m_szExitCommand[0] )
	{
		engine->ClientCmd( m_szExitCommand );
	}

	// intentionally skipping VideoPanel::OnClose()
	EditablePanel::OnClose();

	SetVisible( false );
}
//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFVideoPanel::Shutdown()
{
	OnClose();
	ReleaseVideo();
}

//-----------------------------------------------------------------------------
// Purpose: Begins playback of a movie
// Output : Returns true on success, false on failure.
//-----------------------------------------------------------------------------
bool CTFVideoPanel::BeginPlaybackNoAudio(const char *pFilename)
{
	// need working video services
	if (g_pVideo == NULL)
		return false;

	// Create a new video material
	if (m_VideoMaterial != NULL)
	{
		g_pVideo->DestroyVideoMaterial(m_VideoMaterial);
		m_VideoMaterial = NULL;
	}

	m_VideoMaterial = g_pVideo->CreateVideoMaterial("VideoMaterial", pFilename, "GAME",
		VideoPlaybackFlags::DEFAULT_MATERIAL_OPTIONS,
		VideoSystem::DETERMINE_FROM_FILE_EXTENSION, m_bAllowAlternateMedia);

	if (m_VideoMaterial == NULL)
		return false;

	//No audio

	int nWidth, nHeight;
	m_VideoMaterial->GetVideoImageSize(&nWidth, &nHeight);
	m_VideoMaterial->GetVideoTexCoordRange(&m_flU, &m_flV);
	m_pMaterial = m_VideoMaterial->GetMaterial();


	float flFrameRatio = ((float)GetWide() / (float)GetTall());
	float flVideoRatio = ((float)nWidth / (float)nHeight);

	if (flVideoRatio > flFrameRatio)
	{
		m_nPlaybackWidth = GetWide();
		m_nPlaybackHeight = (GetWide() / flVideoRatio);
	}
	else if (flVideoRatio < flFrameRatio)
	{
		m_nPlaybackWidth = (GetTall() * flVideoRatio);
		m_nPlaybackHeight = GetTall();
	}
	else
	{
		m_nPlaybackWidth = GetWide();
		m_nPlaybackHeight = GetTall();
	}

	return true;
}
