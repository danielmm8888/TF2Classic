#include "cbase.h"
#include "ver_panel.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"

class CVerNumLabelInterface : public IVerNumLabel
{
private:
	CVerNumLabel *VerNumLabel;
public:
	CVerNumLabelInterface()
	{
		VerNumLabel = NULL;
	}
	void Create(vgui::VPANEL parent)
	{
		VerNumLabel = new CVerNumLabel(parent);
	}
	void Destroy()
	{
		if (VerNumLabel)
		{
			VerNumLabel->SetParent((vgui::Panel *)NULL);
			delete VerNumLabel;
		}
	}
};
static CVerNumLabelInterface g_MyPanel;
IVerNumLabel* verPanel = (IVerNumLabel*)&g_MyPanel;

CVerNumLabel::CVerNumLabel(vgui::VPANEL parent)
	: BaseClass(NULL, "VerNum", L"")
{
	vgui::VPANEL pParent = enginevgui->GetPanel(PANEL_GAMEUIDLL);
	SetParent(pParent);

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
	SetText(verString);
	SetWide(150);
	SetProportional(false);
	SetVisible(true);

	SetScheme(vgui::scheme()->LoadSchemeFromFile("resource/SourceScheme.res", "SourceScheme"));
}
