#base "default.res"

"classes/Spy_yellow.res"
{
	"classNameLabel"
	{
		"labelText"		"#TF_Spy"
	}
	
	"classInfo"
	{
		"text"			"#classinfo_spy"
	}
	
	"classModel"
	{
		"fov"			"23"
		
		"model"
		{
			"modelname"	"models/player/spy.mdl"
			"modelname_hwm"	"models/player/hwm/spy.mdl"
			"skin" "5"
			"vcd"		"scenes/Player/Spy/low/class_select.vcd"	
								
			"attached_model"
			{
				"modelname" "models/weapons/w_models/w_knife.mdl"
				"skin"		"3"
			}
		}
	}
}