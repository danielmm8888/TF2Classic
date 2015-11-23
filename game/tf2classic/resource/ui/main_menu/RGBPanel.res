"Resource/UI/main_menu/RGBPanel.res"
{
	"CTFRGBPanel"
	{
		"ControlName"	"EditablePanel"
		"fieldName"		"CTFRGBPanel"
		"xpos"			"r280"
		"ypos"			"85"
		"zpos"			"20"
		"wide"			"225"
		"tall"			"200"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"border"		"TFFatLineBorder"
	}	
	
	"particlesmodelpanel"
	{
		"ControlName"	"CTFAdvModelPanel"
		"fieldName"		"particlesmodelpanel"
		
		"xpos"			"15"
		"ypos"			"130"
		"zpos"			"0"		
		"wide"			"200"
		"tall"			"200"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"

		"fov"			"100"
		"allow_rot"		"0"
				
		"model"
		{
			"force_pos"	"1"
			"skin"	"0"

			"angles_x" "0"
			"angles_y" "90"
			"angles_z" "0"
			"origin_x" "170"
			"origin_y" "0"
			"origin_z" "-80"
			"frame_origin_x"	"0"
			"frame_origin_y"	"0"
			"frame_origin_z"	"0"
			"spotlight" "0"
		
			"modelname"		"models/props_urban/urban_blast_door.mdl"
		}
	}
	
	"ColorLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ColorLabel"
		"xpos"				"16"
		"ypos"				"10"
		"zpos"				"5"
		"wide"				"200"
		"tall"				"22"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Colors"
		"textAlignment"		"west"
		"font"				"HudFontMediumBold"
		"fgcolor"			"Button.ArmedTextColor"
	}	
	
	"ColorLabelShadow"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ColorLabelShadow"
		"xpos"				"18"
		"ypos"				"10"
		"zpos"				"4"
		"wide"				"200"
		"tall"				"22"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Colors"
		"textAlignment"		"west"
		"font"				"HudFontMediumBold"
		"fgcolor"			"Black"
	}	
	
	"RedScrollBar"
	{
		"ControlName"		"CCvarSlider"
		"fieldName"			"RedScrollBar"
		"xpos"				"16"
		"ypos"				"40"
		"zpos"				"6"
		"wide"				"195"
		"tall"				"15"
		"visible"			"1"
		"enabled"			"1"
		"minvalue" 			"0"
		"maxvalue" 			"255"
		"labelwidth" 		"25"
		"bordervisible"		"0"
		"cvar_name"			"tf2c_setmerccolor_r"
		"command"			""
		
		"SubButton"
		{
			"labelText" 		"R"
			"textAlignment"		"west"
			"font"				"FontStorePromotion"
			"border_default"	"AdvSlider"
			"border_armed"		"AdvSlider"	
			"border_depressed"	"AdvSlider"	
		}
	}	
	
	"GrnScrollBar"
	{
		"ControlName"		"CCvarSlider"
		"fieldName"			"GrnScrollBar"
		"xpos"				"16"
		"ypos"				"65"
		"zpos"				"6"
		"wide"				"195"
		"tall"				"15"
		"visible"			"1"
		"enabled"			"1"
		"minvalue" 			"0"
		"maxvalue" 			"255"
		"labelwidth" 		"25"
		"bordervisible"		"0"
		"command"			""
		"cvar_name"			"tf2c_setmerccolor_g"
		
		"SubButton"
		{
			"labelText" 		"G"
			"textAlignment"		"west"
			"font"				"FontStorePromotion"
			"border_default"	"AdvSlider"
			"border_armed"		"AdvSlider"	
			"border_depressed"	"AdvSlider"	
		}
	}	
	
	"BluScrollBar"
	{
		"ControlName"		"CCvarSlider"
		"fieldName"			"BluScrollBar"
		"xpos"				"16"
		"ypos"				"90"
		"zpos"				"6"
		"wide"				"195"
		"tall"				"15"
		"visible"			"1"
		"enabled"			"1"
		"minvalue" 			"0"
		"maxvalue" 			"255"
		"labelwidth" 		"25"
		"bordervisible"		"0"
		"command"			""
		"cvar_name"			"tf2c_setmerccolor_b"
		
		"SubButton"
		{
			"labelText" 		"B"
			"textAlignment"		"west"
			"font"				"FontStorePromotion"
			"border_default"	"AdvSlider"
			"border_armed"		"AdvSlider"	
			"border_depressed"	"AdvSlider"	
		}
	}	
	
	"ColorBG"
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"ColorBG"
		"xpos"				"16"
		"ypos"				"115"
		"zpos"				"6"
		"wide"				"195"
		"tall"				"22"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"image"				""
		"scaleImage"		"1"
		"fillcolor"			"0 0 0 255"
		"PaintBackgroundType"	"2"
	}
	

	"ParticleLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ParticleLabel"
		"xpos"				"16"
		"ypos"				"145"
		"zpos"				"5"
		"wide"				"200"
		"tall"				"22"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Particles"
		"textAlignment"		"west"
		"font"				"HudFontMediumBold"
		"fgcolor"			"Button.ArmedTextColor"
	}	
	
	"ParticleLabelShadow"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ParticleLabelShadow"
		"xpos"				"18"
		"ypos"				"145"
		"zpos"				"4"
		"wide"				"200"
		"tall"				"22"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"Particles"
		"textAlignment"		"west"
		"font"				"HudFontMediumBold"
		"fgcolor"			"Black"
	}	
	
	"ParticleComboBox"
	{
		"ControlName"		"CCvarComboBox"
		"fieldName"			"ParticleComboBox"
		"xpos"				"16"
		"ypos"				"170"
		"zpos"				"8"
		"wide"				"195"
		"tall"				"20"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"cvar_name"			"tf2c_setmercparticle"
		"border_override"	"AdvRoundedButtonDefault"
		"font"				"FontStorePromotion"
	}
						
}
		