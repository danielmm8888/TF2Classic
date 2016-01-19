#include "cbase.h"
#include "c_sdkversionchecker.h"
#include "script_parser.h"
#include "tier3/tier3.h"
#include "cdll_util.h"


//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
static C_SDKVersionChecker g_SDKVersionChecker;
C_SDKVersionChecker *GetSDKVersionChecker()
{
	return &g_SDKVersionChecker;
}

class C_SDKVersionParser : public C_ScriptParser
{
public:
	DECLARE_CLASS_GAMEROOT(C_SDKVersionParser, C_ScriptParser);

	C_SDKVersionParser()
	{
		Q_strncpy(sKey, "Unknown", sizeof(sKey));
	}

	void Parse(KeyValues *pKeyValuesData, bool bWildcard, const char *szFileWithoutEXT)
	{
		for (KeyValues *pData = pKeyValuesData->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey())
		{
			if (!Q_stricmp(pData->GetName(), "UserConfig"))
			{
				Q_strncpy(sKey, pData->GetString("BetaKey", ""), sizeof(sKey));
				if (!Q_stricmp(sKey, ""))
				{
					Q_strncpy(sKey, "Default", sizeof(sKey));
				}
			}
		}
	};

	void SetSDKVersionChecker(C_SDKVersionChecker *pChecker)
	{
		pSDKVersionChecker = pChecker;
	}

	const char* GetKey()
	{
		char *szResult = (char*)malloc(sizeof(sKey));
		Q_strncpy(szResult, sKey, sizeof(sKey));
		return szResult;
	}

private:
	char sKey[64];
	C_SDKVersionChecker *pSDKVersionChecker;
};
C_SDKVersionParser g_SDKVersionParser;

void PrintBranchName(const CCommand &args)
{
	Msg("SDK branch: %s\n", g_SDKVersionParser.GetKey());
}
ConCommand tf2c_getsdkbranch("tf2c_getsdkbranch", PrintBranchName);

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
C_SDKVersionChecker::C_SDKVersionChecker() : CAutoGameSystemPerFrame("C_SDKVersionChecker")
{
	if (!filesystem)
		return;

	m_bInited = false;
	Init();
}

C_SDKVersionChecker::~C_SDKVersionChecker()
{
}

//-----------------------------------------------------------------------------
// Purpose: Initializer
//-----------------------------------------------------------------------------
bool C_SDKVersionChecker::Init()
{
	if (!m_bInited)
	{
		g_SDKVersionParser.SetSDKVersionChecker(this);
		g_SDKVersionParser.InitParser("../../appmanifest_243750.acf", true, false, true);
		m_bInited = true;
	}

	return true;
}

const char* C_SDKVersionChecker::GetKey()
{
	return g_SDKVersionParser.GetKey();
}