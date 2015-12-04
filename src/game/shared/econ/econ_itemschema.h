#ifndef ECON_ITEMSCHEMA_H
#define ECON_ITEMSCHEMA_H

#ifdef _WIN32
#pragma once
#endif

#include "igamesystem.h"
#include "GameEventListener.h"

class CEconSchemaParser;

enum
{
	ATTRIB_FORMAT_INVALID = -1,
	ATTRIB_FORMAT_PERCENTAGE,
	ATTRIB_FORMAT_INVERTED_PERCENTAGE,
	ATTRIB_FORMAT_ADDITIVE,
	ATTRIB_FORMAT_ADDITIVE_PERCENTAGE,
};
	

#define CALL_ATTRIB_HOOK_INT(value, name)			\
		value = CAttributeManager::AttribHookValue<int>(value, #name, this)

#define CALL_ATTRIB_HOOK_FLOAT(value, name)			\
		value = CAttributeManager::AttribHookValue<float>(value, #name, this)

#define CALL_ATTRIB_HOOK_INT_ON_OTHER(ent, value, name)			\
		value = CAttributeManager::AttribHookValue<int>(value, #name, ent)

#define CALL_ATTRIB_HOOK_FLOAT_ON_OTHER(ent, value, name)			\
		value = CAttributeManager::AttribHookValue<float>(value, #name, ent)

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
		attribute_format = -1;
		hidden = false;
		CLEAR_STR(effect_type);
		stored_as_integer = false;
	}

	char name[128];
	char attribute_class[128];
	char description_string[128];
	char description_format[128];
	int attribute_format;
	bool hidden;
	char effect_type[128];
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

	EconAttributeDefinition *GetStaticData( void );

	CEconItemAttribute()
	{
		m_iAttributeDefinitionIndex = -1;
		CLEAR_STR( attribute_class );
		value = 0;
	}

public:
	CNetworkVar( int, m_iAttributeDefinitionIndex );
	CNetworkVar( float, value ); // m_iRawValue32
	char attribute_class[128];
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
	CEconItemAttribute *IterateAttributes( string_t strClass );

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
	CUtlVector<CEconItemAttribute> attributes;
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
	EconAttributeDefinition *GetAttributeDefinition( int id );
	EconAttributeDefinition *GetAttributeDefinitionByName(const char* name);
	EconAttributeDefinition *GetAttributeDefinitionByClass(const char* name);
	int GetAttributeIndex( const char *classname );

private:
	CUtlDict< int, unsigned short >					m_GameInfo;
	CUtlDict< EconQuality, unsigned short >			m_Qualities;
	CUtlDict< EconColor, unsigned short >			m_Colors;
	CUtlDict< KeyValues *, unsigned short >			m_PrefabsValues;
	CUtlMap< int, EconItemDefinition * >			m_Items;
	CUtlMap< int, EconAttributeDefinition * >		m_Attributes;

	bool m_bInited;
};

CEconItemSchema *GetItemSchema();
#endif // TF_ECON_ITEMSCHEMA_H