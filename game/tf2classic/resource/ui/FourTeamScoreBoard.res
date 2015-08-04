"Resource/UI/FourTeamScoreBoard.res"
{
	"fourteamscoreboard"
	{
		"ControlName"		"CTFFourTeamScoreBoardDialog"
		"fieldName"		"4scoreinfo"
		"xpos"			"cs-0.5"	//c-300
		"xpos_lodef"		"34"	//34
		"xpos_hidef"		"4"	//4
		"ypos"			"31"	//31
		"wide"			"750"	//750	//600
		"wide_lodef"		"2"		//560
		"wide_hidef"		"2"		//620
		"tall"			"418"	//418
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"avatar_width"		"57"
		"name_width"		"70"
		"status_width"		"15"	//15
		"nemesis_width"		"15"	//15
		"class_width"		"34"	//15
		"score_width"		"14"	//25
		"score_width_lodef"	"35"
		"score_width_hidef"	"50"
		"ping_width"		"20"
	}
	"BlueScoreBG"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"BlueScoreBG"
		"xpos"			"0"	//-6
		"xpos_hidef"		"18"	// 18
		"ypos"			"0"	// 9
		"wide"			"188"	//185	//304
		"wide_lodef"		"130"	//130	//284
		"wide_hidef"		"140"	//140	//294
		"tall"			"50"	//71
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../hud/4score_panel_blue_bg"
		"image_lodef"		"../hud/4score_panel_blue_bg_lodef"
		"scaleImage"		"1"
		"fillcolor"		"0 0 0 255"
		"PaintBackgroundType"	"0"
	}
	"RedScoreBG"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"RedScoreBG"
		"xpos"			"188"	//186	//296
		"xpos_lodef"		"276"	//276
		"xpos_hidef"		"306"	//306
		"ypos"			"0"	// 9
		"wide"			"189"	//100	//304
		"wide_lodef"		"130"	//80	//284
		"wide_hidef"		"140"	//90	//294
		"tall"			"50"	//71
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../hud/4score_panel_red_bg"
		"image_lodef"		"../hud/4score_panel_red_bg_lodef"
		"scaleImage"		"1"
		"fillcolor"		"0 0 0 255"
		"PaintBackgroundType"	"0"
	}
	"GreenScoreBG"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"GreenScoreBG"
		"xpos"			"376"
		"xpos_hidef"		"18"
		"ypos"			"0"	// 9
		"wide"			"188"	//100	//304
		"wide_lodef"		"130"	//80	//284
		"wide_hidef"		"140"	//90	//294
		"tall"			"50"	//71
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../hud/4score_panel_green_bg"
		"image_lodef"		"../hud/4score_panel_green_bg_lodef"
		"scaleImage"		"1"
		"fillcolor"		"0 0 0 255"
		"PaintBackgroundType"	"0"
	}
	"YellowScoreBG"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"YellowScoreBG"
		"xpos"			"564" 	//296
		"xpos_lodef"		"276"	//276
		"xpos_hidef"		"306"	//306
		"ypos"			"0"	// 9
		"wide"			"188"	//100	//304
		"wide_lodef"		"130"	//80	//284
		"wide_hidef"		"140"	//90	//294
		"tall"			"50"	//71
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../hud/4score_panel_yellow_bg"
		"image_lodef"		"../hud/4score_panel_yellow_bg_lodef"
		"scaleImage"		"1"
		"fillcolor"		"0 0 0 255"
		"PaintBackgroundType"	"0"
	}
	"MainBG"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"MainBG"
		"xpos"			"0"	//-6
		"xpos_hidef"		"14"
		"ypos"			"50"	//63
		"zpos"			"0"
		"wide"			"761"	//600
		"wide_lodef"		"710"	//560
		"wide_hidef"		"770"	//620
		"tall"			"570"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"0" //1
		"image"			"../hud/4score_panel_black_bg"
		"image_lodef"		"../hud/4score_panel_black_bg_lodef"
		"scaleImage"		"1"
	}
	"ListColumn01"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ListColumn01"
		"xpos"			"0"	//-6
		"xpos_hidef"		"14"
		"ypos"			"50"	//63
		"zpos"			"0"
		"wide"			"188"	//600
		"wide_lodef"		"710"	//560
		"wide_hidef"		"770"	//620
		"tall"			"570"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"	
		"fillcolor"		"0 0 0 153"
		"PaintBackgroundType"	"0"
	}		
	"ListColumn02"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ListColumn01"
		"xpos"			"188"	//-6
		"xpos_hidef"		"14"
		"ypos"			"50"	//63
		"zpos"			"0"
		"wide"			"189"	//600
		"wide_lodef"		"710"	//560
		"wide_hidef"		"770"	//620
		"tall"			"570"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"	
		"fillcolor"		"0 0 0 200"
		"PaintBackgroundType"	"0"
	}	
	"ListColumn03"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ListColumn03"
		"xpos"			"376"	//-6
		"xpos_hidef"		"14"
		"ypos"			"50"	//63
		"zpos"			"0"
		"wide"			"189"	//600
		"wide_lodef"		"710"	//560
		"wide_hidef"		"770"	//620
		"tall"			"570"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"	
		"fillcolor"		"0 0 0 153"
		"PaintBackgroundType"	"0"
	}	
	"ListColumn04"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ListColumn04"
		"xpos"			"564"	//-6
		"xpos_hidef"		"14"
		"ypos"			"50"	//63
		"zpos"			"0"
		"wide"			"188"	//600
		"wide_lodef"		"710"	//560
		"wide_hidef"		"770"	//620
		"tall"			"570"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"	
		"fillcolor"		"0 0 0 200"
		"PaintBackgroundType"	"0"
	}						
	"BlueTeamLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"BlueTeamLabel"
		"font"			"ScoreboardTeamNameLarge"
		"labelText"		"%blueteamname%"
		"textAlignment"		"west"
		"xpos"			"100"	//10
		"xpos_hidef"		"40"	//40
		"ypos"			"10"	//23
		"wide"			"70"	//70
		"tall"			"34"	//34
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}							
	"BlueTeamScore"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"BlueTeamScore"
		"font"			"ScoreboardTeamScoreAlt"
		"labelText"		"%blueteamscore%"
		"textAlignment"		"west"
		"xpos"			"25"
		"xpos_lodef"		"170"
		"xpos_hidef"		"190"
		"ypos"			"-2"
		"zpos"			"4"
		"wide"			"100"
		"tall"			"45"
		"tall_hidef"		"75"
		"tall_lodef"		"75"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}					
	"BlueTeamPlayerCount"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"BlueTeamPlayerCount"
		"font"			"ScoreboardMediumAlt"
		"labelText"		"%blueteamplayercount%"
		"textAlignment"		"west"
		"xpos"			"12"
		"xpos_hidef"		"100"
		"ypos"			"26"
		"wide"			"100"
		"tall"			"29"
		"autoResize"		"1"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}							
	"RedTeamLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"RedTeamLabel"
		"font"			"ScoreboardTeamNameLarge"
		"labelText"		"%redteamname%"
		"textAlignment"		"west"
		"xpos"			"285"
		"xpos_lodef"	"446"
		"xpos_hidef"	"488"
		"ypos"			"10"	//23
		"wide"			"70"	//70
		"tall"			"34"	//34
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}							
	"RedTeamScore"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"RedTeamScore"
		"font"			"ScoreboardTeamScoreAlt"
		"labelText"		"%redteamscore%"
		"textAlignment"		"west"
		"xpos"			"210" //310
		"xpos_lodef"		"290"		//290
		"xpos_hidef"		"330"		//330
		"ypos"			"-2"
		"zpos"			"4"
		"wide"			"100"
		"tall"			"45"
		"tall_hidef"		"75"
		"tall_lodef"		"75"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"RedTeamPlayerCount"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"RedTeamPlayerCount"
		"font"			"ScoreboardMediumAlt"
		"labelText"		"%redteamplayercount%"
		"textAlignment"		"west"
		"xpos"			"197"	//420
		"xpos_lodef"		"376"	//376
		"xpos_hidef"		"418"	//418
		"ypos"			"26"
		"wide"			"100"
		"tall"			"29"
		"autoResize"		"1"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}		
	"GreenTeamLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"GreenTeamLabel"
		"font"			"ScoreboardTeamNameLarge"
		"labelText"		"%greenteamname%"
		"textAlignment"		"east"
		"xpos"			"470"	//10
		"xpos_hidef"		"40"	//40
		"ypos"			"10"	//23
		"wide"			"70"	//70
		"tall"			"34"	//34
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}							
	"GreenTeamScore"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"GreenTeamScore"
		"font"			"ScoreboardTeamScoreAlt"
		"labelText"		"%greenteamscore%"
		"textAlignment"		"west"
		"xpos"			"395"	//190
		"xpos_lodef"		"170"
		"xpos_hidef"		"190"
		"ypos"			"-2"
		"zpos"			"4"
		"wide"			"100"
		"tall"			"45"
		"tall_hidef"		"75"
		"tall_lodef"		"75"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"GreenTeamPlayerCount"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"GreenTeamPlayerCount"
		"font"			"ScoreboardMediumAlt"
		"labelText"		"%greenteamplayercount%"
		"textAlignment"		"west"
		"xpos"			"382"	//420
		"xpos_lodef"		"376"	//376
		"xpos_hidef"		"418"	//418
		"ypos"			"26"
		"wide"			"100"
		"tall"			"29"
		"autoResize"		"1"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}	
	"YellowTeamLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"YellowTeamLabel"
		"font"			"ScoreboardTeamNameLarge"
		"labelText"		"%yellowteamname%"
		"textAlignment"		"east"
		"xpos"			"655"	//10
		"xpos_hidef"		"40"	//40
		"ypos"			"10"	//23
		"wide"			"70"	//70
		"tall"			"34"	//34
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}							
	"YellowTeamScore"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"YellowTeamScore"
		"font"			"ScoreboardTeamScoreAlt"
		"labelText"		"%yellowteamscore%"
		"textAlignment"		"west"
		"xpos"			"580"	//190
		"xpos_lodef"		"170"
		"xpos_hidef"		"190"
		"ypos"			"-2"
		"zpos"			"4"
		"wide"			"100"
		"tall"			"45"
		"tall_hidef"		"75"
		"tall_lodef"		"75"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"YellowTeamPlayerCount"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"YellowTeamPlayerCount"
		"font"			"ScoreboardMediumAlt"
		"labelText"		"%yellowteamplayercount%"
		"textAlignment"		"west"
		"xpos"			"567"	//420
		"xpos_lodef"		"376"	//376
		"xpos_hidef"		"418"	//418
		"ypos"			"26"
		"wide"			"100"
		"tall"			"29"
		"autoResize"		"1"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"ServerLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"ServerLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%server%"
		"textAlignment"		"west"
		"xpos"			"580"	//11
		"xpos_hidef"		"31"	//31
		"ypos"			"364"	//60
		"ypos_lodef"		"62"	//62
		"zpos"			"4"
		"wide"			"300"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}							
	"BluePlayerList"
	{
		"ControlName"		"SectionedListPanel"
		"fieldName"		"BluePlayerList"
		"xpos"			"-2"	//5
		"xpos_lodef"		"10"	//10
		"xpos_hidef"		"30"	//30
		"ypos"			"48"	//72
		"zpos"			"20"	//20
		"wide"			"192"	//290
		"wide_lodef"		"267"	//267
		"wide_hidef"		"277"	//277
		"tall"			"250"	//250
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"autoresize"		"3"
		"linespacing"		"20"
		"fgcolor"		"blue"
	}
	"RedPlayerList"
	{
		"ControlName"		"SectionedListPanel"
		"fieldName"		"RedPlayerList"
		"xpos"			"186"	//305
		"xpos_lodef"		"284"	//284
		"xpos_hidef"		"314"	//314
		"ypos"			"48"	//72
		"zpos"			"20"	//20
		"wide"			"192"	//290
		"wide_lodef"		"267"	//267
		"wide_hidef"		"277"	//277
		"tall"			"250"	//250
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"autoresize"		"3"
		"linespacing"		"20"
		"textcolor"		"red"
	}
	"GreenPlayerList"
	{
		"ControlName"		"SectionedListPanel"
		"fieldName"		"GreenPlayerList"
		"xpos"			"374"
		"xpos_lodef"		"10"
		"xpos_hidef"		"30"
		"ypos"			"48"	//72
		"zpos"			"20"	//20
		"wide"			"192"	//290
		"wide_lodef"		"267"	//267
		"wide_hidef"		"277"	//277
		"tall"			"250"	//250
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"autoresize"		"3"
		"linespacing"		"20"
		"fgcolor"		"green"
	}
	"YellowPlayerList"
	{
		"ControlName"		"SectionedListPanel"
		"fieldName"		"YellowPlayerList"
		"xpos"			"563"
		"xpos_lodef"		"284"
		"xpos_hidef"		"314"
		"ypos"			"48"	//72
		"zpos"			"20"	//20
		"wide"			"192"	//290
		"wide_lodef"		"267"	//267
		"wide_hidef"		"277"	//277
		"tall"			"250"	//250
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"
		"autoresize"		"3"
		"linespacing"		"20"
		"textcolor"		"yellow"
	}
	"VerticalLine"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"VerticalLine"
		"xpos"			"184"	//299
		"xpos_lodef"		"279"	//279
		"xpos_hidef"		"309"	//309
		"ypos"			"50"	//70
		"zpos"			"50"	//2
		"wide"			"2"	//2
		"tall"			"262"	//262
		"tall_lodef"		"206"	//206
		"tall_hidef"		"212"	//212
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
		"tabPosition"		"0"	
		"fillcolor"		"0 0 0 255"	//0 0 0 153
		"PaintBackgroundType"	"0"
	}
	"Spectators"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Spectators"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%spectators%"
		"textAlignment"		"west"
		"xpos"			"460"	//115
		"xpos_hidef"		"135"	//135
		"ypos"			"395"	//327
		"zpos"			"4"
		"wide"			"280"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"ShadedBar"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ShadedBar"
		"xpos"			"0"	//10
		"xpos_hidef"		"30"
		"ypos"			"342"
		"zpos"			"2"
		"wide"			"750"	//580
		"wide_lodef"		"539"	//539
		"wide_hidef"		"559"	//559
		"tall"			"80"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"	
		"fillcolor"		"40 40 40 255"
		"PaintBackgroundType"	"0"
	}
	"ClassImage"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ClassImage"
		"xpos"			"5"
		"xpos_lodef"		"12"
		"ypos"			"326"
		"zpos"			"3"
		"wide"			"92"
		"tall"			"92"
		"visible"		"1"
		"enabled"		"1"
		"image"			"../hud/class_scoutred"
		"scaleImage"		"1"	
	}
	"PlayerNameLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"PlayerNameLabel"
		"font"			"ScoreboardMedium"
		"labelText"		"%playername%"
		"textAlignment"		"west"
		"xpos"			"100"
		"xpos_lodef"		"105"
		"ypos"			"347"
		"zpos"			"3"
		"wide"			"325"
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
		"xpos"			"500"
		"ypos"			"385"
		"zpos"			"3"
		"wide"			"165"
		"tall"			"20"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"fgcolor"		"236 227 203 255"
	}							
	"HorizontalLine"
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"HorizontalLine"
		"xpos"			"100"
		"xpos_lodef"		"105"
		"ypos"			"367"
		"zpos"			"3"
		"wide"			"650"
		"wide_lodef"		"434"
		"wide_hidef"		"464"
		"tall"			"2"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"	
		"fillcolor"		"127 127 127 153"
		"PaintBackgroundType"	"0"
	}
	"PlayerScoreLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"PlayerScoreLabel"
		"font"			"ScoreboardMedium"
		"labelText"		"%playerscore%"
		"textAlignment"		"east"
		"xpos"			"600"	//440
		"xpos_lodef"		"399"	//399
		"xpos_hidef"		"439"	//439
		"ypos"			"347"
		"zpos"			"3"
		"wide"			"140"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}
	"KillsLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"KillsLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_KillsLabel"
		"textAlignment"		"east"
		"xpos"			"80"
		"ypos"			"365"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"DeathsLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"DeathsLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_DeathsLabel"
		"textAlignment"		"east"
		"xpos"			"80"
		"ypos"			"375"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"AssistsLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"AssistsLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_AssistsLabel"
		"textAlignment"		"east"
		"xpos"			"80"
		"ypos"			"385"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}
	"DestructionLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"DestructionLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_DestructionLabel"
		"textAlignment"		"east"
		"xpos"			"80"
		"ypos"			"395"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}												
	"Kills"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Kills"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%kills%"
		"textAlignment"		"west"
		"xpos"			"180"
		"ypos"			"365"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Deaths"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Deaths"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%deaths%"
		"textAlignment"		"west"
		"xpos"			"180"
		"ypos"			"375"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Assists"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Assists"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%assists%"
		"textAlignment"		"west"
		"xpos"			"180"
		"ypos"			"385"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Destruction"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Destruction"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%destruction%"
		"textAlignment"		"west"
		"xpos"			"180"
		"ypos"			"395"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"CapturesLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"CapturesLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_CapturesLabel"
		"textAlignment"		"east"
		"xpos"			"200"
		"ypos"			"365"
		"zpos"			"3"
		"wide"			"100"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"DefensesLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"DefensesLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_DefensesLabel"
		"textAlignment"		"east"
		"xpos"			"200"
		"ypos"			"375"
		"zpos"			"3"
		"wide"			"100"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"DominationLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"DominationLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_DominationLabel"
		"textAlignment"		"east"
		"xpos"			"200"
		"ypos"			"385"
		"zpos"			"3"
		"wide"			"100"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"RevengeLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"RevengeLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_RevengeLabel"
		"textAlignment"		"east"
		"xpos"			"200"
		"ypos"			"395"
		"zpos"			"3"
		"wide"			"100"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Captures"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Captures"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%captures%"
		"textAlignment"		"west"
		"xpos"			"305"
		"ypos"			"365"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Defenses"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Defenses"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%defenses%"
		"textAlignment"		"west"
		"xpos"			"305"
		"ypos"			"375"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Domination"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Domination"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%dominations%"
		"textAlignment"		"west"
		"xpos"			"305"
		"ypos"			"385"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Revenge"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Revenge"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%Revenge%"
		"textAlignment"		"west"
		"xpos"			"305"
		"ypos"			"395"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"HealingLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"HealingLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_HealingLabel"
		"textAlignment"		"east"
		"xpos"			"326"
		"ypos"			"395"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"InvulnLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"InvulnLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_InvulnLabel"
		"textAlignment"		"east"
		"xpos"			"326"
		"ypos"			"365"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"TeleportsLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"TeleportsLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_TeleportsLabel"
		"textAlignment"		"east"
		"xpos"			"326"
		"ypos"			"385"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"HeadshotsLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"HeadshotsLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_HeadshotsLabel"
		"textAlignment"		"east"
		"xpos"			"326"
		"ypos"			"375"
		"zpos"			"3"
		"wide"			"95"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Healing"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Healing"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%healing%"
		"textAlignment"		"west"
		"xpos"			"425"
		"ypos"			"395"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Invuln"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Invuln"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%invulns%"
		"textAlignment"		"west"
		"xpos"			"425"
		"ypos"			"365"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Teleports"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Teleports"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%teleports%"
		"textAlignment"		"west"
		"xpos"			"425"
		"ypos"			"385"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"Headshots"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Headshots"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%headshots%"
		"textAlignment"		"west"
		"xpos"			"425"
		"ypos"			"375"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}						
	"BackstabsLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"BackstabsLabel"
		"font"			"ScoreboardVerySmall"
		"labelText"		"#TF_ScoreBoard_BackstabsLabel"
		"textAlignment"		"north-east"
		"xpos"			"445"
		"ypos"			"370"
		"zpos"			"3"
		"wide"			"110"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}
	"Backstabs"
	{
		"ControlName"		"CExLabel"
		"fieldName"		"Backstabs"
		"font"			"ScoreboardVerySmall"
		"labelText"		"%backstabs%"
		"textAlignment"		"north-west"
		"xpos"			"560"
		"ypos"			"370"
		"zpos"			"3"
		"wide"			"35"
		"tall"			"20"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
	}		
	
	"ButtonLegendBG"		[$X360]
	{
		"ControlName"		"ImagePanel"
		"fieldName"		"ButtonLegendBG"
		"xpos"			"10"
		"xpos_hidef"		"30"
		"ypos"			"373"
		"zpos"			"0"
		"wide"			"539"
		"wide_hidef"		"559"
		"tall"			"38"
		"autoResize"		"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"tabPosition"		"0"	
		"fillcolor"		"0 0 0 153"
		"PaintBackgroundType"	"0"
	}
	
	"ButtonLegend"		[$X360]
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"ButtonLegend"
		"xpos"			"10"
		"xpos_hidef"		"35"
		"ypos"			"373"
		"zpos"			"1"
		"wide"			"539"
		"wide_hidef"		"595"
		"tall"			"150"
		"visible"		"1"
										
		"SelectHintIcon"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"SelectHintIcon"
			"font"			"GameUIButtons"
			"xpos"			"10"
			"xpos_hidef"		"0"
			"ypos"			"0"
			"zpos"			"1"
			"wide"			"300"
			"tall"			"38"
			"autoResize"		"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"C"
			"textAlignment"		"Left"
			"dulltext"		"0"
			"brighttext"		"0"
		}
		
		"SelectHintLabel"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"SelectHintLabel"
			"font"			"ScoreboardMedium"
			"xpos"			"25"
			"xpos_lodef"		"37"
			"ypos"			"2"
			"zpos"			"1"
			"wide"			"300"
			"tall"			"39"
			"autoResize"		"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"#GameUI_Select"
			"textAlignment"		"Left"
			"dulltext"		"0"
			"brighttext"		"0"
		}
		
		"GamerCardIcon"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"GamerCardIcon"
			"font"			"GameUIButtons"
			"xpos"			"150"
			"xpos_hidef"		"145"
			"ypos"			"0"
			"zpos"			"1"
			"wide"			"300"
			"tall"			"38"
			"autoResize"		"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"A"
			"textAlignment"		"Left"
			"dulltext"		"0"
			"brighttext"		"0"
		}
		
		"GamerCardLabel"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"GamerCardLabel"
			"font"			"ScoreboardMedium"
			"xpos"			"170"
			"xpos_lodef"		"177"
			"ypos"			"2"
			"zpos"			"1"
			"wide"			"300"
			"tall"			"39"
			"autoResize"		"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"#TF_ViewGamercard"
			"textAlignment"		"Left"
			"dulltext"		"0"
			"brighttext"		"0"
		}
		
		"ReputationIcon"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"ReputationIcon"
			"font"			"GameUIButtons"
			"xpos"			"350"
			"xpos_hidef"		"378"
			"ypos"			"0"
			"zpos"			"1"
			"wide"			"300"
			"tall"			"38"
			"autoResize"		"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"X"
			"textAlignment"		"Left"
			"dulltext"		"0"
			"brighttext"		"0"
		}
		
		"ReputationLabel"
		{
			"ControlName"		"CExLabel"
			"fieldName"		"ReputationLabel"
			"font"			"ScoreboardMedium"
			"xpos"			"403"
			"xpos_lodef"		"377"
			"ypos"			"2"
			"zpos"			"1"
			"wide"			"300"
			"tall"			"39"
			"autoResize"		"0"
			"pinCorner"		"0"
			"visible"		"1"
			"enabled"		"1"
			"labelText"		"#GameUI_PlayerReview"
			"textAlignment"		"Left"
			"dulltext"		"0"
			"brighttext"		"0"
		}
	}				
}
