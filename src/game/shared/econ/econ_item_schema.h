#ifndef ECON_ITEM_SCHEMA_H
#define ECON_ITEM_SCHEMA_H

#ifdef _WIN32
#pragma once
#endif

#include "tf_shareddefs.h"

enum
{
	ATTRIB_FORMAT_INVALID = -1,
	ATTRIB_FORMAT_PERCENTAGE = 0,
	ATTRIB_FORMAT_INVERTED_PERCENTAGE,
	ATTRIB_FORMAT_ADDITIVE,
	ATTRIB_FORMAT_ADDITIVE_PERCENTAGE,
	ATTRIB_FORMAT_OR,
};
	
enum
{
	ATTRIB_EFFECT_INVALID = -1,
	ATTRIB_EFFECT_UNUSUAL = 0,
	ATTRIB_EFFECT_STRANGE,
	ATTRIB_EFFECT_NEUTRAL,
	ATTRIB_EFFECT_POSITIVE,
	ATTRIB_EFFECT_NEGATIVE,
};

#define CALL_ATTRIB_HOOK_INT(value, name)			\
		value = CAttributeManager::AttribHookValue<int>(value, #name, this)

#define CALL_ATTRIB_HOOK_FLOAT(value, name)			\
		value = CAttributeManager::AttribHookValue<float>(value, #name, this)

#define CALL_ATTRIB_HOOK_STRING(value, name)		\
		value = CAttributeManager::AttribHookValue<string_t>(value, #name, this)


#define CALL_ATTRIB_HOOK_INT_ON_OTHER(ent, value, name)			\
		value = CAttributeManager::AttribHookValue<int>(value, #name, ent)

#define CALL_ATTRIB_HOOK_FLOAT_ON_OTHER(ent, value, name)			\
		value = CAttributeManager::AttribHookValue<float>(value, #name, ent)

#define CALL_ATTRIB_HOOK_STRING_ON_OTHER(ent, value, name)		\
		value = CAttributeManager::AttribHookValue<string_t>(value, #name, ent)

#define CLEAR_STR(name)		\
		name[0] = '\0'

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
		string_attribute = false;
		description_format = -1;
		hidden = false;
		effect_type = -1;
		stored_as_integer = false;
	}

	char name[128];
	char attribute_class[128];
	char description_string[128];
	bool string_attribute;
	int description_format;
	int effect_type;
	bool hidden;
	bool stored_as_integer;
};

// Client specific.
#ifdef CLIENT_DLL
EXTERN_RECV_TABLE( DT_EconItemAttribute );
// Server specific.
#else
EXTERN_SEND_TABLE( DT_EconItemAttribute );
#endif

class CEconItemAttribute
{
public:
	DECLARE_EMBEDDED_NETWORKVAR();
	DECLARE_CLASS_NOBASE( CEconItemAttribute );

	CEconItemAttribute()
	{
		Init( -1, 0.0f );
	}
	CEconItemAttribute( int iIndex, float flValue )
	{
		Init( iIndex, flValue );
	}
	CEconItemAttribute( int iIndex, float flValue, const char *pszAttributeClass )
	{
		Init( iIndex, flValue, pszAttributeClass );
	}
	CEconItemAttribute( int iIndex, const char *pszValue, const char *pszAttributeClass )
	{
		Init( iIndex, pszValue, pszAttributeClass );
	}

	void Init( int iIndex, float flValue, const char *pszAttributeClass = NULL );
	void Init( int iIndex, const char *pszValue, const char *pszAttributeClass = NULL );
	EconAttributeDefinition *GetStaticData( void );

public:
	CNetworkVar( int, m_iAttributeDefinitionIndex );
	CNetworkVar( float, value ); // m_iRawValue32
	CNetworkString( value_string, 128 );
	CNetworkString( attribute_class, 128 );
	string_t m_strAttributeClass;
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

class EconItemVisuals
{
public:
	EconItemVisuals()
	{
		SetDefLessFunc( animation_replacement );
		memset( aWeaponSounds, 0, sizeof( aWeaponSounds ) );
	}

public:
	CUtlDict< bool, unsigned short > player_bodygroups;
	CUtlMap< int, int > animation_replacement;
	CUtlDict< const char*, unsigned short > playback_activity;
	CUtlDict< const char*, unsigned short > misc_info;
	char aWeaponSounds[NUM_SHOOT_SOUND_TYPES][MAX_WEAPON_STRING];
	//CUtlDict< EconItemStyle*, unsigned short > styles;
};

class CEconItemDefinition
{
public:
	CEconItemDefinition()
	{
		CLEAR_STR(name);
		used_by_classes = 0;

		for ( int i = 0; i < TF_CLASS_COUNT_ALL; i++ )
			item_slot_per_class[i] = -1;

		show_in_armory = false;
		CLEAR_STR(item_class);
		CLEAR_STR(item_type_name);
		CLEAR_STR(item_name);
		CLEAR_STR(item_description);
		item_slot = -1;
		anim_slot = -1;
		CLEAR_STR(item_quality);
		baseitem = false;
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
		memset( model_player_per_class, 0, sizeof( model_player_per_class ) );
		attach_to_hands = 0;
		act_as_wearable = false;
		CLEAR_STR(mouse_pressed_sound);
		CLEAR_STR(drop_sound);
	}

	EconItemVisuals *GetVisuals( int iTeamNum = TEAM_UNASSIGNED );
	int GetLoadoutSlot( int iClass = TF_CLASS_UNDEFINED );
	CEconItemAttribute *IterateAttributes( string_t strClass );

public:
	char name[128];
	CUtlDict< bool, unsigned short > capabilities;
	CUtlDict< bool, unsigned short > tags;
	int used_by_classes;
	int item_slot_per_class[TF_CLASS_COUNT_ALL];
	bool show_in_armory;
	char item_class[128];
	char item_type_name[128];
	char item_name[128];
	char item_description[128];
	int  item_slot;
	int  anim_slot;
	char item_quality[128];
	bool baseitem;
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
	char model_player_per_class[TF_CLASS_COUNT_ALL][128];
	int attach_to_hands;
	bool act_as_wearable;
	CUtlVector<CEconItemAttribute> attributes;
	EconItemVisuals visual[TF_TEAM_COUNT];
	char mouse_pressed_sound[128];
	char drop_sound[128];
};

#endif // ECON_ITEM_SCHEMA_H
