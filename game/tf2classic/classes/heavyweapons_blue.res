#base "default.res"

"classes/HWGuy_blue.res"
{
	"classNameLabel"
	{
		"labelText"		"#TF_HWGuy"
	}
	
	"classInfo"
	{
		"text"			"#classinfo_heavy"
	}
	
	"classModel"
	{
		"fov"			"23"
		
		"model"
		{
			"modelname"	"models/player/heavy.mdl"
			"modelname_hwm"	"models/player/hwm/heavy.mdl"
			"skin"		"1"
			"origin_x" "330"
			"origin_z" "-46"
			"vcd"		"scenes/Player/Heavy/low/class_select.vcd"

			"attached_model"
			{
				"modelname" "models/weapons/w_models/w_minigun.mdl"
				"skin"		"1"
			}
		}
	}
}