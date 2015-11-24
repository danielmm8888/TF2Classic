"Resource/UI/DeathMatchScoreBoard.res"
{
	"deathmatchscoreboard"
	{
		"ControlName"		"CTFFourTeamScoreBoardDialog"
		"fieldName"		"4scoreinfo"
		"xpos"			"cs-0.5"
		"xpos_lodef"	"34"
		"xpos_hidef"	"4"
		"ypos"			"c-150"
		"wide"			"480"
		"wide_lodef"	"560"
		"wide_hidef"	"620"
		"tall"			"300"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"avatar_width"		"58"
		"name_width"		"180"
		"status_width"		"15"	//15
		"nemesis_width"		"15"	//15
		"class_width"		"34"	//15
		"score_width"		"30"	//25
		"ping_width"		"35"
		"kills_width"		"30"
		"deaths_width"		"40"
		"killstreak_width"	"40"
		"border"		"TFThinLineBorder"
	}					
	"ShadedTitleBar"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ShadedTitleBar"
		"xpos"			"0"	//10
		"xpos_hidef"		"30"
		"ypos"			"0"
		"zpos"			"0"
		"wide"			"480"	//580
		"wide_lodef"		"539"	//539
		"wide_hidef"		"559"	//559
		"tall"			"54"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"	
		"drawcolor"		"255 250 255 255"
		"border"		"TFThinLineBorder"
	}								
	"RedTeamPlayerCount"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"RedTeamPlayerCount"
		"font"			"ScoreboardMediumAlt"
		"labelText"		"%redteamplayercount%"
		"textAlignment"		"west"
		"xpos"			"30"	//420
		"xpos_lodef"		"376"	//376
		"xpos_hidef"		"418"	//418
		"ypos"			"2"
		"wide"			"100"
		"tall"			"29"
		"autoResize"		"1"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}	
	"GamemodeLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"GamemodeLabel"
		"font"			"ScoreboardMedium"
		"labelText"		"#Gametype_Deathmatch"
		"textAlignment"		"west"
		"xpos"			"30"
		"ypos"			"28"	
		"zpos"			"3"
		"wide"			"165"
		"tall"			"20"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"fgcolor"		"236 227 203 255"
	}					
	"ServerLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"ServerLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%server%"
		"textAlignment"		"east"
		"xpos"			"140"	//11
		"xpos_hidef"		"31"	//31
		"ypos"			"5"	//60
		"ypos_lodef"		"62"	//62
		"zpos"			"4"
		"wide"			"300"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}	
	"MapName"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"mapname"
		"font"			"ScoreboardMedium"
		"labelText"		"%mapname%"
		"textAlignment"		"east"
		"xpos"			"280"
		"ypos"			"28"
		"zpos"			"3"
		"wide"			"165"
		"tall"			"20"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"fgcolor"		"236 227 203 255"
	}	
	"RedPlayerList"
	{
		"ControlName"		"SectionedListPanel"
		"fieldName"		"RedPlayerList"
		"xpos"			"25"	//305
		"ypos"			"53"		 //72
		"zpos"			"20"	//20
		"wide"			"435"	//290
		"tall"			"237"	//250
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"autoresize"		"0"
		"linespacing"		"20"
		"textcolor"		"white"
	}
	"Spectators"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Spectators"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%spectators%"
		"textAlignment"		"west"
		"xpos"			"30"	//115
		"xpos_hidef"		"135"	//135
		"ypos"			"320"	//327
		"zpos"			"4"
		"wide"			"280"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}			
	"RedScoreBG"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"RedScoreBG"
		"xpos"			"0"	//186	//296
		"xpos_lodef"		"276"	//276
		"xpos_hidef"		"306"	//306
		"ypos"			"0"	// 9
		"wide"			"289"	//100	//304
		"wide_lodef"		"130"	//80	//284
		"wide_hidef"		"140"	//90	//294
		"tall"			"50"	//71
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
		"image"			""
		"image_lodef"		"../hud/4score_panel_none_bg_lodef"
		"scaleImage"		"1"
		"fillcolor"		"0 0 0 255"
		"PaintBackgroundType"	"0"
	}				
	
	"WinPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"WinPanel"
		"xpos"			"20"
		"ypos"			"65"
		"zpos"			"25"
		"wide"			"440"
		"tall"			"270"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
		"border"		"MainMenuBGBorder"
	
		"Player1Model"
		{
			"ControlName"	"CModelPanel"
			"fieldName"		"Player1Model"
			
			"xpos"			"20"
			"ypos"			"20"
			"zpos"			"1"		
			"wide"			"400"
			"tall"			"260"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fov"			"55"
					
			"model"
			{
				"skin"	"0"
				"angles_x" "0"
				"angles_y" "180"
				"angles_z" "0"
				"origin_x" "190"
				"origin_y" "0"
				"origin_z" "-36"
				"frame_origin_x"	"0"
				"frame_origin_y"	"0"
				"frame_origin_z"	"0"
				"spotlight" "1"
			
				"modelname"		"models/player/merc_deathmatch.mdl"
				
				"animation"
				{
					"name"		"WINSEQ"
					"sequence"	"winscreen_first"
					"default"	"1"
				}			
			}
		}		
			
		"Player1Name"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player1Name"
			"font"			"HudFontMediumSmallBold"
			"labelText"		"%Player1Name%"
			"textAlignment"	"center"
			"xpos"			"130"
			"ypos"			"205"
			"zpos"			"3"
			"wide"			"180"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}		
			
		"Player1Kills"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player1Kills"
			"font"			"HudFontSmall"
			"labelText"		"%Player1Kills%"
			"textAlignment"	"center"
			"xpos"			"140"
			"ypos"			"220"
			"zpos"			"3"
			"wide"			"170"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}		
			
		"Player1Deaths"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player1Deaths"
			"font"			"HudFontSmall"
			"labelText"		"%Player1Deaths%"
			"textAlignment"	"center"
			"xpos"			"140"
			"ypos"			"232"
			"zpos"			"3"
			"wide"			"170"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}	
	
		"Player2Model"
		{
			"ControlName"	"CModelPanel"
			"fieldName"		"Player2Model"
			
			"xpos"			"20"
			"ypos"			"20"
			"zpos"			"1"		
			"wide"			"400"
			"tall"			"230"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fov"			"55"
						
			"model"
			{
				"skin"	"0"
				"angles_x" "0"
				"angles_y" "210"
				"angles_z" "0"
				"origin_x" "220"
				"origin_y" "75"
				"origin_z" "-31"
				"frame_origin_x"	"0"
				"frame_origin_y"	"0"
				"frame_origin_z"	"0"
				"spotlight" "1"
			
				"modelname"		"models/player/merc_deathmatch.mdl"
				
				"animation"
				{
					"name"		"WINSEQ"
					"sequence"	"winscreen_second"
					"default"	"1"
				}			
			}
		}		
			
		"Player2Name"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player2Name"
			"font"			"HudFontSmallBold"
			"labelText"		"%Player2Name%"
			"textAlignment"	"center"
			"xpos"			"0"
			"ypos"			"185"
			"zpos"			"3"
			"wide"			"170"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}		
			
		"Player2Kills"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player2Kills"
			"font"			"HudFontSmallest"
			"labelText"		"%Player2Kills%"
			"textAlignment"	"center"
			"xpos"			"0"
			"ypos"			"200"
			"zpos"			"3"
			"wide"			"170"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}		
			
		"Player2Deaths"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player2Deaths"
			"font"			"HudFontSmallest"
			"labelText"		"%Player2Deaths%"
			"textAlignment"	"center"
			"xpos"			"0"
			"ypos"			"212"
			"zpos"			"3"
			"wide"			"170"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}	
	
		"Player3Model"
		{
			"ControlName"	"CModelPanel"
			"fieldName"		"Player3Model"
			
			"xpos"			"20"
			"ypos"			"20"
			"zpos"			"1"		
			"wide"			"400"
			"tall"			"230"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fov"			"55"
					
			"model"
			{
				"skin"	"0"
				"angles_x" "0"
				"angles_y" "170"
				"angles_z" "0"
				"origin_x" "230"
				"origin_y" "-75"
				"origin_z" "-31"
				"frame_origin_x"	"0"
				"frame_origin_y"	"0"
				"frame_origin_z"	"0"
				"spotlight" "1"
			
				"modelname"		"models/player/merc_deathmatch.mdl"
				
				"animation"
				{
					"name"		"WINSEQ"
					"sequence"	"winscreen_third"
					"default"	"1"
				}			
			}
		}		
			
		"Player3Name"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player3Name"
			"font"			"HudFontSmallBold"
			"labelText"		"%Player3Name%"
			"textAlignment"	"center"
			"xpos"			"280"
			"ypos"			"185"
			"zpos"			"3"
			"wide"			"170"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}		
			
		"Player3Kills"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player3Kills"
			"font"			"HudFontSmallest"
			"labelText"		"%Player3Kills%"
			"textAlignment"	"center"
			"xpos"			"280"
			"ypos"			"200"
			"zpos"			"3"
			"wide"			"170"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}		
			
		"Player3Deaths"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"Player3Deaths"
			"font"			"HudFontSmallest"
			"labelText"		"%Player3Deaths%"
			"textAlignment"	"center"
			"xpos"			"280"
			"ypos"			"212"
			"zpos"			"3"
			"wide"			"170"
			"tall"			"20"
			"autoResize"	"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"fgcolor"		"236 227 203 255"
		}	
		
		
	}
}
