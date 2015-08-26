//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef tf_cvarslider_H
#define tf_cvarslider_H
#ifdef _WIN32
#pragma once
#endif

//#include <vgui_controls/Slider.h>
#include "tf_advslider.h"

class CCvarSlider : public CTFAdvSlider
{
	DECLARE_CLASS_SIMPLE(CCvarSlider, CTFAdvSlider);

public:

	CCvarSlider(vgui::Panel *parent, const char *panelName, const char *name);
	CCvarSlider( vgui::Panel *parent, const char *panelName, char const *caption,
		float minValue, float maxValue, char const *cvarname, bool bAllowOutOfRange=false );
	~CCvarSlider();

	void			SetupSlider( float minValue, float maxValue, const char *cvarname, bool bAllowOutOfRange );

	void			SetCVarName( char const *cvarname );
	void			SetMinMaxValues( float minValue, float maxValue, bool bSetTickdisplay = true );
	void			SetTickColor( Color color );

	virtual void	Paint();

	virtual void	ApplySettings( KeyValues *inResourceData );
	virtual void	GetSettings( KeyValues *outResourceData );

	void			ApplyChanges();
	float			GetSliderValue();
    void            SetSliderValue(float fValue);
    void            Reset();
    bool            HasBeenModified();

private:
	MESSAGE_FUNC( OnSliderMoved, "SliderMoved" );
	MESSAGE_FUNC( OnApplyChanges, "ApplyChanges" );

    bool        m_bAllowOutOfRange;
    bool        m_bModifiedOnce;
    float       m_fStartValue;
    int         m_iStartValue;
    int         m_iLastSliderValue;
    float       m_fCurrentValue;
	char		m_szCvarName[ 64 ];

	bool		m_bCreatedInCode;
	float		m_flMinValue;
	float		m_flMaxValue;
};

#endif // tf_cvarslider_H
