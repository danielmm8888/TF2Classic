"Resource/UI/main_menu/LoadoutPanel.res"
{		
	"CTFLoadoutPanel"
	{
		"ControlName"		"EditablePanel"
		"fieldName"			"CTFLoadoutPanel"
		"xpos"				"0"
		"ypos"				"0"
		"wide"				"f0"
		"tall"				"f0"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"border"			""
	}	
	
	"BackgroundImage"
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"BackgroundImage"
		"xpos"				"100"
		"ypos"				"20"
		"zpos"				"-2"
		"wide"				"f+200"
		"tall"				"f+40"
		"autoResize"		"0"
		"pinCorner"			"0"
		"scaleimage"		"1"
		"border"			"AdvRoundedButtonArmed"
		"visible"			"1"
		//"fillcolor"			"46 43 42 255"
		"enabled"			"1"
	}
	
	"ClassLabel"
	{
		"ControlName"	"CExLabel"
		"fieldName"		"ClassLabel"
		"font"			"HudFontMediumBold"
		"labelText"		"#ClassBeingEquipped"
		"textAlignment"	"west"
		"xpos"			"c-280"
		"ypos"			"15"
		"zpos"			"1"
		"wide"			"200"
		"tall"			"25"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
	}
	
	
	"CharacterLoadout"
	{
		"ControlName"		"Label"
		"fieldName"		"CharacterLoadout"
		"font"			"HudFontSmallestBold"
		"labelText"		"#CharacterLoadout"
		"textAlignment"	"south-west"
		"xpos"			"c0"
		"ypos"			"20"
		"zpos"			"1"
		"wide"			"f0"
		"tall"			"15"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
	}
	"EquipLabel"
	{
		"ControlName"		"Label"
		"fieldName"		"EquipLabel"
		"font"			"HudFontMediumBigBold"
		"labelText"		"#EquipYourClass"
		"textAlignment"	"north-west"
		"xpos"			"0"
		"ypos"			"35"
		"zpos"			"1"
		"wide"			"f0"
		"tall"			"30"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
	}
	
	"classmodelpanel"
	{
		"ControlName"	"CTFAdvModelPanel"
		"fieldName"		"classmodelpanel"
		
		"xpos"			"c-320"
		"ypos"			"c-90"
		"zpos"			"0"		
		"wide"			"250"
		"tall"			"280"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"

		"fov"			"25"
		"allow_rot"		"1"
				
		"model"
		{
			"force_pos"	"1"
			"skin"	"0"

			"angles_x" "0"
			"angles_y" "200"
			"angles_z" "0"
			"origin_x" "190"
			"origin_y" "0"
			"origin_z" "-36"
			"frame_origin_x"	"0"
			"frame_origin_y"	"0"
			"frame_origin_z"	"0"
			"spotlight" "1"
		
			"modelname"		"models/player/heavy.mdl"
			
			"attached_model"
			{
				"modelname" "models/weapons/w_models/w_flamethrower.mdl"
				"skin"	"0"
			}
			
			"animation"
			{
				"name"		"PRIMARY"
				"activity"	"ACT_MP_STAND_PRIMARY"
				"default"	"1"
			}
			"animation"
			{
				"name"		"SECONDARY"
				"activity"	"ACT_MP_STAND_SECONDARY"
			}
			"animation"
			{
				"name"		"MELEE"
				"activity"	"ACT_MP_STAND_MELEE"
			}
			"animation"
			{
				"name"		"GRENADE"
				"activity"	""
			}	
			"animation"
			{
				"name"		"BUILDING"
				"activity"	"ACT_MP_STAND_BUILDING"
			}	
			"animation"
			{
				"name"		"PDA"
				"activity"	"ACT_MP_STAND_PDA"
			}	
			"animation"
			{
				"name"		"ITEM1"
				"activity"	"ACT_MP_STAND_ITEM1"
			}
			"animation"
			{
				"name"		"ITEM2"
				"activity"	"ACT_MP_STAND_ITEM2"
			}
			"animation"
			{
				"name"		"MELEE_ALLCLASS"
				"activity"	"ACT_MP_STAND_MELEE_ALLCLASS"
			}
			"animation"
			{
				"name"		"SECONDARY2"
				"activity"	"ACT_MP_STAND_SECONDARY2"
			}
			"animation"
			{
				"name"		"PRIMARY2"
				"activity"	"ACT_MP_STAND_PRIMARY2"
			}
		}
	}
	
	"gamemodelpanel"
	{
		"ControlName"	"CModelPanel"
		"fieldName"		"gamemodelpanel"
		
		"xpos"			"c-320"
		"ypos"			"c-90"
		"zpos"			"2"		
		"wide"			"250"
		"tall"			"280"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		"fov"			"28"
				
		"model"
		{
			"skin"	"0"
			"angles_x" "0"
			"angles_y" "175"
			"angles_z" "0"
			"origin_x" "190"
			"origin_y" "0"
			"origin_z" "-36"
			"frame_origin_x"	"0"
			"frame_origin_y"	"0"
			"frame_origin_z"	"0"
			"spotlight" "1"
		
			"modelname"		"models/player/merc_deathmatch.mdl"
			
			"attached_model"
			{
				"modelname" "models/weapons/w_models/w_crowbar.mdl"
				"skin"	"0"
			}
			
			"animation"
			{
				"name"		"MELEE"
				"activity"	"ACT_MP_STAND_MELEE"
				"default"	"1"
			}			
		}
	}
	
	"weaponsetpanel"
	{
		"ControlName"	"CTFWeaponSetPanel"
		"fieldName"		"weaponsetpanel"		
		"xpos"			"c-70"
		"ypos"			"c-70"
		"zpos"			"-1"		
		"wide"			"380"
		"tall"			"280"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		//"border"		"MainMenuHighlightBorder"
	}

	"rgbpanel"
	{
		"ControlName"	"CTFRGBPanel"
		"fieldName"		"rgbpanel"		
		"xpos"			"c-80"
		"ypos"			"c-70"
		"zpos"			"-1"		
		"wide"			"230"
		"tall"			"280"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"0"
		"enabled"		"1"
		"border"		"TFFatLineBorder"
	}	
	
	"BackButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"BackButton"
		"xpos"			"c-245"
		"ypos"			"r60"
		"zpos"			"20"
		"wide"			"100"
		"tall"			"25"
		"visible"		"1"
		"enabled"		"1"
		"command"		"back"		
		
		"SubButton"
		{
			"labelText" 		"<< Back (&Q)"
			"textAlignment"		"center"
			"font"				"TallTextSmall"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
	}	

	"ParticleLabel"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ParticleLabel"
		"xpos"				"c-280"
		"ypos"				"115"
		"zpos"				"5"
		"wide"				"500"
		"tall"				"30"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%classname%"
		"textAlignment"		"west"
		"font"				"TeamMenuBold"
		"fgcolor"			"Button.ArmedTextColor"
	}	
	
	"ParticleLabelShadow"
	{
		"ControlName"		"CExLabel"
		"fieldName"			"ParticleLabelShadow"
		"xpos"				"c-278"
		"ypos"				"115"
		"zpos"				"4"
		"wide"				"500"
		"tall"				"30"
		"autoResize"		"0"
		"pinCorner"			"0"
		"visible"			"1"
		"enabled"			"1"
		"labelText"			"%classname%"
		"textAlignment"		"west"
		"font"				"TeamMenuBold"
		"fgcolor"			"Black"
	}	
	
	"LogoCircle"
	{
		"ControlName"	"CTFRotatingImagePanel"
		"fieldName"		"LogoCircle"
		"xpos"			"87"
		"ypos"			"44"
		"zpos"			"5"
		"wide"			"30"
		"tall"			"30"
		"image"			"vgui/class_icons/scout"
		"visible"		"0"
		"enabled"		"1"
	}
	
	"classselection"
	{
		"ControlName"		"CAdvTabs"
		"fieldName"			"classselection"
		"xpos"				"c-300"
		"ypos"				"30"
		"zpos"				"6"
		"wide"				"600"
		"tall"				"63"
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"		
		"offset"			"3"
	
	
		"scout_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"scout_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"		
			"command"			"select_scout"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/scout"	
				"imagewidth"		"55"
			}
		}
	
		"soldier_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"soldier_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"		
			"command"			"select_soldier"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/soldier"	
				"imagewidth"		"55"
			}
		}
	
		"pyro_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"pyro_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_pyro"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/pyro"	
				"imagewidth"		"55"
			}
		}
	
		"demoman_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"demoman_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_demoman"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/demo"	
				"imagewidth"		"55"
			}
		}
	
		"heavyweapons_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"heavyweapons_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_heavyweapons"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/heavy"	
				"imagewidth"		"55"
			}
		}
	
		"engineer_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"engineer_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_engineer"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/engineer"	
				"imagewidth"		"55"
			}
		}
	
		"medic_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"medic_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_medic"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/medic"	
				"imagewidth"		"55"
			}
		}
	
		"sniper_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"sniper_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_sniper"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/sniper"	
				"imagewidth"		"55"
			}
		}
	
		"spy_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"spy_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_spy"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/spy"	
				"imagewidth"		"55"
			}
		}
	
		"merc_blue"
		{
			"ControlName"		"CTFAdvButton"
			"fieldName"			"merc_blue"
			"scaleimage"		"0"	
			"visible"			"1"
			"enabled"			"1"
			"bordervisible"		"0"	
			"command"			"select_merc"
			
			"SubButton"
			{
				"labelText" 		""
				"textAlignment"		"south"
				"font"				"TallTextSmall"
				"selectedFgColor_override"		"HudProgressBarActive"
			}
			
			"SubImage"
			{
				"image" 			"class_icons/allclass"	
				"imagewidth"		"55"
			}
		}
	}

	
}
