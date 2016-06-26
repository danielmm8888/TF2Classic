"Resource/UI/main_menu/MainMenuPanel.res"
{
	"CTFMainMenuPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFMainMenuPanel"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"f0"
		"tall"				"f0"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
	}	
	
	"FakeBGImage"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"FakeBGImage"
		"xpos"			"0"
		"ypos"			"0"	
		"zpos"			"200"		
		"wide"			"f0"
		"tall"			"f0"
		"visible"		"0"
		"enabled"		"1"
		"image"			"../console/background03_widescreen"
		"alpha"			"255"
		"scaleImage"	"1"	
	}	
	
	"Logo"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Logo"
		"xpos"			"15"
		"ypos"			"15"	
		"zpos"			"3"		
		"wide"			"450"
		"tall"			"225"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../vgui/main_menu/TF2_Classic_Logo_NoCircle"
		"alpha"			"255"
		"scaleImage"	"1"	
	}	
	
	"LogoCircle"
	{
		"ControlName"	"CTFRotatingImagePanel"
		"fieldName"		"LogoCircle"
		"xpos"			"87"
		"ypos"			"29"
		"zpos"			"5"
		"wide"			"30"
		"tall"			"30"
		"image"			"vgui/main_menu/TF2_Classic_Logo_Circle"
		"visible"		"1"
		"enabled"		"1"
	}
	
	"MainMenuBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"MainMenuBG"
		"xpos"				"25"
		"ypos"				"135"
		"zpos"				"3"
		"wide"				"270"
		"tall"				"235"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuAdvButtonDepressed"
		"font"				"MenuMainTitle"
	}
	
	"BlogPanel"
	{
		"ControlName"		"CTFBlogPanel"
		"fieldName"			"BlogPanel"
		"xpos"				"r420"
		"ypos"				"135"
		"zpos"				"3"
		"wide"				"400"
		"tall"				"335"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuAdvButtonDepressed"	
	}
	
	"ServerlistPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"ServerlistPanel"
		"xpos"				"25"
		"ypos"				"375"
		"zpos"				"3"
		"wide"				"270"
		"tall"				"95"
		"visible"			"1"
		"enabled"			"1"
		"server_width"		"150"
		"players_width"		"25"
		"ping_width"		"17"
		"map_width"			"115"
		"scroll_width"		"3"
		"border"			"MainMenuAdvButtonDepressed"	
	}	
	
	"AvatarBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"AvatarBG"
		"xpos"				"35"
		"ypos"				"145"
		"zpos"				"5"
		"wide"				"50"
		"tall"				"50"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuBGBorderAlpha"
		"font"				"MenuMainTitle"
		"bgcolor_override"	"117 107 94 255"
	}
	
	"AvatarImage"
	{
		"ControlName"	"CAvatarImagePanel"
		"fieldName"		"AvatarImage"
		"xpos"			"40"
		"ypos"			"150"
		"zpos"			"6"
		"wide"			"40"
		"tall"			"40"
		"visible"		"1"
		"enabled"		"1"
		"image"			""
		"scaleImage"		"1"	
		"color_outline"		"52 48 45 255"
	}	
	
	"WelcomeBG"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"WelcomeBG"
		"xpos"				"40"
		"ypos"				"145"
		"zpos"				"4"
		"wide"				"245"
		"tall"				"50"
		"visible"			"1"
		"enabled"			"1"
		"border"			"MainMenuBGBorderAlpha"
		"font"				"MenuMainTitle"
		"bgcolor_override"	"36 33 32 255"
	}
	
	"WelcomeLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"WelcomeLabel"
		"xpos"				"90"
		"ypos"				"150"
		"zpos"				"6"
		"wide"				"190"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"alpha"				"255"
		"labelText"			"Welcome back,"
		"textAlignment"		"west"
		"font"				"HudFontMediumSmallBold"
		"fgcolor"			"AdvTextDefault"
	}
	
	"WelcomeLabel1"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"WelcomeLabel1"
		"xpos"				"90"
		"ypos"				"150"
		"zpos"				"5"
		"wide"				"190"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"alpha"				"0"
		"labelText"			"Welcome back,"
		"textAlignment"		"west"
		"font"				"HudFontMediumSmallBold_Shadow"
		"fgcolor"			"AdvTextDefault"
	}	
	
	"NicknameLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"NicknameLabel"
		"xpos"				"90"
		"ypos"				"170"
		"zpos"				"5"
		"wide"				"190"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%nickname%"
		"textAlignment"		"west"
		"font"				"HudFontMediumSmallBold"
		"fgcolor"			"AdvTextDefault"
	}
	
	"ServerBrowserButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"ServerBrowserButton"
		"xpos"					"35"
		"ypos"					"200"
		"zpos"					"5"
		"wide"					"250"
		"tall"					"30"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"		
		"command"				"gamemenucommand OpenServerBrowser"		
		
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
		"ControlName"			"CTFAdvButton"
		"fieldName"				"MultiplayerGameButton"
		"xpos"					"258"
		"ypos"					"203"
		"zpos"					"6"
		"wide"					"24"
		"tall"					"24"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"
		"command"				"gamemenucommand OpenCreateMultiplayerGameDialog"
		
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
		"ControlName"			"CTFAdvButton"
		"fieldName"				"LoadoutButton"
		"xpos"					"35"
		"ypos"					"235"
		"zpos"					"5"
		"wide"					"250"
		"tall"					"30"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"	
		"command"				"newloadout"
		
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
		"ControlName"			"CTFAdvButton"
		"fieldName"				"OptionsDialogButton"
		"xpos"					"35"
		"ypos"					"270"
		"zpos"					"5"
		"wide"					"250"
		"tall"					"30"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"	
		"command"				"newoptionsdialog"
		
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
		"ControlName"			"CTFAdvButton"
		"fieldName"				"OptionsOldButton"		
		"xpos"					"258"
		"ypos"					"273"
		"zpos"					"6"
		"wide"					"24"
		"tall"					"24"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"	
		"command"				"gamemenucommand openoptionsdialog"
		
		"SubButton"
		{
			"labelText" 		""
			"tooltip" 			"Options"
			"bordervisible"		"0"
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
	
	"QuitButton"
	{
		"ControlName"			"CTFAdvButton"
		"fieldName"				"QuitButton"
		"xpos"					"35"
		"ypos"					"305"
		"zpos"					"5"
		"wide"					"250"
		"tall"					"30"
		"visible"				"1"
		"enabled"				"1"
		"bordervisible"			"1"	
		"command"				"newquit"
		
		"SubButton"
		{
			"labelText" 		"#GameUI_GameMenu_Quit"
			"bordervisible"		"0"
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
	
	"StatsButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"StatsButton"
		"xpos"				"35"
		"ypos"				"340"
		"zpos"				"6"
		"wide"				"20"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"1"
		"labelText" 		""
		"command"			"newstats"
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Stats"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/main_menu/glyph_stats"
			"imagewidth"		"16"
		}
	}	
	

	"MusicToggleCheck"
	{
		"ControlName"		"CTFAdvCheckButton"
		"fieldName"			"MusicToggleCheck"
		"xpos"				"64"		//+19
		"ypos"				"340"
		"zpos"				"6"
		"wide"				"20"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"1"
		"labelText" 		""
		"command"			"tf2c_mainmenu_music"	
		"valuetrue"			"0"
		"valuefalse"		"1"		
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Music Toggle"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvLeftButtonDefault"
			"border_armed"		"AdvLeftButtonArmed"
			"border_depressed"	"AdvLeftButtonDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/main_menu/glyph_speaker"
			"imagecheck" 		"../vgui/main_menu/glyph_disabled"	
			"imagewidth"		"16"
		}
	}	
	
	"RandomMusicButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"RandomMusicButton"
		"xpos"				"83"
		"ypos"				"340"
		"zpos"				"5"
		"wide"				"20"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"1"
		"labelText" 		""
		"command"			"randommusic"			
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Random music"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvRightButtonDefault"
			"border_armed"		"AdvRightButtonArmed"
			"border_depressed"	"AdvRightButtonDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/main_menu/glyph_random"
			"imagewidth"		"16"
		}
	}	
	

	"BlogToggleCheck"
	{
		"ControlName"		"CTFAdvCheckButton"
		"fieldName"			"BlogToggleCheck"
		"xpos"				"112"		//+19
		"ypos"				"340"
		"zpos"				"6"
		"wide"				"20"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"1"
		"labelText" 		""
		"command"			"tf2c_mainmenu_showblog"	
		"valuetrue"			"0"
		"valuefalse"		"1"		
		
		"SubButton"
		{
			"labelText" 		""
			"bordervisible"		"0"
			"tooltip" 			"Blog panel Toggle"
			"textAlignment"		"center"
			"font"				"MenuSmallFont"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
		
		"SubImage"
		{
			"image" 			"../vgui/glyph_tv"
			"imagecheck" 		"../vgui/main_menu/glyph_disabled"	
			"imagewidth"		"16"
		}
	}	
	
	"NotificationButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"NotificationButton"
		"xpos"				"263"
		"ypos"				"340"
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
	
	"VersionLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"VersionLabel"
		"xpos"				"r525"
		"ypos"				"0"
		"zpos"				"5"
		"wide"				"520"
		"tall"				"40"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"VersionLabel"
		"textAlignment"		"north-east"
		"fgcolor"			"HudOffWhite"
		"font"				"MenuSmallFont"
	}	
	
	"TestButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"TestButton"
		"xpos"				"r100"
		"ypos"				"r50"
		"zpos"				"5"
		"wide"				"50"
		"tall"				"20"
		"visible"			"0"
		"enabled"			"1"
		"labelText" 		"Test"
		"bgvisible"			"1"
		"bordervisible"		"0"		
		"textAlignment"		"center"
		"command"			"testnotification"	
		"DefaultTextColor"		"MainMenuTextDefault"
		"ArmedTextColor"		"MainMenuTextArmed"
		"DepressedTextColor"	"MainMenuTextDepressed"		
		"font"				"HudFontSmallBold"
	}				
}
		