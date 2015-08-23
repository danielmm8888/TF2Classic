"Resource/UI/main_menu/QuitDialogPanel.res"
{
	"CTFQuitDialogPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"CTFQuitDialogPanel"
		"xpos"			"c-200"
		"ypos"			"c-100"
		"wide"			"400"
		"tall"			"200"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"border"		"TFFatLineBorderOpaque"
	}
	
	"TitleLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"TitleLabel"
		"xpos"				"20"
		"ypos"				"10"
		"zpos"				"5"
		"wide"				"600"
		"tall"				"25"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Exit Game"
		"textAlignment"		"west"
		"font"				"HudFontBiggerBold"
		"fgcolor"			"AdvTextDefault"
	}	
	
	"TitleLabelShadow"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"TitleLabelShadow"
		"xpos"				"22"
		"ypos"				"10"
		"zpos"				"4"
		"wide"				"600"
		"tall"				"25"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Exit Game"
		"textAlignment"		"west"
		"font"				"HudFontBiggerBold"
		"fgcolor"			"Black"
	}	
	
	"Logo"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"Logo"
		"xpos"			"10"
		"ypos"			"55"	
		"zpos"			"2"		
		"wide"			"75"
		"tall"			"75"
		"visible"		"0"
		"enabled"		"1"
		"image"			"../hud/eng_eureka_tele_home"
		"scaleImage"	"1"	
	}		
		
	"QuitLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"QuitLabel"
		"xpos"				"50"
		"ypos"				"70"
		"zpos"				"5"
		"wide"				"300"
		"tall"				"50"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Are you sure?"
		"textAlignment"		"center"
		"font"				"ScoreboardTeamNameLarge"
		"fgcolor"			"AdvTextDefault"
	}	
	
	"ConfirmButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"ConfirmButton"
		"xpos"				"265"
		"ypos"				"170"
		"zpos"				"6"
		"wide"				"50"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"command"			"quitconfirm"
		
		"SubButton"
		{
			"labelText" 		"Quit"
			"textAlignment"		"center"
			"font"				"TallTextSmall"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}		
	}			

	"CancelButton"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"CancelButton"
		"xpos"				"325"
		"ypos"				"170"
		"zpos"				"6"
		"wide"				"60"
		"tall"				"20"
		"visible"			"1"
		"enabled"			"1"
		"command"			"vguicancel"	
		
		"SubButton"
		{
			"labelText" 		"Cancel"
			"textAlignment"		"center"
			"font"				"TallTextSmall"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}		
	}	
}
		