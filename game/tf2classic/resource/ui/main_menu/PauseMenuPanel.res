"Resource/UI/main_menu/PauseMenuPanel.res"
{
	"CTFPauseMenuPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFPauseMenuPanel"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"f0"
		"tall"				"f0"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
	}
	
	"Logo"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Logo"
		"xpos"			"25"
		"ypos"			"25"	
		"zpos"			"3"		
		"wide"			"450"
		"tall"			"225"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../vgui/main_menu/TF2_Classic_Logo"
		"alpha"			"255"
		"scaleImage"	"1"	
	}	
	
	"MainMenuBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"MainMenuBG"
		"xpos"				"25"
		"ypos"				"185"
		"zpos"				"3"
		"wide"				"250"
		"tall"				"225"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuAdvButtonDepressed"
		"font"				"MenuMainTitle"
	}
		
	"ResumeButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"ResumeButton"
		"xpos"				"35"
		"ypos"				"195"
		"zpos"				"5"
		"wide"				"230"
		"tall"				"30"
		"visible"			"1"
		"enabled"			"1"
		"command"			"gamemenucommand ResumeGame"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_ResumeGame"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/icon_resume"
			"imagewidth"		"18"	
		}
	}		
	
	"MuteButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"MuteButton"
		"xpos"				"238"
		"ypos"				"198"
		"zpos"				"6"
		"wide"				"24"
		"tall"				"24"
		"visible"			"1"
		"enabled"			"1"
		"command"			"gamemenucommand openplayerlistdialog"
		
		"SubButton"
		{
			"labelText" 		""
			"tooltip" 			"Mute Players"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextArmed"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDefault"	
			"border_default"				"MainMenuAdvButtonDepressed"
			"border_depressed"				"MainMenuAdvButton"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_muted"
			"imagewidth"		"16"	
		}
	}	
	
	"ServerBrowserButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"ServerBrowserButton"
		"xpos"				"35"
		"ypos"				"230"
		"zpos"				"5"
		"wide"				"230"
		"tall"				"30"
		"visible"			"1"
		"enabled"			"1"
		"command"			"gamemenucommand OpenServerBrowser"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_FindServers"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_server_browser"
			"imagewidth"		"18"	
		}
	}	
	
	"MultiplayerGameButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"MultiplayerGameButton"
		"xpos"				"238"
		"ypos"				"233"
		"zpos"				"6"
		"wide"				"24"
		"tall"				"24"
		"visible"			"1"
		"enabled"			"1"
		"command"			"gamemenucommand OpenCreateMultiplayerGameDialog"
		
		"SubButton"
		{
			"labelText" 		""
			"tooltip" 			"Create Server"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextArmed"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDefault"	
			"border_default"				"MainMenuAdvButtonDepressed"
			"border_depressed"				"MainMenuAdvButton"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_create"
			"imagewidth"		"16"	
		}
	}
	
	"LoadoutButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"LoadoutButton"
		"xpos"				"35"
		"ypos"				"265"
		"zpos"				"5"
		"wide"				"230"
		"tall"				"30"
		"visible"			"1"
		"enabled"			"1"
		"command"			"newloadout"
		
		"SubButton"
		{
			"labelText" 		"Loadout"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_items"
			"imagewidth"		"18"	
		}
	}	
	
	"OptionsDialogButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"OptionsDialogButton"
		"xpos"				"35"
		"ypos"				"300"
		"zpos"				"5"
		"wide"				"230"
		"tall"				"30"
		"visible"			"1"
		"enabled"			"1"
		"command"			"newoptionsdialog"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Options"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_options"
			"imagewidth"		"18"	
		}
	}				
	
	"OptionsOldButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"OptionsOldButton"
		"xpos"				"238"
		"ypos"				"303"
		"zpos"				"6"
		"wide"				"24"
		"tall"				"24"
		"visible"			"1"
		"enabled"			"1"
		"command"			"gamemenucommand openoptionsdialog"
		
		"SubButton"
		{
			"labelText" 		""
			"tooltip" 			"Options"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextArmed"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDefault"	
			"border_default"				"MainMenuAdvButtonDepressed"
			"border_depressed"				"MainMenuAdvButton"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_steamworkshop"
			"imagewidth"		"16"	
		}
	}				
	
	"DisconnectButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"DisconnectButton"
		"xpos"				"35"
		"ypos"				"335"
		"zpos"				"5"
		"wide"				"230"
		"tall"				"30"
		"visible"			"1"
		"enabled"			"1"
		"command"			"gamemenucommand Disconnect"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Disconnect"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_view"
			"imagewidth"		"18"	
		}
	}	

	"QuitButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"QuitButton"
		"xpos"				"35"
		"ypos"				"370"
		"zpos"				"5"
		"wide"				"230"
		"tall"				"30"
		"visible"			"1"
		"enabled"			"1"
		"command"			"newquit"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Quit"
			"xshift" 			"10"
			"yshift" 			"0"
			"textAlignment"		"west"
			"font"				"HudFontSmallBold"
			"defaultFgColor_override"		"MainMenuTextDefault"
			"armedFgColor_override"			"MainMenuTextArmed"
			"depressedFgColor_override"		"MainMenuTextDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_quit"
			"imagewidth"		"18"	
		}
	}		
	
	"NotificationButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"NotificationButton"
		"xpos"				"278"
		"ypos"				"390"
		"zpos"				"6"
		"wide"				"20"
		"tall"				"20"
		"visible"			"0"
		"enabled"			"1"
		"bordervisible"		"1"	
		"command"			"shownotification"	
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Show notification"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_alert"
			"imagewidth"		"18"
		}
	}				
}
		