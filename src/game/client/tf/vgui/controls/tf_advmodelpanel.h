#ifndef CTF_MODEL_PANEL_H
#define CTF_MODEL_PANEL_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_advbuttonbase.h"
#include "basemodel_panel.h"

class CStudioHdr;

class CTFAdvModelPanel : public CBaseModelPanel
{
	DECLARE_CLASS_SIMPLE(CTFAdvModelPanel, CBaseModelPanel);
public:
	CTFAdvModelPanel( vgui::Panel *parent, const char *name );
	virtual ~CTFAdvModelPanel();

	virtual void ApplySettings(KeyValues *inResourceData);
	virtual void PerformLayout();
	virtual void OnThink();
	virtual void Paint();
	virtual void SetupLights() { }

	void SetModelName(const char* name, int skin = 0);
	void SetParticleName(const char* name);

	void SetAutoRotate(bool bState) { m_bAutoRotate = bState; };
	bool GetAutoRotate() { return m_bAutoRotate; };

	void SetAnimationIndex(int iIndex) { m_iAnimationIndex = iIndex; };

	void Update();

	// function to set up scene sets
	//void SetupCustomLights( Color cAmbient, Color cKey, float fKeyBoost, Color cRim, float fRimBoost );

	void SetBodygroup( int iGroup, int iValue );
	int FindBodygroupByName( const char *name );
	int GetNumBodyGroups();
	CStudioHdr *GetModelPtr();
	

	bool m_bShouldPaint;

private:
	CStudioHdr* m_pStudioHdr;
	particle_data_t *m_pData;
	bool	m_bAutoRotate;
	int		m_iAnimationIndex;

protected:
	virtual void PostPaint3D(IMatRenderContext *pRenderContext) OVERRIDE;
};


#endif // CTF_MODEL_PANEL_H
