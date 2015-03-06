#include <vgui/IVGui.h>
#include <vgui_controls/TextEntry.h>
#include "ienginevgui.h"
#include <filesystem.h>

class CVerNumLabel : public vgui::Label
{
	DECLARE_CLASS_SIMPLE(CVerNumLabel, vgui::Label);
	CVerNumLabel(vgui::VPANEL parent);
};

class IVerNumLabel
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual void		Destroy(void) = 0;
};

extern IVerNumLabel* verPanel;
