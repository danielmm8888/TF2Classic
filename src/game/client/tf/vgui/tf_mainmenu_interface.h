#ifndef TFMAINMENUINTERFACE_H
#define TFMAINMENUINTERFACE_H

#include <vgui/VGUI.h>

namespace vgui
{
	class Panel;
}

class IMainMenu
{
public:
	virtual void		Create(vgui::VPANEL parent) = 0;
	virtual vgui::VPANEL	GetPanel(void) = 0;
	virtual void		Destroy(void) = 0;
};

extern IMainMenu *MainMenu;

#endif // TFMAINMENUINTERFACE_H