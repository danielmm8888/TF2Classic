"Resource/UI/main_menu/NotificationPanel.res"
{
	"CTFNotificationPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFNotificationPanel"
		"xpos"				"r300"
		"ypos"				"20"
		"wide"				"280"
		"tall"				"110"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"0"
		"enabled"			"1"
		"border"			"MainMenuHighlightBorder"
	}	
	
	"CloseButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"CloseButton"
		"xpos"				"250"
		"ypos"				"5"
		"zpos"				"1"
		"wide"				"24"
		"tall"				"24"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"vguicancel"	
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Close"
			"textAlignment"		"west"
			"font"				"MenuSmallFont"
			"border_default"	"MainMenuAdvButtonDepressed"
			"border_armed"		"MainMenuAdvButtonArmed"
			"border_depressed"	"MainMenuTextDefault"	
			"defaultFgColor_override"		"LightRed"
			"armedFgColor_override"			"LighterRed"
			"depressedFgColor_override"		"MainMenuTextDefault"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_close_x"
			"imagewidth"		"24"
		}
	}	
	
	"PrevButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"PrevButton"
		"xpos"				"220"
		"ypos"				"85"
		"zpos"				"1"
		"wide"				"24"
		"tall"				"24"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"Prev"	
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Prev"
			"textAlignment"		"west"
			"font"				"MenuSmallFont"
			"border_default"	"MainMenuAdvButtonDepressed"
			"border_armed"		"MainMenuAdvButtonArmed"
			"border_depressed"	"MainMenuTextDefault"	
			"defaultFgColor_override"		"TanDark"
			"armedFgColor_override"			"LightRed"
			"depressedFgColor_override"		"MainMenuTextDefault"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/blog_back_solid"
			"imagewidth"		"24"
		}
	}	
	
	"NextButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"NextButton"
		"xpos"				"250"
		"ypos"				"85"
		"zpos"				"1"
		"wide"				"24"
		"tall"				"24"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"Next"	
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Next"
			"textAlignment"		"west"
			"font"				"MenuSmallFont"
			"border_default"	"MainMenuAdvButtonDepressed"
			"border_armed"		"MainMenuAdvButtonArmed"
			"border_depressed"	"MainMenuTextDefault"	
			"defaultFgColor_override"		"TanDark"
			"armedFgColor_override"			"LightRed"
			"depressedFgColor_override"		"MainMenuTextDefault"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/blog_forward_solid"
			"imagewidth"		"24"
		}
	}	
	
	"RemoveButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"RemoveButton"
		"xpos"				"250"
		"ypos"				"30"
		"zpos"				"1"
		"wide"				"24"
		"tall"				"24"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"Remove"	
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Remove"
			"textAlignment"		"west"
			"font"				"MenuSmallFont"
			"border_default"	"MainMenuAdvButtonDepressed"
			"border_armed"		"MainMenuAdvButtonArmed"
			"border_depressed"	"MainMenuTextDefault"	
			"defaultFgColor_override"		"LightRed"
			"armedFgColor_override"			"LighterRed"
			"depressedFgColor_override"		"MainMenuTextDefault"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/icon_trash_on"
			"imagewidth"		"24"
		}
	}	
	
	"TitleLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"TitleLabel"
		"xpos"				"10"
		"ypos"				"5"
		"zpos"				"-1"
		"wide"				"225"
		"tall"				"30"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%title%"
		"textAlignment"		"west"
		"font"				"TeamMenuBold"
		"fgcolor"			"MainMenuTextDefault"
	}			
	
	"CountLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"CountLabel"
		"xpos"				"225"
		"ypos"				"5"
		"zpos"				"-1"
		"wide"				"200"
		"tall"				"100"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%count%"
		"textAlignment"		"north-east"
		"wrap"				"1"
		"font"				"GameUIButtonText"
		"fgcolor"			"MainMenuTextDefault"
	}	
	
	"MessageLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"MessageLabel"
		"xpos"				"10"
		"ypos"				"40"
		"zpos"				"-1"
		"wide"				"250"
		"tall"				"110"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%message%"
		"textAlignment"		"north-west"
		"wrap"				"1"
		"font"				"ControllerHintText"
		"fgcolor"			"MainMenuTextDefault"
	}		
}		