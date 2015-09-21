#base "default.res"

"classes/Engineer_red.res"
{
	"classNameLabel"
	{
		"labelText"		"#TF_Engineer"
	}
	
	"classInfo"
	{
		"text"			"#classinfo_engineer"
	}
	
	"classModel"
	{
		"fov"			"23"
		
		"model"
		{
			"modelname"	"models/player/engineer.mdl"
			"modelname_hwm"	"models/player/hwm/engineer.mdl"
			"origin_z" "-47"
			"vcd"		"scenes/Player/Engineer/low/class_select.vcd"		

			"attached_model"
			{
				"modelname" "models/weapons/w_models/w_wrench.mdl"
			}
		}	
	}
}