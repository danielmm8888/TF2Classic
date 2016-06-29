//====== Copyright © 1996-2013, Valve Corporation, All rights reserved. =======//
//
// Purpose: Base class for Deathmatch powerups 
//
//=============================================================================//
#include "cbase.h"
#include "entity_capture_flag.h"
#include "view.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

ConVar tf2c_powerup_respawn_timer( "tf2c_powerup_respawn_timer", "1", FCVAR_ARCHIVE, "Show visual respawn timers for power-ups in Deathmatch." );
ConVar tf2c_powerup_respawn_size( "tf2c_powerup_respawn_size", "40", FCVAR_CHEAT );

class C_TFBaseDMPowerup : public C_BaseAnimating
{
public:
	DECLARE_CLASS( C_TFBaseDMPowerup, C_BaseAnimating );
	DECLARE_CLIENTCLASS();

	C_TFBaseDMPowerup();

	void	OnDataChanged( DataUpdateType_t type );

	void	Spawn( void );
	void	ClientThink( void );
	int		DrawModel( int flags );

private:
	QAngle		m_qAngle;

	bool		m_bRespawning;
	float		m_flRespawnTime;
	float		m_flRespawnAtTime;

	IMaterial	*m_pReturnProgressMaterial_Empty;		// For labels above players' heads.
	IMaterial	*m_pReturnProgressMaterial_Full;
};

IMPLEMENT_CLIENTCLASS_DT( C_TFBaseDMPowerup, DT_TFBaseDMPowerup, CTFBaseDMPowerup )
	RecvPropBool( RECVINFO( m_bRespawning ) ),
	RecvPropTime( RECVINFO( m_flRespawnTime ) ),
	RecvPropTime( RECVINFO( m_flRespawnAtTime ) ),
END_RECV_TABLE()

C_TFBaseDMPowerup::C_TFBaseDMPowerup()
{
	m_qAngle = vec3_angle;

	m_pReturnProgressMaterial_Empty = NULL;
	m_pReturnProgressMaterial_Full = NULL;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_TFBaseDMPowerup::OnDataChanged( DataUpdateType_t type )
{
	BaseClass::OnDataChanged( type );

	if ( type == DATA_UPDATE_CREATED )
	{
		SetNextClientThink( CLIENT_THINK_ALWAYS );
	}
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_TFBaseDMPowerup::Spawn( void )
{
	BaseClass::Spawn();
	m_qAngle = GetAbsAngles();
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
void C_TFBaseDMPowerup::ClientThink( void )
{
	m_qAngle.y += 90 * gpGlobals->frametime;
	if ( m_qAngle.y >= 360 )
		m_qAngle.y -= 360;

	SetAbsAngles( m_qAngle );

	SetNextClientThink( CLIENT_THINK_ALWAYS );
}

int C_TFBaseDMPowerup::DrawModel( int flags )
{
	int ret = BaseClass::DrawModel( flags );

	if ( m_bRespawning && tf2c_powerup_respawn_timer.GetBool() )
	{
		if ( !m_pReturnProgressMaterial_Full )
		{
			m_pReturnProgressMaterial_Full = materials->FindMaterial( "VGUI/flagtime_full", TEXTURE_GROUP_VGUI );
		}

		if ( !m_pReturnProgressMaterial_Empty )
		{
			m_pReturnProgressMaterial_Empty = materials->FindMaterial( "VGUI/flagtime_empty", TEXTURE_GROUP_VGUI );
		}

		if ( !m_pReturnProgressMaterial_Full || !m_pReturnProgressMaterial_Empty )
		{
			return ret;
		}

		CMatRenderContextPtr pRenderContext( materials );

		float flSize = tf2c_powerup_respawn_size.GetFloat();

		Vector vOrigin = GetAbsOrigin()/* + Vector( 0, 0, flSize + 10 )*/;
		QAngle vAngle = vec3_angle;

		// Align it towards the viewer
		Vector vUp = CurrentViewUp();
		Vector vRight = CurrentViewRight();
		if ( fabs( vRight.z ) > 0.95 )	// don't draw it edge-on
			return ret;

		vRight.z = 0;
		VectorNormalize( vRight );

		unsigned char ubColor[4] = { 190, 190, 40, 192 };

		// First we draw a quad of a complete icon, background
		CMeshBuilder meshBuilder;

		pRenderContext->Bind( m_pReturnProgressMaterial_Empty );
		IMesh *pMesh = pRenderContext->GetDynamicMesh();

		meshBuilder.Begin( pMesh, MATERIAL_QUADS, 1 );

		meshBuilder.Color4ubv( ubColor );
		meshBuilder.TexCoord2f( 0, 0, 0 );
		meshBuilder.Position3fv( ( vOrigin + ( vRight * -flSize ) + ( vUp * flSize ) ).Base() );
		meshBuilder.AdvanceVertex();

		meshBuilder.Color4ubv( ubColor );
		meshBuilder.TexCoord2f( 0, 1, 0 );
		meshBuilder.Position3fv( ( vOrigin + ( vRight * flSize ) + ( vUp * flSize ) ).Base() );
		meshBuilder.AdvanceVertex();

		meshBuilder.Color4ubv( ubColor );
		meshBuilder.TexCoord2f( 0, 1, 1 );
		meshBuilder.Position3fv( ( vOrigin + ( vRight * flSize ) + ( vUp * -flSize ) ).Base() );
		meshBuilder.AdvanceVertex();

		meshBuilder.Color4ubv( ubColor );
		meshBuilder.TexCoord2f( 0, 0, 1 );
		meshBuilder.Position3fv( ( vOrigin + ( vRight * -flSize ) + ( vUp * -flSize ) ).Base() );
		meshBuilder.AdvanceVertex();

		meshBuilder.End();

		pMesh->Draw();

		float flProgress = ( m_flRespawnAtTime - gpGlobals->curtime ) / m_flRespawnTime;

		pRenderContext->Bind( m_pReturnProgressMaterial_Full );
		pMesh = pRenderContext->GetDynamicMesh();

		vRight *= flSize * 2;
		vUp *= flSize * -2;

		// Next we're drawing the circular progress bar, in 8 segments
		// For each segment, we calculate the vertex position that will draw
		// the slice.
		int i;
		for ( i = 0; i<8; i++ )
		{
			if ( flProgress < Segments[i].maxProgress )
			{
				CMeshBuilder meshBuilder_Full;

				meshBuilder_Full.Begin( pMesh, MATERIAL_TRIANGLES, 3 );

				// vert 0 is ( 0.5, 0.5 )
				meshBuilder_Full.Color4ubv( ubColor );
				meshBuilder_Full.TexCoord2f( 0, 0.5, 0.5 );
				meshBuilder_Full.Position3fv( vOrigin.Base() );
				meshBuilder_Full.AdvanceVertex();

				// Internal progress is the progress through this particular slice
				float internalProgress = RemapVal( flProgress, Segments[i].maxProgress - 0.125, Segments[i].maxProgress, 0.0, 1.0 );
				internalProgress = clamp( internalProgress, 0.0, 1.0 );

				// Calculate the x,y of the moving vertex based on internal progress
				float swipe_x = Segments[i].vert2x - ( 1.0 - internalProgress ) * 0.5 * Segments[i].swipe_dir_x;
				float swipe_y = Segments[i].vert2y - ( 1.0 - internalProgress ) * 0.5 * Segments[i].swipe_dir_y;

				// vert 1 is calculated from progress
				meshBuilder_Full.Color4ubv( ubColor );
				meshBuilder_Full.TexCoord2f( 0, swipe_x, swipe_y );
				meshBuilder_Full.Position3fv( ( vOrigin + ( vRight * ( swipe_x - 0.5 ) ) + ( vUp *( swipe_y - 0.5 ) ) ).Base() );
				meshBuilder_Full.AdvanceVertex();

				// vert 2 is ( Segments[i].vert1x, Segments[i].vert1y )
				meshBuilder_Full.Color4ubv( ubColor );
				meshBuilder_Full.TexCoord2f( 0, Segments[i].vert2x, Segments[i].vert2y );
				meshBuilder_Full.Position3fv( ( vOrigin + ( vRight * ( Segments[i].vert2x - 0.5 ) ) + ( vUp *( Segments[i].vert2y - 0.5 ) ) ).Base() );
				meshBuilder_Full.AdvanceVertex();

				meshBuilder_Full.End();

				pMesh->Draw();
			}
		}
	}

	return ret;
}
