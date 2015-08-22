"Resource/UI/main_menu/NotificationPanel.res"
{
	"CTFNotificationPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFNotificationPanel"
		"xpos"				"r220"
		"ypos"				"20"
		"wide"				"200"
		"tall"				"150"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuHighlightBorder"
	}	
	
	"CloseButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"CloseButton"
		"xpos"				"175"
		"ypos"				"5"
		"zpos"				"1"
		"wide"				"16"
		"tall"				"16"
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
			"imagewidth"		"16"
		}
	}	
	
	"PrevButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"PrevButton"
		"xpos"				"150"
		"ypos"				"130"
		"zpos"				"1"
		"wide"				"16"
		"tall"				"16"
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
			"imagewidth"		"16"
		}
	}	
	
	"NextButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"NextButton"
		"xpos"				"175"
		"ypos"				"130"
		"zpos"				"1"
		"wide"				"16"
		"tall"				"16"
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
			"imagewidth"		"16"
		}
	}	
	
	"RemoveButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"RemoveButton"
		"xpos"				"175"
		"ypos"				"25"
		"zpos"				"1"
		"wide"				"16"
		"tall"				"16"
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
			"imagewidth"		"16"
		}
	}	
	
	"TitleLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"TitleLabel"
		"xpos"				"10"
		"ypos"				"5"
		"zpos"				"-1"
		"wide"				"200"
		"tall"				"25"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"SHET"
		"textAlignment"		"west"
		"font"				"HudFontBiggerBold"
		"fgcolor"			"MainMenuTextDefault"
	}			
	
	"CountLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"CountLabel"
		"xpos"				"155"
		"ypos"				"8"
		"zpos"				"-1"
		"wide"				"200"
		"tall"				"100"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			""
		"textAlignment"		"north-east"
		"wrap"				"1"
		"font"				"FontStorePromotion"
		"fgcolor"			"MainMenuTextDefault"
	}	
	
	"MessageLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"MessageLabel"
		"xpos"				"10"
		"ypos"				"35"
		"zpos"				"-1"
		"wide"				"200"
		"tall"				"100"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"blah-blah"
		"textAlignment"		"north-west"
		"wrap"				"1"
		"font"				"FontStorePromotion"
		"fgcolor"			"MainMenuTextDefault"
	}		
}		