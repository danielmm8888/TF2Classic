//========= Copyright © 1996-2005, Valve Corporation, All rights reserved. ============//
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================//
#include "cbase.h"
#include "tf_optionsvideopanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_scriptobject.h"
#include "controls/tf_advpanellistpanel.h"
#include "controls/tf_advbutton.h"
#include "IGameUIFuncs.h"
#include "modes.h"
#include "materialsystem/materialsystem_config.h"
#include "filesystem.h"
#include "vgui_controls/CheckButton.h"
#include "vgui_controls/ComboBox.h"
#include "vgui_controls/Frame.h"
#include "vgui_controls/QueryBox.h"
#include "controls/tf_cvarslider.h"
#include "controls/tf_cvartogglecheckbutton.h"
#include "tier1/KeyValues.h"
#include "vgui/IInput.h"
#include "vgui/ILocalize.h"
#include "vgui/ISystem.h"
#include "tier0/icommandline.h"
#include "tier1/convar.h"

#include "inetchannelinfo.h"

extern IMaterialSystem *materials;

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

using namespace vgui;

//-----------------------------------------------------------------------------
// Purpose: aspect ratio mappings (for normal/widescreen combo)
//-----------------------------------------------------------------------------
struct RatioToAspectMode_t
{
	int anamorphic;
	float aspectRatio;
};
RatioToAspectMode_t g_RatioToAspectModes[] =
{
	{	0,		4.0f / 3.0f },
	{	1,		16.0f / 9.0f },
	{	2,		16.0f / 10.0f },
	{	2,		1.0f },
};

//-----------------------------------------------------------------------------
// Purpose: list of valid dx levels
//-----------------------------------------------------------------------------
int g_DirectXLevels[] =
{
	70,
	80,
	81,
	90,
	95,
};

//-----------------------------------------------------------------------------
// Purpose: returns the string name of a given dxlevel
//-----------------------------------------------------------------------------
void GetNameForDXLevel( int dxlevel, char *name, int bufferSize)
{
	if( dxlevel == 95 )
	{
		Q_snprintf( name, bufferSize, "DirectX v9.0+" );
	}
	else
	{
		Q_snprintf( name, bufferSize, "DirectX v%.1f", dxlevel / 10.0f );
	}
}
	
//-----------------------------------------------------------------------------
// Purpose: returns the aspect ratio mode number for the given resolution
//-----------------------------------------------------------------------------
int GetScreenAspectMode( int width, int height )
{
	float aspectRatio = (float)width / (float)height;

	// just find the closest ratio
	float closestAspectRatioDist = 99999.0f;
	int closestAnamorphic = 0;
	for (int i = 0; i < ARRAYSIZE(g_RatioToAspectModes); i++)
	{
		float dist = fabs( g_RatioToAspectModes[i].aspectRatio - aspectRatio );
		if (dist < closestAspectRatioDist)
		{
			closestAspectRatioDist = dist;
			closestAnamorphic = g_RatioToAspectModes[i].anamorphic;
		}
	}

	return closestAnamorphic;
}

//-----------------------------------------------------------------------------
// Purpose: returns the string name of the specified resolution mode
//-----------------------------------------------------------------------------
void GetResolutionName( vmode_t *mode, char *sz, int sizeofsz )
{
	if ( mode->width == 1280 && mode->height == 1024 )
	{
		// LCD native monitor resolution gets special case
		Q_snprintf( sz, sizeofsz, "%i x %i (LCD)", mode->width, mode->height );
	}
	else
	{
		Q_snprintf( sz, sizeofsz, "%i x %i", mode->width, mode->height );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFOptionsVideoPanel::CTFOptionsVideoPanel(vgui::Panel *parent, const char *panelName) : CTFDialogPanelBase(parent, panelName)
{
	Init();
}

bool CTFOptionsVideoPanel::Init()
{
	BaseClass::Init();

	m_pListPanel = new CPanelListPanel(this, "PanelListPanel");
	return true;
}

void CTFOptionsVideoPanel::CreateControls()
{
	BaseClass::CreateControls();

	m_bRequireRestart = false;

	CTFAdvButton *pTitleVideo = new CTFAdvButton(this, "DescTextTitle", "Video");
	m_pMode = new ComboBox(this, "Resolution", 8, false);
	m_pAspectRatio = new ComboBox(this, "AspectRatio", 6, false);

	char pszAspectName[3][64];
	wchar_t *unicodeText = g_pVGuiLocalize->Find("#GameUI_AspectNormal");
	g_pVGuiLocalize->ConvertUnicodeToANSI(unicodeText, pszAspectName[0], 32);
	unicodeText = g_pVGuiLocalize->Find("#GameUI_AspectWide16x9");
	g_pVGuiLocalize->ConvertUnicodeToANSI(unicodeText, pszAspectName[1], 32);
	unicodeText = g_pVGuiLocalize->Find("#GameUI_AspectWide16x10");
	g_pVGuiLocalize->ConvertUnicodeToANSI(unicodeText, pszAspectName[2], 32);

	int iNormalItemID = m_pAspectRatio->AddItem(pszAspectName[0], NULL);
	int i16x9ItemID = m_pAspectRatio->AddItem(pszAspectName[1], NULL);
	int i16x10ItemID = m_pAspectRatio->AddItem(pszAspectName[2], NULL);

	const MaterialSystem_Config_t &config = materials->GetCurrentConfigForVideoCard();

	int iAspectMode = GetScreenAspectMode(config.m_VideoMode.m_Width, config.m_VideoMode.m_Height);
	switch (iAspectMode)
	{
	default:
	case 0:
		m_pAspectRatio->ActivateItem(iNormalItemID);
		break;
	case 1:
		m_pAspectRatio->ActivateItem(i16x9ItemID);
		break;
	case 2:
		m_pAspectRatio->ActivateItem(i16x10ItemID);
		break;
	}

	m_pWindowed = new ComboBox(this, "DisplayModeCombo", 6, false);
	m_pWindowed->AddItem("#GameUI_Fullscreen", NULL);
	m_pWindowed->AddItem("#GameUI_Windowed", NULL);

	m_pGammaSlider = new CCvarSlider(this, "Gamma", "#GameUI_Gamma", 1.6f, 2.6f, "mat_monitorgamma");

	// Moved down here so we can set the Drop down's 
	// menu state after the default (disabled) value is loaded
	PrepareResolutionList();

	// only show the benchmark button if they have the benchmark map
	//if (!g_pFullFileSystem->FileExists("maps/test_hardware.bsp"))
	//{
	//	m_pBenchmark->SetVisible(false);
	//}
	AddControl(pTitleVideo, O_CATEGORY);
	AddControl(m_pMode, O_LIST, "#GameUI_Resolution");
	AddControl(m_pAspectRatio, O_LIST, "#GameUI_AspectRatio");
	AddControl(m_pWindowed, O_LIST, "#GameUI_DisplayMode");
	AddControl(m_pGammaSlider, O_SLIDER);


	//VIDEOADV
	CTFAdvButton *pTitleVideoAdv = new CTFAdvButton(this, "DescTextTitle", "#GameUI_VideoAdvanced_Title");

	m_pDXLevel = new ComboBox(this, "dxlabel", 6, false);
	KeyValues *pKeyValues = new KeyValues("config");
	materials->GetRecommendedConfigurationInfo(0, pKeyValues);
	m_pDXLevel->DeleteAllItems();
	for (int i = 0; i < ARRAYSIZE(g_DirectXLevels); i++)
	{
		// don't allow choice of lower dxlevels than the default, 
		// unless we're already at that lower level or have it forced
		if (!CommandLine()->CheckParm("-dxlevel") &&
			g_DirectXLevels[i] != config.dxSupportLevel &&
			g_DirectXLevels[i] < pKeyValues->GetInt("ConVar.mat_dxlevel"))
			continue;

		KeyValues *pTempKV = new KeyValues("config");
		if (g_DirectXLevels[i] == pKeyValues->GetInt("ConVar.mat_dxlevel")
			|| materials->GetRecommendedConfigurationInfo(g_DirectXLevels[i], pTempKV))
		{
			// add the configuration in the combo
			char szDXLevelName[64];
			GetNameForDXLevel(g_DirectXLevels[i], szDXLevelName, sizeof(szDXLevelName));
			m_pDXLevel->AddItem(szDXLevelName, new KeyValues("dxlevel", "dxlevel", g_DirectXLevels[i]));
		}

		pTempKV->deleteThis();
	}
	pKeyValues->deleteThis();

	m_pModelDetail = new ComboBox(this, "ModelDetail", 6, false);
	m_pModelDetail->AddItem("#gameui_low", NULL);
	m_pModelDetail->AddItem("#gameui_medium", NULL);
	m_pModelDetail->AddItem("#gameui_high", NULL);

	m_pTextureDetail = new ComboBox(this, "TextureDetail", 6, false);
	m_pTextureDetail->AddItem("#gameui_low", NULL);
	m_pTextureDetail->AddItem("#gameui_medium", NULL);
	m_pTextureDetail->AddItem("#gameui_high", NULL);
	m_pTextureDetail->AddItem("#gameui_ultra", NULL);

	// Build list of MSAA and CSAA modes, based upon those which are supported by the device
	//
	// The modes that we've seen in the wild to date are as follows (in perf order, fastest to slowest)
	//
	//								2x	4x	6x	8x	16x	8x	16xQ
	//		Texture/Shader Samples	1	1	1	1	1	1	1
	//		Stored Color/Z Samples	2	4	6	4	4	8	8
	//		Coverage Samples		2	4	6	8	16	8	16
	//		MSAA or CSAA			M	M	M	C	C	M	C
	//
	//	The CSAA modes are nVidia only (added in the G80 generation of GPUs)
	//
	m_nNumAAModes = 0;
	m_pAntialiasingMode = new ComboBox(this, "AntialiasingMode", 10, false);
	m_pAntialiasingMode->AddItem("#GameUI_None", NULL);
	m_nAAModes[m_nNumAAModes].m_nNumSamples = 1;
	m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
	m_nNumAAModes++;

	if (materials->SupportsMSAAMode(2))
	{
		m_pAntialiasingMode->AddItem("#GameUI_2X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 2;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;
	}

	if (materials->SupportsMSAAMode(4))
	{
		m_pAntialiasingMode->AddItem("#GameUI_4X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;
	}

	if (materials->SupportsMSAAMode(6))
	{
		m_pAntialiasingMode->AddItem("#GameUI_6X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 6;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;
	}

	if (materials->SupportsCSAAMode(4, 2))							// nVidia CSAA			"8x"
	{
		m_pAntialiasingMode->AddItem("#GameUI_8X_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 2;
		m_nNumAAModes++;
	}

	if (materials->SupportsCSAAMode(4, 4))							// nVidia CSAA			"16x"
	{
		m_pAntialiasingMode->AddItem("#GameUI_16X_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 4;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 4;
		m_nNumAAModes++;
	}

	if (materials->SupportsMSAAMode(8))
	{
		m_pAntialiasingMode->AddItem("#GameUI_8X", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 8;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 0;
		m_nNumAAModes++;
	}

	if (materials->SupportsCSAAMode(8, 2))							// nVidia CSAA			"16xQ"
	{
		m_pAntialiasingMode->AddItem("#GameUI_16XQ_CSAA", NULL);
		m_nAAModes[m_nNumAAModes].m_nNumSamples = 8;
		m_nAAModes[m_nNumAAModes].m_nQualityLevel = 2;
		m_nNumAAModes++;
	}

	m_pFilteringMode = new ComboBox(this, "FilteringMode", 6, false);
	m_pFilteringMode->AddItem("#GameUI_Bilinear", NULL);
	m_pFilteringMode->AddItem("#GameUI_Trilinear", NULL);
	m_pFilteringMode->AddItem("#GameUI_Anisotropic2X", NULL);
	m_pFilteringMode->AddItem("#GameUI_Anisotropic4X", NULL);
	m_pFilteringMode->AddItem("#GameUI_Anisotropic8X", NULL);
	m_pFilteringMode->AddItem("#GameUI_Anisotropic16X", NULL);

	m_pShadowDetail = new ComboBox(this, "ShadowDetail", 6, false);
	m_pShadowDetail->AddItem("#gameui_low", NULL);
	m_pShadowDetail->AddItem("#gameui_medium", NULL);
	if (materials->SupportsShadowDepthTextures())
	{
		m_pShadowDetail->AddItem("#gameui_high", NULL);
	}

	ConVarRef mat_dxlevel("mat_dxlevel");

	m_pHDR = new ComboBox(this, "HDR", 6, false);
	m_pHDR->AddItem("#GameUI_hdr_level0", NULL);
	m_pHDR->AddItem("#GameUI_hdr_level1", NULL);

	if (materials->SupportsHDRMode(HDR_TYPE_INTEGER))
	{
		m_pHDR->AddItem("#GameUI_hdr_level2", NULL);
	}
#if 0
	if (materials->SupportsHDRMode(HDR_TYPE_FLOAT))
	{
		m_pHDR->AddItem("#GameUI_hdr_level3", NULL);
	}
#endif

	m_pHDR->SetEnabled(mat_dxlevel.GetInt() >= 80);

	m_pWaterDetail = new ComboBox(this, "WaterDetail", 6, false);
	m_pWaterDetail->AddItem("#gameui_noreflections", NULL);
	m_pWaterDetail->AddItem("#gameui_reflectonlyworld", NULL);
	m_pWaterDetail->AddItem("#gameui_reflectall", NULL);

	m_pVSync = new ComboBox(this, "VSync", 2, false);
	m_pVSync->AddItem("#gameui_disabled", NULL);
	m_pVSync->AddItem("#gameui_enabled", NULL);

	m_pShaderDetail = new ComboBox(this, "ShaderDetail", 6, false);
	m_pShaderDetail->AddItem("#gameui_low", NULL);
	m_pShaderDetail->AddItem("#gameui_high", NULL);

	m_pColorCorrection = new ComboBox(this, "ColorCorrection", 2, false);
	m_pColorCorrection->AddItem("#gameui_disabled", NULL);
	m_pColorCorrection->AddItem("#gameui_enabled", NULL);

	m_pMotionBlur = new ComboBox(this, "MotionBlur", 2, false);
	m_pMotionBlur->AddItem("#gameui_disabled", NULL);
	m_pMotionBlur->AddItem("#gameui_enabled", NULL);

	m_pQueuedMode = new ComboBox(this, "Multicore", 2, false);
	m_pQueuedMode->AddItem("#gameui_disabled", NULL);
	m_pQueuedMode->AddItem("#gameui_enabled", NULL);

	m_pFOVSlider = new CCvarSlider(this, "FOV", "#GameUI_FOV", 75.0f, 100.0f, "fov_desired");

	m_pDXLevel->SetVisible(false);

	m_pColorCorrection->SetEnabled(mat_dxlevel.GetInt() >= 90);
	m_pMotionBlur->SetEnabled(mat_dxlevel.GetInt() >= 90);

	//MarkDefaultSettingsAsRecommended();
	m_bUseChanges = false;

	AddControl(pTitleVideoAdv, O_CATEGORY);
	AddControl(m_pModelDetail, O_LIST, "#GameUI_Model_Detail");
	AddControl(m_pTextureDetail, O_LIST, "#GameUI_Texture_Detail");
	AddControl(m_pShadowDetail, O_LIST, "#GameUI_Shadow_Detail");
	AddControl(m_pWaterDetail, O_LIST, "#GameUI_Water_Detail");
	AddControl(m_pShaderDetail, O_LIST, "#GameUI_Shader_Detail");
	AddControl(m_pColorCorrection, O_LIST, "#GameUI_Color_Correction");
	AddControl(m_pAntialiasingMode, O_LIST, "#GameUI_Antialiasing_Mode");
	AddControl(m_pFilteringMode, O_LIST, "#GameUI_Filtering_Mode");
	AddControl(m_pVSync, O_LIST, "#GameUI_Wait_For_VSync");
	AddControl(m_pMotionBlur, O_LIST, "#GameUI_MotionBlur");
	AddControl(m_pFOVSlider, O_SLIDER);
	AddControl(m_pQueuedMode, O_LIST, "#GameUI_MulticoreRendering");
	AddControl(m_pHDR, O_LIST, "#GameUI_HDR");
}


void CTFOptionsVideoPanel::DestroyControls()
{
	BaseClass::DestroyControls();
}

//-----------------------------------------------------------------------------
// Purpose: sets background color & border
//-----------------------------------------------------------------------------
void CTFOptionsVideoPanel::ApplySchemeSettings(IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/OptionsVideo.res");
}

void CTFOptionsVideoPanel::OnCommand(const char* command)
{
	BaseClass::OnCommand(command);
}

//-----------------------------------------------------------------------------
// Purpose: Generates resolution list
//-----------------------------------------------------------------------------
void CTFOptionsVideoPanel::PrepareResolutionList()
{
	// get the currently selected resolution
	char sz[256];
	m_pMode->GetText(sz, 256);
	int currentWidth = 0, currentHeight = 0;
	sscanf( sz, "%i x %i", &currentWidth, &currentHeight );

	// Clean up before filling the info again.
	m_pMode->DeleteAllItems();
	m_pAspectRatio->SetItemEnabled(1, false);
	m_pAspectRatio->SetItemEnabled(2, false);

	// get full video mode list
	vmode_t *plist = NULL;
	int count = 0;
	gameuifuncs->GetVideoModes( &plist, &count );

	const MaterialSystem_Config_t &config = materials->GetCurrentConfigForVideoCard();

	bool bWindowed = (m_pWindowed->GetActiveItem() > 0);
	int desktopWidth, desktopHeight;
	gameuifuncs->GetDesktopResolution( desktopWidth, desktopHeight );

	// iterate all the video modes adding them to the dropdown
	bool bFoundWidescreen = false;
	int selectedItemID = -1;
	for (int i = 0; i < count; i++, plist++)
	{
		char sz[ 256 ];
		GetResolutionName( plist, sz, sizeof( sz ) );

		// don't show modes bigger than the desktop for windowed mode
		if ( bWindowed && (plist->width > desktopWidth || plist->height > desktopHeight) )
			continue;

		int itemID = -1;
		int iAspectMode = GetScreenAspectMode( plist->width, plist->height );
		if ( iAspectMode > 0 )
		{
			m_pAspectRatio->SetItemEnabled( iAspectMode, true );
			bFoundWidescreen = true;
		}

		// filter the list for those matching the current aspect
		if ( iAspectMode == m_pAspectRatio->GetActiveItem() )
		{
			itemID = m_pMode->AddItem( sz, NULL);
		}

		// try and find the best match for the resolution to be selected
		if ( plist->width == currentWidth && plist->height == currentHeight )
		{
			selectedItemID = itemID;
		}
		else if ( selectedItemID == -1 && plist->width == config.m_VideoMode.m_Width && plist->height == config.m_VideoMode.m_Height )
		{
			selectedItemID = itemID;
		}
	}

	// disable ratio selection if we can't display widescreen.
	m_pAspectRatio->SetEnabled( bFoundWidescreen );

	m_nSelectedMode = selectedItemID;

	if (selectedItemID != -1)
	{
		m_pMode->ActivateItem(selectedItemID);
	}
	else
	{
		char sz[256];
		sprintf( sz, "%d x %d", config.m_VideoMode.m_Width, config.m_VideoMode.m_Height );
		m_pMode->SetText( sz );
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFOptionsVideoPanel::~CTFOptionsVideoPanel()
{
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsVideoPanel::OnResetData()
{
	BaseClass::OnResetData();

	m_bRequireRestart = false;

	const MaterialSystem_Config_t &config = materials->GetCurrentConfigForVideoCard();

    // reset UI elements
    m_pWindowed->ActivateItem(config.Windowed() ? 1 : 0);
	//Msg("WINDOWED %i\n", config.Windowed());

	// reset gamma control
	m_pGammaSlider->SetEnabled(!config.Windowed());
	m_pGammaSlider->Reset();

    SetCurrentResolutionComboItem();


	//VIDEOADV
	ConVarRef mat_dxlevel("mat_dxlevel");
	ConVarRef r_rootlod("r_rootlod");
	ConVarRef mat_picmip("mat_picmip");
	ConVarRef mat_queue_mode("mat_queue_mode");
	ConVarRef mat_trilinear("mat_trilinear");
	ConVarRef mat_forceaniso("mat_forceaniso");
	ConVarRef mat_antialias("mat_antialias");
	ConVarRef mat_aaquality("mat_aaquality");
	ConVarRef mat_vsync("mat_vsync");
	ConVarRef r_flashlightdepthtexture("r_flashlightdepthtexture");
#ifndef _X360
	ConVarRef r_waterforceexpensive("r_waterforceexpensive");
#endif
	ConVarRef r_waterforcereflectentities("r_waterforcereflectentities");
	ConVarRef mat_reducefillrate("mat_reducefillrate");
	ConVarRef mat_hdr_level("mat_hdr_level");
	ConVarRef mat_colorcorrection("mat_colorcorrection");
	ConVarRef mat_motion_blur_enabled("mat_motion_blur_enabled");
	ConVarRef r_shadowrendertotexture("r_shadowrendertotexture");
	ConVarRef fov_desired("fov_desired");

	ResetDXLevelCombo();

	m_pModelDetail->ActivateItem(2 - clamp(r_rootlod.GetInt(), 0, 2));
	m_pTextureDetail->ActivateItem(2 - clamp(mat_picmip.GetInt(), -1, 2));

	if (r_flashlightdepthtexture.GetBool())		// If we're doing flashlight shadow depth texturing...
	{
		r_shadowrendertotexture.SetValue(1);		// ...be sure render to texture shadows are also on
		m_pShadowDetail->ActivateItem(2);
	}
	else if (r_shadowrendertotexture.GetBool())	// RTT shadows, but not shadow depth texturing
	{
		m_pShadowDetail->ActivateItem(1);
	}
	else	// Lowest shadow quality
	{
		m_pShadowDetail->ActivateItem(0);
	}

	m_pShaderDetail->ActivateItem(mat_reducefillrate.GetBool() ? 0 : 1);
	m_pHDR->ActivateItem(clamp(mat_hdr_level.GetInt(), 0, 2));

	switch (mat_forceaniso.GetInt())
	{
	case 2:
		m_pFilteringMode->ActivateItem(2);
		break;
	case 4:
		m_pFilteringMode->ActivateItem(3);
		break;
	case 8:
		m_pFilteringMode->ActivateItem(4);
		break;
	case 16:
		m_pFilteringMode->ActivateItem(5);
		break;
	case 0:
	default:
		if (mat_trilinear.GetBool())
		{
			m_pFilteringMode->ActivateItem(1);
		}
		else
		{
			m_pFilteringMode->ActivateItem(0);
		}
		break;
	}

	// Map convar to item on AA drop-down
	int nAASamples = mat_antialias.GetInt();
	int nAAQuality = mat_aaquality.GetInt();
	int nMSAAMode = FindMSAAMode(nAASamples, nAAQuality);
	m_pAntialiasingMode->ActivateItem(nMSAAMode);

#ifndef _X360
	if (r_waterforceexpensive.GetBool())
#endif
	{
		if (r_waterforcereflectentities.GetBool())
		{
			m_pWaterDetail->ActivateItem(2);
		}
		else
		{
			m_pWaterDetail->ActivateItem(1);
		}
	}
#ifndef _X360
	else
	{
		m_pWaterDetail->ActivateItem(0);
	}
#endif

	m_pVSync->ActivateItem(mat_vsync.GetInt());

	m_pColorCorrection->ActivateItem(mat_colorcorrection.GetInt());

	m_pMotionBlur->ActivateItem(mat_motion_blur_enabled.GetInt());

	m_pQueuedMode->ActivateItem(abs(mat_queue_mode.GetInt()));

	m_pFOVSlider->Reset();

	// get current hardware dx support level
	char dxVer[64];
	GetNameForDXLevel(mat_dxlevel.GetInt(), dxVer, sizeof(dxVer));
	SetControlString("dxlabel", dxVer);

	// get installed version
	char szVersion[64];
	szVersion[0] = 0;
	system()->GetRegistryString("HKEY_LOCAL_MACHINE\\Software\\Microsoft\\DirectX\\Version", szVersion, sizeof(szVersion));
	int os = 0, majorVersion = 0, minorVersion = 0, subVersion = 0;
	sscanf(szVersion, "%d.%d.%d.%d", &os, &majorVersion, &minorVersion, &subVersion);
	Q_snprintf(dxVer, sizeof(dxVer), "DirectX v%d.%d", majorVersion, minorVersion);
	SetControlString("dxinstalledlabel", dxVer);
}

void CTFOptionsVideoPanel::ResetDXLevelCombo()
{
	ConVarRef mat_dxlevel("mat_dxlevel");
	for (int i = 0; i < m_pDXLevel->GetItemCount(); i++)
	{
		KeyValues *kv = m_pDXLevel->GetItemUserData(i);
		if (kv->GetInt("dxlevel") == mat_dxlevel.GetInt())
		{
			m_pDXLevel->ActivateItem(i);
			break;
		}
	}

	// Reset HDR too
	if (m_pHDR->IsEnabled())
	{
		ConVarRef mat_hdr_level("mat_hdr_level");
		Assert(mat_hdr_level.IsValid());
		m_pHDR->ActivateItem(clamp(mat_hdr_level.GetInt(), 0, 2));
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFOptionsVideoPanel::SetCurrentResolutionComboItem()
{
	vmode_t *plist = NULL;
	int count = 0;
	gameuifuncs->GetVideoModes( &plist, &count );

	const MaterialSystem_Config_t &config = materials->GetCurrentConfigForVideoCard();

    int resolution = -1;
    for ( int i = 0; i < count; i++, plist++ )
	{
		if ( plist->width == config.m_VideoMode.m_Width && 
			 plist->height == config.m_VideoMode.m_Height )
		{
            resolution = i;
			break;
		}
	}

    if (resolution != -1)
	{
		char sz[256];
		GetResolutionName( plist, sz, sizeof(sz) );
        m_pMode->SetText(sz);
	}
}

//-----------------------------------------------------------------------------
// Purpose: restarts the game
//-----------------------------------------------------------------------------
void CTFOptionsVideoPanel::OnApplyChanges()
{
	BaseClass::OnApplyChanges();

	if ( RequiresRestart() )
	{
		INetChannelInfo *nci = engine->GetNetChannelInfo();
		if ( nci )
		{
			// Only retry if we're not running the server
			const char *pAddr = nci->GetAddress();
			if ( pAddr )
			{
				if ( Q_strncmp(pAddr,"127.0.0.1",9) && Q_strncmp(pAddr,"localhost",9) )
				{
					engine->ClientCmd_Unrestricted( "retry\n" );
				}
				else
				{
					engine->ClientCmd_Unrestricted( "disconnect\n" );
				}
			}
		}
	}


	// resolution
	char sz[256];
	//if ( m_nSelectedMode == -1 )
	//{
	//	m_pMode->GetText(sz, 256);
	//}
	//else
	//{
		m_pMode->GetItemText( m_nSelectedMode, sz, 256 );
	//}
	
	int width = 0, height = 0;
	sscanf(sz, "%i x %i", &width, &height);
	Msg("VIDEO %s\n", sz);
	// windowed
	bool windowed = (m_pWindowed->GetActiveItem() > 0) ? true : false;

	// make sure there is a change
	const MaterialSystem_Config_t &config = materials->GetCurrentConfigForVideoCard();
	if ( config.m_VideoMode.m_Width != width 
		|| config.m_VideoMode.m_Height != height
		|| config.Windowed() != windowed)
	{
		// set mode
		char szCmd[ 256 ];
		Q_snprintf( szCmd, sizeof( szCmd ), "mat_setvideomode %i %i %i\n", width, height, windowed ? 1 : 0 );
		Msg(szCmd);
		engine->ClientCmd_Unrestricted( szCmd );
	}

	if (config.m_VideoMode.m_Width != width || config.m_VideoMode.m_Height != height || config.Windowed() != windowed)
	{
		MAINMENU_ROOT->LaunchInvalidatePanelsLayout();
	}

	m_pGammaSlider->ApplyChanges();

	// apply changes
	engine->ClientCmd_Unrestricted( "mat_savechanges\n" );


	//VIDEOADV
	//if (!m_bUseChanges)
	//	return;

	ApplyChangesToConVar("mat_dxlevel", m_pDXLevel->GetActiveItemUserData()->GetInt("dxlevel"));
	ApplyChangesToConVar("r_rootlod", 2 - m_pModelDetail->GetActiveItem());
	ApplyChangesToConVar("mat_picmip", 2 - m_pTextureDetail->GetActiveItem());

	// reset everything tied to the filtering mode, then the switch sets the appropriate one
	ApplyChangesToConVar("mat_trilinear", false);
	ApplyChangesToConVar("mat_forceaniso", 1);
	switch (m_pFilteringMode->GetActiveItem())
	{
	case 0:
		break;
	case 1:
		ApplyChangesToConVar("mat_trilinear", true);
		break;
	case 2:
		ApplyChangesToConVar("mat_forceaniso", 2);
		break;
	case 3:
		ApplyChangesToConVar("mat_forceaniso", 4);
		break;
	case 4:
		ApplyChangesToConVar("mat_forceaniso", 8);
		break;
	case 5:
		ApplyChangesToConVar("mat_forceaniso", 16);
		break;
	}

	// Set the AA convars according to the menu item chosen
	int nActiveAAItem = m_pAntialiasingMode->GetActiveItem();
	ApplyChangesToConVar("mat_antialias", m_nAAModes[nActiveAAItem].m_nNumSamples);
	ApplyChangesToConVar("mat_aaquality", m_nAAModes[nActiveAAItem].m_nQualityLevel);

	if (m_pHDR->IsEnabled())
	{
		ConVarRef mat_hdr_level("mat_hdr_level");
		Assert(mat_hdr_level.IsValid());
		mat_hdr_level.SetValue(m_pHDR->GetActiveItem());
	}

	if (m_pShadowDetail->GetActiveItem() == 0)						// Blobby shadows
	{
		ApplyChangesToConVar("r_shadowrendertotexture", 0);			// Turn off RTT shadows
		ApplyChangesToConVar("r_flashlightdepthtexture", 0);			// Turn off shadow depth textures
	}
	else if (m_pShadowDetail->GetActiveItem() == 1)					// RTT shadows only
	{
		ApplyChangesToConVar("r_shadowrendertotexture", 1);			// Turn on RTT shadows
		ApplyChangesToConVar("r_flashlightdepthtexture", 0);			// Turn off shadow depth textures
	}
	else if (m_pShadowDetail->GetActiveItem() == 2)					// Shadow depth textures
	{
		ApplyChangesToConVar("r_shadowrendertotexture", 1);			// Turn on RTT shadows
		ApplyChangesToConVar("r_flashlightdepthtexture", 1);			// Turn on shadow depth textures
	}

	ApplyChangesToConVar("mat_reducefillrate", (m_pShaderDetail->GetActiveItem() > 0) ? 0 : 1);

	switch (m_pWaterDetail->GetActiveItem())
	{
	default:
	case 0:
#ifndef _X360
		ApplyChangesToConVar("r_waterforceexpensive", false);
#endif
		ApplyChangesToConVar("r_waterforcereflectentities", false);
		break;
	case 1:
#ifndef _X360
		ApplyChangesToConVar("r_waterforceexpensive", true);
#endif
		ApplyChangesToConVar("r_waterforcereflectentities", false);
		break;
	case 2:
#ifndef _X360
		ApplyChangesToConVar("r_waterforceexpensive", true);
#endif
		ApplyChangesToConVar("r_waterforcereflectentities", true);
		break;
	}

	m_pFOVSlider->ApplyChanges();

	ApplyChangesToConVar("mat_vsync", m_pVSync->GetActiveItem());

	ApplyChangesToConVar("mat_colorcorrection", m_pColorCorrection->GetActiveItem());

	ApplyChangesToConVar("mat_queue_mode", m_pQueuedMode->GetActiveItem() * -1);

	ApplyChangesToConVar("mat_motion_blur_enabled", m_pMotionBlur->GetActiveItem());
}

void CTFOptionsVideoPanel::MarkDefaultSettingsAsRecommended()
{
	// Pull in data from dxsupport.cfg database (includes fine-grained per-vendor/per-device config data)
	KeyValues *pKeyValues = new KeyValues("config");
	materials->GetRecommendedConfigurationInfo(0, pKeyValues);

	// Read individual values from keyvalues which came from dxsupport.cfg database
	int nSkipLevels = pKeyValues->GetInt("ConVar.mat_picmip", 0);
	int nAnisotropicLevel = pKeyValues->GetInt("ConVar.mat_forceaniso", 1);
	int nForceTrilinear = pKeyValues->GetInt("ConVar.mat_trilinear", 0);
	int nAASamples = pKeyValues->GetInt("ConVar.mat_antialias", 0);
	int nAAQuality = pKeyValues->GetInt("ConVar.mat_aaquality", 0);
	int nRenderToTextureShadows = pKeyValues->GetInt("ConVar.r_shadowrendertotexture", 0);
	int nShadowDepthTextureShadows = pKeyValues->GetInt("ConVar.r_flashlightdepthtexture", 0);
#ifndef _X360
	int nWaterUseRealtimeReflection = pKeyValues->GetInt("ConVar.r_waterforceexpensive", 0);
#endif
	int nWaterUseEntityReflection = pKeyValues->GetInt("ConVar.r_waterforcereflectentities", 0);
	int nMatVSync = pKeyValues->GetInt("ConVar.mat_vsync", 1);
	int nRootLOD = pKeyValues->GetInt("ConVar.r_rootlod", 0);
	int nReduceFillRate = pKeyValues->GetInt("ConVar.mat_reducefillrate", 0);
	int nDXLevel = pKeyValues->GetInt("ConVar.mat_dxlevel", 0);
	int nColorCorrection = pKeyValues->GetInt("ConVar.mat_colorcorrection", 0);
	int nMotionBlur = pKeyValues->GetInt("ConVar.mat_motion_blur_enabled", 0);

	int nQueuedMode;
	if (GetCPUInformation()->m_nPhysicalProcessors >= 2)
	{
		nQueuedMode = -1;
	}
	else
	{
		nQueuedMode = 0;
	}

	// Only recommend a dxlevel if there is more than one available
	if (m_pDXLevel->GetItemCount() > 1)
	{
		for (int i = 0; i < m_pDXLevel->GetItemCount(); i++)
		{
			KeyValues *kv = m_pDXLevel->GetItemUserData(i);
			if (kv->GetInt("dxlevel") == pKeyValues->GetInt("ConVar.mat_dxlevel"))
			{
				SetComboItemAsRecommended(m_pDXLevel, i);
				break;
			}
		}
	}

	SetComboItemAsRecommended(m_pModelDetail, 2 - nRootLOD);
	SetComboItemAsRecommended(m_pTextureDetail, 2 - nSkipLevels);

	switch (nAnisotropicLevel)
	{
	case 2:
		SetComboItemAsRecommended(m_pFilteringMode, 2);
		break;
	case 4:
		SetComboItemAsRecommended(m_pFilteringMode, 3);
		break;
	case 8:
		SetComboItemAsRecommended(m_pFilteringMode, 4);
		break;
	case 16:
		SetComboItemAsRecommended(m_pFilteringMode, 5);
		break;
	case 0:
	default:
		if (nForceTrilinear != 0)
		{
			SetComboItemAsRecommended(m_pFilteringMode, 1);
		}
		else
		{
			SetComboItemAsRecommended(m_pFilteringMode, 0);
		}
		break;
	}

	// Map desired mode to list item number
	int nMSAAMode = FindMSAAMode(nAASamples, nAAQuality);
	SetComboItemAsRecommended(m_pAntialiasingMode, nMSAAMode);

	if (nShadowDepthTextureShadows)
		SetComboItemAsRecommended(m_pShadowDetail, 2);	// Shadow depth mapping (in addition to RTT shadows)
	else if (nRenderToTextureShadows)
		SetComboItemAsRecommended(m_pShadowDetail, 1);	// RTT shadows
	else
		SetComboItemAsRecommended(m_pShadowDetail, 0);	// Blobbies

	SetComboItemAsRecommended(m_pShaderDetail, nReduceFillRate ? 0 : 1);

#ifndef _X360
	if (nWaterUseRealtimeReflection)
#endif
	{
		if (nWaterUseEntityReflection)
		{
			SetComboItemAsRecommended(m_pWaterDetail, 2);
		}
		else
		{
			SetComboItemAsRecommended(m_pWaterDetail, 1);
		}
	}
#ifndef _X360
	else
	{
		SetComboItemAsRecommended(m_pWaterDetail, 0);
	}
#endif

	SetComboItemAsRecommended(m_pVSync, nMatVSync != 0);

	SetComboItemAsRecommended(m_pHDR, nDXLevel >= 90 ? 2 : 0);

	SetComboItemAsRecommended(m_pColorCorrection, nColorCorrection);

	SetComboItemAsRecommended(m_pMotionBlur, nMotionBlur);

	SetComboItemAsRecommended(m_pQueuedMode, nQueuedMode);

	pKeyValues->deleteThis();
}

void CTFOptionsVideoPanel::ApplyChangesToConVar(const char *pConVarName, int value)
{
	Assert(cvar->FindVar(pConVarName));
	char szCmd[256];
	Q_snprintf(szCmd, sizeof(szCmd), "%s %d\n", pConVarName, value);
	engine->ClientCmd_Unrestricted(szCmd);
}

void CTFOptionsVideoPanel::SetComboItemAsRecommended(vgui::ComboBox *combo, int iItem)
{
	// get the item text
	wchar_t text[512];
	combo->GetItemText(iItem, text, sizeof(text));

	// append the recommended flag
	wchar_t newText[512];
	_snwprintf(newText, sizeof(newText) / sizeof(wchar_t), L"%s *", text);

	// reset
	combo->UpdateItem(iItem, newText, NULL);
}

int CTFOptionsVideoPanel::FindMSAAMode(int nAASamples, int nAAQuality)
{
	// Run through the AA Modes supported by the device
	for (int nAAMode = 0; nAAMode < m_nNumAAModes; nAAMode++)
	{
		// If we found the mode that matches what we're looking for, return the index
		if ((m_nAAModes[nAAMode].m_nNumSamples == nAASamples) && (m_nAAModes[nAAMode].m_nQualityLevel == nAAQuality))
		{
			return nAAMode;
		}
	}

	return 0;	// Didn't find what we're looking for, so no AA
}

//-----------------------------------------------------------------------------
// Purpose: enables apply button on data changing
//-----------------------------------------------------------------------------
void CTFOptionsVideoPanel::OnTextChanged(Panel *pPanel, const char *pszText)
{
	m_nSelectedMode = m_pMode->GetActiveItem();
	PrepareResolutionList();
}

//-----------------------------------------------------------------------------
// Purpose: enables apply button
//-----------------------------------------------------------------------------
void CTFOptionsVideoPanel::OnDataChanged()
{
	PostActionSignal(new KeyValues("ApplyButtonEnable"));
}

//-----------------------------------------------------------------------------
// Purpose: Checks to see if the changes requires a restart to take effect
//-----------------------------------------------------------------------------
bool CTFOptionsVideoPanel::RequiresRestart()
{
	// make sure there is a change
	return m_bRequireRestart;
}