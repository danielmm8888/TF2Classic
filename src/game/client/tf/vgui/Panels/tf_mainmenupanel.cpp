#include "cbase.h"
#include "tf_mainmenupanel.h"
#include "tf_mainmenu.h"
#include "controls/tf_advbutton.h"
#include "engine/IEngineSound.h"
#include "steam/steam_api.h"
#include "vgui_avatarimage.h"
#include "soundenvelope.h"

using namespace vgui;
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

//-----------------------------------------------------------------------------
// Purpose: Constructor
//-----------------------------------------------------------------------------
CTFMainMenuPanel::CTFMainMenuPanel(vgui::Panel* parent, const char *panelName) : CTFMenuPanelBase(parent, panelName)
{
	SetMainMenu(GetParent());
	Init();
}

//-----------------------------------------------------------------------------
// Purpose: Destructor
//-----------------------------------------------------------------------------
CTFMainMenuPanel::~CTFMainMenuPanel()
{

}

bool CTFMainMenuPanel::Init()
{
	BaseClass::Init();

	m_bMusicPlay = true;
	m_flActionThink = -1;
	m_flAnimationThink = -1;
	m_flMusicThink = -1;
	m_bAnimationIn = true;

	Q_strncpy(m_pzMusicLink, GetRandomMusic(), sizeof(m_pzMusicLink));

	bInMenu = true;
	return true;
}


void CTFMainMenuPanel::ApplySchemeSettings(vgui::IScheme *pScheme)
{
	BaseClass::ApplySchemeSettings(pScheme);

	LoadControlSettings("resource/UI/main_menu/MainMenuPanel.res");
	m_pVersionLabel = dynamic_cast<CExLabel *>(FindChildByName("VersionLabel"));
	m_pProfileAvatar = dynamic_cast<CAvatarImagePanel *>(FindChildByName("AvatarImage"));
	m_pNicknameButton = dynamic_cast<CTFAdvButton *>(FindChildByName("NicknameButton"));
	SetVersionLabel();
}	

void CTFMainMenuPanel::PerformLayout()
{
	BaseClass::PerformLayout();

	CSteamID m_SteamID = steamapicontext->SteamUser()->GetSteamID();
	m_pProfileAvatar->SetPlayer(m_SteamID, k_EAvatarSize64x64);
	m_pProfileAvatar->SetShouldDrawFriendIcon(false);
	m_pNicknameButton->SetText(steamapicontext->SteamFriends()->GetPersonaName());
	m_pNicknameButton->SetDisabled(true);
	DefaultLayout();

	//Msg("STEAMID: %i\n", m_SteamID.ConvertToUint64());``
	//Msg("STEAMName: %s\n", steamapicontext->SteamFriends()->GetPersonaName());
	//Msg("STEAMID: %i\n", steamapicontext->SteamUser()->GetSteamID().ConvertToUint64());
	//steamapicontext->SteamUtils()->GetImageRGBA(steamapicontext->SteamFriends()->GetLargeFriendAvatar(steamapicontext->SteamUser()->GetSteamID()));
	//Msg("STEAMID: %i %s\n", m_SteamID.ConvertToUint64(), steamapicontext->SteamFriends()->GetPersonaName());
};


void CTFMainMenuPanel::OnCommand(const char* command)
{
	if (!Q_strcmp(command, "newquit"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(QUIT_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsdialog"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSDIALOG_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsadv"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSADV_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsmouse"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSMOUSE_MENU);
	}
	else if (!Q_strcmp(command, "newoptionskeyboard"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSKEYBOARD_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsaudio"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSAUDIO_MENU);
	}
	else if (!Q_strcmp(command, "newoptionsvideo"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(OPTIONSVIDEO_MENU);
	}
	else if (!Q_strcmp(command, "newloadout"))
	{
		dynamic_cast<CTFMainMenu*>(GetMainMenu())->ShowPanel(LOADOUT_MENU);
	}
	else if (!Q_strcmp(command, "randommusic"))
	{
		m_bMusicPlay = false;
		Q_strncpy(m_pzMusicLink, GetRandomMusic(), sizeof(m_pzMusicLink));
		m_flMusicThink = gpGlobals->curtime + enginesound->GetSoundDuration(m_pzMusicLink);
	}
	else
	{
		BaseClass::OnCommand(command);
	}
}

void CTFMainMenuPanel::OnTick()
{
	BaseClass::OnTick();
	if (!bInGameLayout)
	{
		if (!m_bMusicPlay && m_pzMusicLink[0] != '\0')
		{
			m_bMusicPlay = true;
			enginesound->NotifyBeginMoviePlayback();
			//surface()->PlaySound(m_pzMusicLink);
			CSoundPatch* pNewSound = null;
			CLocalPlayerFilter filter;
			//DevMsg("JUKEBOX: Playing Track: %s%s.mp3\n", "*#music/_mp3/", m_pzMusicLink);
			pNewSound = CSoundEnvelopeController::GetController().SoundCreate(filter, 0, CHAN_STATIC, m_pzMusicLink, SNDLVL_NONE);
			CSoundEnvelopeController::GetController().Play(pNewSound, 0.0f, 100);
			CSoundEnvelopeController::GetController().SoundChangeVolume(pNewSound, 1.0f, 0.5f);

		}
		if (m_flMusicThink < gpGlobals->curtime && m_pzMusicLink[0] != '\0')
		{
			m_bMusicPlay = false;
			Q_strncpy(m_pzMusicLink, GetRandomMusic(), sizeof(m_pzMusicLink));
			m_flMusicThink = gpGlobals->curtime + enginesound->GetSoundDuration(m_pzMusicLink);
			//Msg("LENGTH %f\n", enginesound->GetSoundDuration(m_pzMusicLink));
		}
		if (m_pVersionLabel && m_flAnimationThink < gpGlobals->curtime)
		{
			//AnimationController::PublicValue_t newPos = { 20, 30, 0, 0 };
			float m_fAlpha = (m_bAnimationIn ? 50.0 : 100.0);
			vgui::GetAnimationController()->RunAnimationCommand(m_pVersionLabel, "Alpha", m_fAlpha, 0.0f, 0.25f, vgui::AnimationController::INTERPOLATOR_LINEAR);
			m_bAnimationIn = !m_bAnimationIn;
			m_flAnimationThink = gpGlobals->curtime + 0.25f;
		}
	}
};

void CTFMainMenuPanel::OnThink()
{
	BaseClass::OnThink();
};

void CTFMainMenuPanel::Show()
{
	BaseClass::Show();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 255, 0.0f, 0.5f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};

void CTFMainMenuPanel::Hide()
{
	BaseClass::Hide();
	vgui::GetAnimationController()->RunAnimationCommand(this, "Alpha", 0, 0.0f, 0.1f, vgui::AnimationController::INTERPOLATOR_LINEAR);
};


void CTFMainMenuPanel::DefaultLayout()
{
	BaseClass::DefaultLayout();
};

void CTFMainMenuPanel::GameLayout()
{
	BaseClass::GameLayout();

};

void CTFMainMenuPanel::PlayMusic()
{

}

void CTFMainMenuPanel::SetVersionLabel()
{
	if (m_pVersionLabel)
	{
		char verString[30];
		if (g_pFullFileSystem->FileExists("version.txt"))
		{
			FileHandle_t fh = filesystem->Open("version.txt", "r", "MOD");
			int file_len = filesystem->Size(fh);
			char* GameInfo = new char[file_len + 1];

			filesystem->Read((void*)GameInfo, file_len, fh);
			GameInfo[file_len] = 0; // null terminator

			filesystem->Close(fh);

			Q_snprintf(verString, sizeof(verString), "Version: %s", GameInfo + 8);

			delete[] GameInfo;
		}
		m_pVersionLabel->SetText(verString);
	}
};

char* CTFMainMenuPanel::GetRandomMusic()
{
	char* pszBasePath = "sound/ui/gamestartup";
	int iCount = 0;

	for (int i = 0; i < 27; i++)
	{
		char szPath[MAX_PATH];
		char szNumber[5];
		Q_snprintf(szNumber, sizeof(szNumber), "%d", iCount + 1);
		Q_strncpy(szPath, pszBasePath, sizeof(szPath));
		Q_strncat(szPath, szNumber, sizeof(szPath));
		Q_strncat(szPath, ".mp3", sizeof(szPath));
		if (!g_pFullFileSystem->FileExists(szPath))
		{
			if (iCount)
				break;
			else
				return "";
		}
		iCount++;
	}

	char* pszSoundPath = "ui/gamestartup";
	int iRand = rand() % iCount;
	char szPath[MAX_PATH];
	char szNumber[5];
	Q_snprintf(szNumber, sizeof(szNumber), "%d", iRand + 1);
	Q_strncpy(szPath, pszSoundPath, sizeof(szPath));
	Q_strncat(szPath, szNumber, sizeof(szPath));
	Q_strncat(szPath, ".mp3", sizeof(szPath));
	char *szResult = (char*)malloc(sizeof(szPath));
	Q_strncpy(szResult, szPath, sizeof(szPath));
	return szResult;
}




//-----------------------------------------------------------------------------
// Purpose: SPINNING SHIT
//-----------------------------------------------------------------------------
DECLARE_BUILD_FACTORY(CTFRotationPanel);

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
CTFRotationPanel::CTFRotationPanel(Panel *parent, const char *name) : CTFImagePanel(parent, name)
{
	///
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRotationPanel::ApplySettings(KeyValues *inResourceData)
{
	BaseClass::ApplySettings(inResourceData);
	Q_strncpy(pImage, inResourceData->GetString("imagerot", ""), sizeof(pImage));
	m_Material.Init(pImage, TEXTURE_GROUP_VGUI);
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
float CTFRotationPanel::GetAngleRotation(void)
{
	int _x, _y;
	surface()->SurfaceGetCursorPos(_x, _y);
	GetParent()->LocalToScreen(_x, _y);
	int x, y;
	GetPos(x, y);
	x += GetWide() / 2.0;
	y += GetTall() / 2.0;
	float deltaY = y - _y;
	float deltaX = x - _x;
	return atan2(deltaY, deltaX) * 180 / 3.1415;
}


//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
void CTFRotationPanel::Paint()
{
	IMaterial *pMaterial = m_Material;
	int x = 0;
	int y = 0;
	ipanel()->GetAbsPos(GetVPanel(), x, y);
	int nWidth = GetWide();
	int nHeight = GetTall();

	CMatRenderContextPtr pRenderContext(materials);
	pRenderContext->MatrixMode(MATERIAL_MODEL);
	pRenderContext->PushMatrix();

	VMatrix panelRotation;
	panelRotation.Identity();
	MatrixBuildRotationAboutAxis(panelRotation, Vector(0, 0, 1), GetAngleRotation());
	//	MatrixRotate( panelRotation, Vector( 1, 0, 0 ), 5 );
	panelRotation.SetTranslation(Vector(x + nWidth / 2, y + nHeight / 2, 0));
	pRenderContext->LoadMatrix(panelRotation);

	IMesh *pMesh = pRenderContext->GetDynamicMesh(true, NULL, NULL, pMaterial);

	CMeshBuilder meshBuilder;
	meshBuilder.Begin(pMesh, MATERIAL_QUADS, 1);

	meshBuilder.TexCoord2f(0, 0, 0);
	meshBuilder.Position3f(-nWidth / 2, -nHeight / 2, 0);
	meshBuilder.Color4ub(255, 255, 255, 255);
	meshBuilder.AdvanceVertex();

	meshBuilder.TexCoord2f(0, 1, 0);
	meshBuilder.Position3f(nWidth / 2, -nHeight / 2, 0);
	meshBuilder.Color4ub(255, 255, 255, 255);
	meshBuilder.AdvanceVertex();

	meshBuilder.TexCoord2f(0, 1, 1);
	meshBuilder.Position3f(nWidth / 2, nHeight / 2, 0);
	meshBuilder.Color4ub(255, 255, 255, 255);
	meshBuilder.AdvanceVertex();

	meshBuilder.TexCoord2f(0, 0, 1);
	meshBuilder.Position3f(-nWidth / 2, nHeight / 2, 0);
	meshBuilder.Color4ub(255, 255, 255, 255);
	meshBuilder.AdvanceVertex();

	meshBuilder.End();

	pMesh->Draw();
	pRenderContext->PopMatrix();
}


