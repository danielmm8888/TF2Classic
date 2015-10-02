#ifndef ECON_ITEMSCHEMA_H
#define ECON_ITEMSCHEMA_H

#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "GameEventListener.h"

class CEconEntity;

class CEconSchemaParser;
	

#define CALL_ATTRIB_HOOK_INT(value, name)			\
		value = CAttributeManager::AttribHookValue<int>(value, #name, (CEconEntity*)this)

#define CALL_ATTRIB_HOOK_FLOAT(value, name)			\
		value = CAttributeManager::AttribHookValue<float>(value, #name, (CEconEntity*)this)

#define CALL_ATTRIB_HOOK_INT_ON_OTHER(ent, value, name)			\
		value = CAttributeManager::AttribHookValue<int>(value, #name, (CEconEntity*)ent)

#define CALL_ATTRIB_HOOK_FLOAT_ON_OTHER(ent, value, name)			\
		value = CAttributeManager::AttribHookValue<float>(value, #name, (CEconEntity*)ent)

#define CLEAR_STR(name)	\
		Q_snprintf(name, sizeof(name), "")

struct EconQuality
{
	EconQuality()
	{
		value = 0;
	}

	int value;
};

struct EconColor
{
	EconColor()
	{
		CLEAR_STR(color_name);
	}

	char color_name[128];
};

struct EconAttributeDefinition
{
	EconAttributeDefinition()
	{
		CLEAR_STR(name);
		CLEAR_STR(attribute_class);
		CLEAR_STR(description_string);
		CLEAR_STR(description_format);
		hidden = false;
		CLEAR_STR(effect_type);
		stored_as_integer = false;
	}

	char name[128];
	char attribute_class[128];
	char description_string[128];
	char description_format[128];
	bool hidden;
	char effect_type[128];
	bool stored_as_integer;
};

struct EconItemAttribute
{
	EconItemAttribute()
	{
		CLEAR_STR(attribute_class);
		value = 0.0f;
	}

	char attribute_class[128];
	float value;
};

struct EconItemStyle
{
	EconItemStyle()
	{
		CLEAR_STR(name);
		CLEAR_STR(model_player);
		CLEAR_STR(image_inventory);
		skin_red = 0;
		skin_blu = 0;
		selectable = false;
	}

	int skin_red;
	int skin_blu;
	bool selectable;
	char name[128];
	char model_player[128];
	char image_inventory[128];
	CUtlDict< const char*, unsigned short > model_player_per_class;
};

struct EconItemVisuals
{
	EconItemVisuals()
	{
	}

	CUtlDict< bool, unsigned short > player_bodygroups;
	CUtlDict< const char*, unsigned short > animation_replacement;
	CUtlDict< const char*, unsigned short > playback_activity;
	CUtlDict< const char*, unsigned short > misc_info;
	//CUtlDict< EconItemStyle*, unsigned short > styles;
};

class EconItemDefinition
{
public:
	EconItemDefinition()
	{
		CLEAR_STR(name);
		show_in_armory = false;
		CLEAR_STR(item_class);
		CLEAR_STR(item_type_name);
		CLEAR_STR(item_name);
		item_slot = -1;
		anim_slot = -1;
		CLEAR_STR(item_quality);
		propername = false;
		CLEAR_STR(item_logname);
		CLEAR_STR(item_iconname);
		min_ilevel = 0;
		max_ilevel = 0;
		CLEAR_STR(image_inventory);
		image_inventory_size_w = 0;
		image_inventory_size_h = 0;
		CLEAR_STR(model_player);
		CLEAR_STR(model_world);
		attach_to_hands = false;
	}

	char name[128];
	CUtlDict< bool, unsigned short > capabilities;
	CUtlDict< bool, unsigned short > tags;
	CUtlDict< bool, unsigned short > used_by_classes;
	bool show_in_armory;
	char item_class[128];
	char item_type_name[128];
	char item_name[128];
	int  item_slot;
	int  anim_slot;
	char item_quality[128];
	bool propername;
	char item_logname[128];
	char item_iconname[128];
	int	 min_ilevel;
	int	 max_ilevel;
	char image_inventory[128];
	int	 image_inventory_size_w;
	int	 image_inventory_size_h;
	char model_player[128];
	char model_world[128];
	CUtlDict< const char*, unsigned short > model_player_per_class;
	bool attach_to_hands;
	CUtlDict< EconItemAttribute, unsigned short > attributes;
	EconItemVisuals visual;
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
	EconAttributeDefinition *GetAttributeDefinitionByClass(const char* name);	

private:
	CUtlDict< int, unsigned short >							m_GameInfo;
	CUtlDict< EconQuality, unsigned short >					m_Qualities;
	CUtlDict< EconColor, unsigned short >					m_Colors;
	CUtlDict< KeyValues*, unsigned short >					m_PrefabsValues;
	CUtlDict< EconItemDefinition*, unsigned short >			m_Items;
	CUtlDict< EconAttributeDefinition, unsigned short >		m_Attributes;

	bool m_bInited;
};

class CAttributeManager
{
public:
	CAttributeManager(){};

	template <class type>	
	static type AttribHookValue(type iValue, const char* text, CEconEntity *pEntity)
	{
		CBaseCombatWeapon* pWeapon = dynamic_cast<CBaseCombatWeapon*>( pEntity );
		float iResult = iValue;

		if ( pWeapon == NULL )
		{
			CBasePlayer *pPlayer = dynamic_cast<CBasePlayer*>(pEntity);
			if ( pPlayer )
				pWeapon = pPlayer->GetActiveWeapon();
		}

		if ( pWeapon )
		{
			if (pEntity->HasItemDefinition())
			{
				int ID = pEntity->GetItemDefIndex();
				EconItemDefinition *pItemDef = GetItemSchema()->GetItemDefinition(ID);
				EconAttributeDefinition *pAttribDef = GetItemSchema()->GetAttributeDefinitionByClass(text);

				if (pItemDef && pAttribDef)
				{
					unsigned int index = pItemDef->attributes.Find(pAttribDef->name);
					if (index < pItemDef->attributes.Count())
					{
						EconItemAttribute *attribute = &pItemDef->attributes[index];
						iResult = attribute->value;
					}
				}
			}
		}

		return iResult;
	}
};

CEconItemSchema *GetItemSchema();
#endif // TF_ECON_ITEMSCHEMA_H