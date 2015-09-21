#base "default.res"

"classes/Pyro_yellow.res"
{
	"classNameLabel"
	{
		"labelText"		"#TF_Pyro"
	}
	
	"classInfo"
	{
		"text"			"#classinfo_pyro"
	}
	
	"classModel"
	{
		"fov"			"24"
		
		"model"
		{
			"modelname"	"models/player/pyro.mdl"
			"modelname_hwm"	"models/player/hwm/pyro.mdl"
			"skin"		"5"
			"origin_z" "-48"
			"vcd"		"scenes/Player/Pyro/low/class_select.vcd"
			
			"attached_model"
			{
				"modelname" "models/weapons/w_models/w_flamethrower.mdl"
				"skin"		"3"
			}
		}
	}
}