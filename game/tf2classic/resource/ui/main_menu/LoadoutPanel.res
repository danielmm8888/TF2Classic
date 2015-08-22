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
	}	
	
	"BackgroundImage"
	{
		"ControlName"		"ImagePanel"
		"fieldName"			"BackgroundImage"
		"xpos"				"-30"
		"ypos"				"-30"
		"zpos"				"-2"
		"wide"				"f-60"
		"tall"				"f-60"
		"autoResize"		"0"
		"pinCorner"			"0"
		"scaleimage"		"1"
		"image"				"../vgui/main_menu/button_square_armed"
		"visible"			"1"
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
	
	"TopLine"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"TopLine"
		"xpos"			"c-305"
		"ypos"			"180"
		"zpos"			"2"
		"wide"			"610"
		"tall"			"10"
		"visible"		"0"
		"enabled"		"1"
		"image"			"loadout_dotted_line"
		"tileImage"		"1"
		"tileVertically" "0"
	}				
	"BottomLine"
	{
		"ControlName"	"ImagePanel"
		"fieldName"		"BottomLine"
		"xpos"			"c-305"
		"ypos"			"440"
		"zpos"			"2"
		"wide"			"610"
		"tall"			"10"
		"visible"		"0"
		"enabled"		"1"
		"image"			"loadout_dotted_line"
		"tileImage"		"1"
		"tileVertically" "0"
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
		
		"xpos"			"c-350"
		"ypos"			"160"
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
				"name"		"BUILDING"
				"activity"	"ACT_MP_STAND_BUILDING"
			}
			"animation"
			{
				"name"		"PDA"
				"activity"	"ACT_MP_STAND_PDA"
			}			
		}
	}
	
	"weaponsetpanel"
	{
		"ControlName"	"CTFWeaponSetPanel"
		"fieldName"		"weaponsetpanel"		
		"xpos"			"300"
		"ypos"			"160"
		"zpos"			"-1"		
		"wide"			"500"
		"tall"			"280"
		"autoResize"	"0"
		"pinCorner"		"0"
		"visible"		"1"
		"enabled"		"1"
		//"border"		"MainMenuHighlightBorder"
	}
	
	"BackButton"
	{
		"ControlName"	"CTFAdvButton"
		"fieldName"		"BackButton"
		"xpos"			"c-305"
		"ypos"			"r30"
		"zpos"			"20"
		"wide"			"100"
		"tall"			"25"
		"visible"		"1"
		"enabled"		"1"
		"command"		"back"		
		
		"SubButton"
		{
			"labelText" 		"Back"
			"textAlignment"		"center"
			"font"				"TallTextSmall"
			"border_default"	"AdvRoundedButtonDefault"
			"border_armed"		"AdvRoundedButtonArmed"
			"border_depressed"	"AdvRoundedButtonDepressed"	
		}
	}

	"scout_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"scout_blue"
		"xpos"				"c-220"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"		
		"command"			"select_scout"
		
		"SubButton"
		{
			"labelText" 		"Scout"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_scout_blu"	
		}
	}

	"soldier_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"soldier_blue"
		"xpos"				"c-165"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"		
		"command"			"select_soldier"
		
		"SubButton"
		{
			"labelText" 		"Soldier"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_soldier_blu"	
		}
	}

	"pyro_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"pyro_blue"
		"xpos"				"c-110"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"select_pyro"
		
		"SubButton"
		{
			"labelText" 		"Pyro"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_pyro_blu"	
		}
	}

	"demoman_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"demoman_blue"
		"xpos"				"c-55"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"select_demoman"
		
		"SubButton"
		{
			"labelText" 		"Demoman"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_demo_blu"	
		}
	}

	"heavyweapons_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"heavyweapons_blue"
		"xpos"				"c-0"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"select_heavyweapons"
		
		"SubButton"
		{
			"labelText" 		"Heavy"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_heavy_blu"	
		}
	}

	"engineer_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"engineer_blue"
		"xpos"				"c+55"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"select_engineer"
		
		"SubButton"
		{
			"labelText" 		"Engineer"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_engineer_blu"	
		}
	}

	"medic_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"medic_blue"
		"xpos"				"c+110"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"select_medic"
		
		"SubButton"
		{
			"labelText" 		"Medic"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_medic_blu"	
		}
	}

	"sniper_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"sniper_blue"
		"xpos"				"c+165"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"select_sniper"
		
		"SubButton"
		{
			"labelText" 		"Sniper"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_sniper_blu"	
		}
	}

	"spy_blue"
	{
		"ControlName"		"CTFAdvButton"
		"fieldName"			"spy_blue"
		"xpos"				"c+220"
		"ypos"				"0"
		"zpos"				"6"
		"wide"				"65"
		"tall"				"130"
		"scaleimage"		"0"	
		"visible"			"1"
		"enabled"			"1"
		"bordervisible"		"0"	
		"command"			"select_spy"
		
		"SubButton"
		{
			"labelText" 		"Spy"
			"xshift" 			"0"
			"yshift" 			"-10"
			"textAlignment"		"south"
			"font"				"TallTextSmall"
		}
		
		"SubImage"
		{
			"image" 			"class_sel_sm_spy_blu"	
		}
	}

	
}
