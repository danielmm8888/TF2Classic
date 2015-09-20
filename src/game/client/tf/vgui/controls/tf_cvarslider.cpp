//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//
//=============================================================================//
#include "cbase.h"
#include "tf_cvarslider.h"
#include <stdio.h>
//#include "EngineInterface.h"
#include "tier1/KeyValues.h"
#include "tier1/convar.h"
#include <vgui/IVGui.h>
#include <vgui_controls/PropertyPage.h>

#define tf_cvarslider_SCALE_FACTOR 1.0f

// memdbgon must be the last include file in a .cpp file!!!
#include <tier0/memdbgon.h>

using namespace vgui;

//DECLARE_BUILD_FACTORY( CCvarSlider );
vgui::Panel *CCvarSlider_Factory()
{
	return new CCvarSlider(NULL, NULL, "CCvarSlider");
}
DECLARE_BUILD_FACTORY_CUSTOM(CCvarSlider, CCvarSlider_Factory);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCvarSlider::CCvarSlider(Panel *parent, const char *panelName, const char *name) : CTFAdvSlider(parent, panelName, name)
{
	SetupSlider( 0, 1, "", false );
	m_bCreatedInCode = false;

	AddActionSignalTarget( this );
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCvarSlider::CCvarSlider( Panel *parent, const char *panelName, char const *caption,
		float minValue, float maxValue, char const *cvarname, bool bAllowOutOfRange )
		: CTFAdvSlider(parent, panelName, caption)
{
	AddActionSignalTarget( this );

	SetupSlider( minValue, maxValue, cvarname, bAllowOutOfRange );

	// For backwards compatability. Ignore .res file settings for forced setup sliders.
	m_bCreatedInCode = true;
	m_bShowInt = false;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::SetupSlider( float minValue, float maxValue, const char *cvarname, bool bAllowOutOfRange )
{
	m_flMinValue = minValue;
	m_flMaxValue = maxValue;

	// scale by tf_cvarslider_SCALE_FACTOR
	//SetRange( (int)( tf_cvarslider_SCALE_FACTOR * minValue ), (int)( tf_cvarslider_SCALE_FACTOR * maxValue ) );
	//SetMinMax((int)(tf_cvarslider_SCALE_FACTOR * minValue), (int)(tf_cvarslider_SCALE_FACTOR * maxValue));
	SetMinMax(minValue, maxValue);

	char szMin[ 32 ];
	char szMax[ 32 ];

	Q_snprintf( szMin, sizeof( szMin ), "%.2f", minValue );
	Q_snprintf( szMax, sizeof( szMax ), "%.2f", maxValue );

	//SetTickCaptions( szMin, szMax );

	Q_strncpy( m_szCvarName, cvarname, sizeof( m_szCvarName ) );
	SetCommandString(cvarname);

    m_bModifiedOnce = false;
    m_bAllowOutOfRange = bAllowOutOfRange;

	// Set slider to current value
	Reset();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CCvarSlider::~CCvarSlider()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::ApplySettings( KeyValues *inResourceData )
{
	BaseClass::ApplySettings( inResourceData );

	if ( !m_bCreatedInCode )
	{
		float minValue = inResourceData->GetFloat( "minvalue", 0 );
		float maxValue = inResourceData->GetFloat( "maxvalue", 1 );
		const char *cvarname = inResourceData->GetString( "cvar_name", "" );
		bool bAllowOutOfRange = inResourceData->GetInt( "allowoutofrange", 0 ) != 0;
		SetupSlider( minValue, maxValue, cvarname, bAllowOutOfRange );

		if ( GetParent() )
		{
			// HACK: If our parent is a property page, we want the dialog containing it
			if ( dynamic_cast<vgui::PropertyPage*>(GetParent()) && GetParent()->GetParent() )
			{
				GetParent()->GetParent()->AddActionSignalTarget( this );
			}
			else
			{
				GetParent()->AddActionSignalTarget( this );
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Get control settings for editing
//-----------------------------------------------------------------------------
void CCvarSlider::GetSettings( KeyValues *outResourceData )
{
	BaseClass::GetSettings(outResourceData);

	if ( !m_bCreatedInCode )
	{
		outResourceData->SetFloat( "minvalue", m_flMinValue );
		outResourceData->SetFloat( "maxvalue", m_flMaxValue );
		outResourceData->SetString( "cvar_name", m_szCvarName );
		outResourceData->SetInt( "allowoutofrange", m_bAllowOutOfRange );
	}
}

void CCvarSlider::SetCVarName( char const *cvarname )
{
	Q_strncpy( m_szCvarName, cvarname, sizeof( m_szCvarName ) );

	m_bModifiedOnce = false;

	// Set slider to current value
	Reset();
}

void CCvarSlider::SetMinMaxValues( float minValue, float maxValue, bool bSetTickDisplay )
{
	//SetMinMax( (int)( tf_cvarslider_SCALE_FACTOR * minValue ), (int)( tf_cvarslider_SCALE_FACTOR * maxValue ) );
	SetMinMax(minValue, maxValue);

	if ( bSetTickDisplay )
	{
		char szMin[ 32 ];
		char szMax[ 32 ];

		Q_snprintf( szMin, sizeof( szMin ), "%.2f", minValue );
		Q_snprintf( szMax, sizeof( szMax ), "%.2f", maxValue );


		//SetTickCaptions( szMin, szMax );
	}

	// Set slider to current value
	Reset();
}

void CCvarSlider::SetTickColor( Color color )
{
	//m_TickColor = color;
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::Paint()
{
	// Get engine's current value
//	float curvalue = engine->pfnGetCvarFloat( m_szCvarName );
	ConVarRef var( m_szCvarName );
	if ( !var.IsValid() )
		return;
	float curvalue = var.GetFloat();
	
    // did it get changed from under us?
    if (curvalue != m_fStartValue)
    {
		//int val = (int)( tf_cvarslider_SCALE_FACTOR * curvalue );
        m_fStartValue = curvalue;
        m_fCurrentValue = curvalue;
        
		SetValue(curvalue);
        m_iStartValue = GetValue();
    }

	BaseClass::Paint();
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::ApplyChanges()
{
    //if (m_bModifiedOnce)
    //{
	m_fStartValue = GetValue();
		/*
        if (m_bAllowOutOfRange)
        {
            m_fStartValue = m_fCurrentValue;
        }
        else
        {
            m_fStartValue = (float) m_iStartValue / tf_cvarslider_SCALE_FACTOR;
        }
		*/
		//engine->Cvar_SetValue( m_szCvarName, m_fStartValue );
	ConVarRef var( m_szCvarName );
	var.SetValue( (float)m_fStartValue );
	//}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CCvarSlider::GetSliderValue()
{	
    if (m_bAllowOutOfRange)
    {
	    return m_fCurrentValue; 
    }
    else
    {
       // return ((float)GetValue())/ tf_cvarslider_SCALE_FACTOR;
		return GetValue();
    }
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::SetSliderValue(float fValue)
{
    //int nVal = (int)( tf_cvarslider_SCALE_FACTOR * fValue );
    //SetValue( nVal, false);
	SetValue(fValue);

    // remember this slider value
    m_iLastSliderValue = GetValue();

    if (m_fCurrentValue != fValue)
    {
        m_fCurrentValue = fValue;
        m_bModifiedOnce = true;
    }
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::Reset()
{
	// Set slider to current value
//	m_fStartValue = engine->pfnGetCvarFloat( m_szCvarName );
	ConVarRef var( m_szCvarName );
	if ( !var.IsValid() )
	{
	    m_fCurrentValue = m_fStartValue = 0.0f;
		SetValue( 0 );
		m_iStartValue = GetValue();
	    m_iLastSliderValue = m_iStartValue;
		return;
	}
	float m_fStartValue = var.GetFloat();
    m_fCurrentValue = m_fStartValue;

    //int value = (int)( tf_cvarslider_SCALE_FACTOR * m_fStartValue );
	SetValue(m_fStartValue);

	m_iStartValue = GetValue();
    m_iLastSliderValue = m_iStartValue;

}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
bool CCvarSlider::HasBeenModified()
{
    if (GetValue() != m_iStartValue)
    {
        m_bModifiedOnce = true;
    }

    return m_bModifiedOnce;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : position - 
//-----------------------------------------------------------------------------
void CCvarSlider::OnSliderMoved()
{
	if (HasBeenModified())
	{
        if (m_iLastSliderValue != GetValue())
        {
            m_iLastSliderValue = GetValue();
            m_fCurrentValue = ((float) m_iLastSliderValue)/tf_cvarslider_SCALE_FACTOR;
        }

		// tell parent that we've been modified
		PostActionSignal(new KeyValues("ControlModified"));
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CCvarSlider::OnApplyChanges( void )
{
	if ( !m_bCreatedInCode )
	{
		ApplyChanges();
	}
}