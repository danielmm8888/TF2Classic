#ifndef TF_ECON_ITEMSCHEMA_H
#define TF_ECON_ITEMSCHEMA_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"
#include "igamesystem.h"
#include "GameEventListener.h"

class CEconItemSchema;
class CEconSchemaParser;

#define GET_STRING(copyto, from, name)													\
		if (Q_strcmp(from->GetString(#name, ""), "") || !Q_strcmp(copyto->name, ""))	\
			Q_snprintf(copyto->name, sizeof(copyto->name), from->GetString(#name, ""))

struct EconQuality
{
	EconQuality(){
		value = 0;
	}
	int value;
};

struct EconColor
{
	EconColor(){
		V_strcpy_safe(color_name, "");
	}
	char color_name[64];
};

struct EconAttributeDefinition
{
	EconAttributeDefinition(){
		V_strcpy_safe(name, "");
		V_strcpy_safe(attribute_class, "");
		V_strcpy_safe(description_string, "");
		V_strcpy_safe(description_format, "");
		hidden = false;
		V_strcpy_safe(effect_type, "");
		stored_as_integer = false;
	}
	char name[64];
	char attribute_class[64];
	char description_string[64];
	char description_format[64];
	bool hidden;
	char effect_type[64];
	bool stored_as_integer;
};

struct EconItemAttribute
{
	EconItemAttribute(){
		V_strcpy_safe(attribute_class, "");
		value = 0.0f;
	}
	char attribute_class[64];
	float value;
};


struct EconItemStyle
{
	EconItemStyle(){
		V_strcpy_safe(image_inventory, "");
		skin_red = 0;
		skin_blu = 0;
		selectable = false;
	}
	int skin_red;
	int skin_blu;
	bool selectable;
	char image_inventory[64];
};

struct EconItemVisuals
{
	EconItemVisuals(){
		V_strcpy_safe(sound_single_shot, "");
		V_strcpy_safe(sound_burst, "");
		V_strcpy_safe(sound_special1, "");
	}
	char sound_single_shot[64];
	char sound_burst[64];
	char sound_special1[64];
	CUtlDict< const char*, unsigned short > animation_replacement;
	CUtlDict< EconItemStyle, unsigned short > styles;
};

class EconItemDefinition
{
public:
	EconItemDefinition()
	{
		V_strcpy_safe(name, "");
		show_in_armory = false;
		V_strcpy_safe(item_class, "");
		V_strcpy_safe(item_type_name, "");
		V_strcpy_safe(item_name, "");
		V_strcpy_safe(item_slot, "");
		V_strcpy_safe(item_quality, "");
		propername = false;
		V_strcpy_safe(item_logname, "");
		V_strcpy_safe(item_iconname, "");
		min_ilevel = 0;
		max_ilevel = 0;
		V_strcpy_safe(image_inventory, "");
		image_inventory_size_w = 0;
		image_inventory_size_h = 0;
		V_strcpy_safe(model_player, "");
		attach_to_hands = false;
	}

	char name[64];
	CUtlDict< bool, unsigned short > capabilities;
	CUtlDict< bool, unsigned short > tags;
	CUtlDict< bool, unsigned short > used_by_classes;
	bool show_in_armory;
	char item_class[64];
	char item_type_name[64];
	char item_name[64];
	char item_slot[64];
	char item_quality[64];
	bool propername;
	char item_logname[64];
	char item_iconname[64];
	int	 min_ilevel;
	int	 max_ilevel;
	char image_inventory[64];
	int	 image_inventory_size_w;
	int	 image_inventory_size_h;
	char model_player[64];
	bool attach_to_hands;
	CUtlDict< EconItemAttribute, unsigned short > attributes;
	//EconItemVisuals visuals;
};

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
class CEconItemSchema : public CAutoGameSystemPerFrame, public CGameEventListener
{
	friend CEconSchemaParser;
public:
	CEconItemSchema();
	~CEconItemSchema();

	// Methods of IGameSystem
	virtual bool Init();
	virtual char const *Name() { return "CEconItemSchema"; }
	// Gets called each frame
	virtual void Update(float frametime);

	// Methods of CGameEventListener
	virtual void FireGameEvent(IGameEvent *event);

	EconItemDefinition* GetItemDefinition(int id);
	EconAttributeDefinition *GetAttributeDefinition(const char* name);
	
private:
	CUtlDict< int, unsigned short >							m_GameInfo;
	CUtlDict< EconQuality, unsigned short >					m_Qualities;
	CUtlDict< EconColor, unsigned short >					m_Colors;
	CUtlDict< KeyValues*, unsigned short >					m_PrefabsValues;
	CUtlDict< EconItemDefinition*, unsigned short >			m_Items;
	CUtlDict< EconAttributeDefinition, unsigned short >		m_Attributes;

	bool m_bInited;
};

CEconItemSchema *GetItemSchema();
#endif // TF_ECON_ITEMSCHEMA_H
