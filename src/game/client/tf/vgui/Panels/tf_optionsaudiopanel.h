//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//

#ifndef OPTIONS_SUB_AUDIO_H
#define OPTIONS_SUB_AUDIO_H
#ifdef _WIN32
#pragma once
#endif

#include <language.h>
#include "tf_dialogpanelbase.h"

class CPanelListPanel;
class CLabeledCommandComboBox;
class CCvarSlider;
class CCvarToggleCheckButton;
class CTFAdvSlider;
class CTFAdvCheckButton;
typedef struct IVoiceTweak_s IVoiceTweak;

//-----------------------------------------------------------------------------
// Purpose: Audio Details, Part of OptionsDialog
//-----------------------------------------------------------------------------
class CTFOptionsAudioPanel : public CTFDialogPanelBase
{
	DECLARE_CLASS_SIMPLE(CTFOptionsAudioPanel, CTFDialogPanelBase);

public:
	CTFOptionsAudioPanel(vgui::Panel* parent, const char *panelName);
	~CTFOptionsAudioPanel();
	virtual bool Init();
	virtual void OnResetData();
	virtual void OnApplyChanges();
	virtual void OnCommand(const char *command);
	bool RequiresRestart();
	static char* GetUpdatedAudioLanguage() { return m_pchUpdatedAudioLanguage; }
protected:
	void ApplySchemeSettings(vgui::IScheme *pScheme);
	void CreateControls();
	void DestroyControls();

private:
	MESSAGE_FUNC( OnControlModified, "ControlModified" );
	MESSAGE_FUNC( OnTextChanged, "TextChanged" )
	{
		OnControlModified();
	}

	MESSAGE_FUNC( RunTestSpeakers, "RunTestSpeakers" );

	vgui::ComboBox				*m_pSpeakerSetupCombo;
	vgui::ComboBox				*m_pSoundQualityCombo;
	CCvarSlider					*m_pSFXSlider;
	CCvarSlider					*m_pMusicSlider;
	vgui::ComboBox				*m_pCloseCaptionCombo;
	bool						   m_bRequireRestart;
   
   vgui::ComboBox				*m_pSpokenLanguageCombo;
   MESSAGE_FUNC( OpenThirdPartySoundCreditsDialog, "OpenThirdPartySoundCreditsDialog" );
   vgui::DHANDLE<class CTFOptionsAudioPanelThirdPartyCreditsDlg> m_OptionsSubAudioThirdPartyCreditsDlg;
   ELanguage         m_nCurrentAudioLanguage;
   static char             *m_pchUpdatedAudioLanguage;


   //from the voice settings:
   IVoiceTweak				*m_pVoiceTweak;		// Engine voice tweak API.
   CTFAdvCheckButton		*m_pMicBoost;
   vgui::ImagePanel        *m_pMicMeter;
   vgui::ImagePanel        *m_pMicMeter2;
   vgui::Button            *m_pTestMicrophoneButton;
   vgui::Label             *m_pMicrophoneSliderLabel;
   CTFAdvSlider			*m_pMicrophoneVolume;
   vgui::Label             *m_pReceiveSliderLabel;
   CCvarSlider             *m_pReceiveVolume;
   CCvarToggleCheckButton  *m_pVoiceEnableCheckButton;
   int                     m_nMicVolumeValue;
   bool                    m_bMicBoostSelected;
   float                   m_fReceiveVolume;
   int                     m_nReceiveSliderValue;
};



#endif // OPTIONS_SUB_AUDIO_H
