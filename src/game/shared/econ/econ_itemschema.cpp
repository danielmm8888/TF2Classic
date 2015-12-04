#include "cbase.h"
#include "econ/econ_itemschema.h"
#include "tier3/tier3.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"
#include "script_parser.h"

const char *g_AttributeDescriptionFormats[] =
{
	"value_is_percentage",
	"value_is_inverted_percentage",
	"value_is_additive",
	"value_is_additive_percentage",
	"value_is_or",
	"value_is_date",
	"value_is_account_id",
	"value_is_particle_index",
	"value_is_killstreakeffect_index",
	"value_is_killstreak_idleeffect_index",
	"value_is_item_def",
	"value_is_from_lookup_table"
};

//-----------------------------------------------------------------------------
// Purpose: for the UtlMap
//-----------------------------------------------------------------------------
static bool schemalessFunc( const int &lhs, const int &rhs )
{
	return lhs < rhs;
}

//-----------------------------------------------------------------------------
// Purpose:
//-----------------------------------------------------------------------------
static CEconItemSchema g_EconItemSchema;
CEconItemSchema *GetItemSchema()
{
	return &g_EconItemSchema;
}

class CEconSchemaParser : public CScriptParser
{
public:
	DECLARE_CLASS_GAMEROOT(CEconSchemaParser, CScriptParser);

#define GET_STRING(copyto, from, name)													\
		if (Q_strcmp(from->GetString(#name, ""), "") || !Q_strcmp(copyto->name, ""))	\
			Q_snprintf(copyto->name, sizeof(copyto->name), from->GetString(#name, ""))

#define GET_BOOL(copyto, from, name)													\
		if (from->GetBool(#name, false) != false || !copyto->name)						\
			copyto->name = from->GetBool(#name, false)

#define GET_FLOAT(copyto, from, name)													\
		if (from->GetFloat(#name, 0.0f) != 0.0f || !copyto->name)						\
			copyto->name = from->GetFloat(#name, 0.0f)

#define GET_INT(copyto, from, name)														\
		if (from->GetInt(#name, 0) != 0 || !copyto->name)								\
			copyto->name = from->GetInt(#name, 0)

#define GET_STRING_CONVERT(copyto, from, name)											\
		if (Q_strcmp(pData->GetString(#name, ""), ""))	

#define STRING_CASE(copyto, name, str, val)												\
		if (!Q_strcmp(pData->GetString(#name, ""), #str))								\
		{																				\
			copyto->name = val;															\
		}

#define FIND_ELEMENT(map, key, val)						\
		unsigned int index = map.Find(key);				\
		if (index < map.Count())						\
			val = map.Element(index)				

#define FIND_ELEMENT_STRING(map, key, val)						\
		unsigned int index = map.Find(key);						\
		if (index < map.Count())								\
			Q_snprintf(val, sizeof(val), map.Element(index))

#define IF_ELEMENT_FOUND(map, key)						\
		unsigned int index = map.Find(key);				\
		if (index < map.Count())			

#define GET_VALUES_FAST_BOOL(dict, keys)\
		for (KeyValues *pKeyData = keys->GetFirstSubKey(); pKeyData != NULL; pKeyData = pKeyData->GetNextKey())\
		{													\
			IF_ELEMENT_FOUND(dict, pKeyData->GetName())		\
			{												\
				dict.Element(index) = pKeyData->GetBool();	\
			}												\
			else											\
			{												\
				dict.Insert(pKeyData->GetName(), pKeyData->GetBool());\
			}												\
		}

#define GET_VALUES_FAST_STRING(dict, keys)\
		for (KeyValues *pKeyData = keys->GetFirstSubKey(); pKeyData != NULL; pKeyData = pKeyData->GetNextKey())	\
		{													\
			IF_ELEMENT_FOUND(dict, pKeyData->GetName())		\
			{												\
				Q_snprintf((char*)dict.Element(index), sizeof(dict.Element(index)), pKeyData->GetString());		\
			}												\
			else											\
			{												\
				dict.Insert(pKeyData->GetName(), strdup(pKeyData->GetString()));\
			}												\
		}	

	void Parse(KeyValues *pKeyValuesData, bool bWildcard, const char *szFileWithoutEXT)
	{
		KeyValues *pPrefabs = pKeyValuesData->FindKey( "prefabs" );
		if ( pPrefabs )
		{
			ParsePrefabs( pPrefabs );
		}

		KeyValues *pGameInfo = pKeyValuesData->FindKey( "game_info" );
		if ( pGameInfo )
		{
			ParseGameInfo( pGameInfo );
		}

		KeyValues *pQualities = pKeyValuesData->FindKey( "qualities" );
		if ( pQualities )
		{
			ParseQualities( pQualities );
		}

		KeyValues *pColors = pKeyValuesData->FindKey( "colors" );
		if ( pColors )
		{
			ParseColors( pColors );
		}

		KeyValues *pAttributes = pKeyValuesData->FindKey( "attributes" );
		if ( pAttributes )
		{
			ParseAttributes( pAttributes );
		}

		KeyValues *pItems = pKeyValuesData->FindKey( "items" );
		if ( pItems )
		{
			ParseItems( pItems );
		}
	};

	void ParseGameInfo(KeyValues *pKeyValuesData)
	{
		for (KeyValues *pSubData = pKeyValuesData->GetFirstSubKey(); pSubData != NULL; pSubData = pSubData->GetNextKey())
		{
			GetItemSchema()->m_GameInfo.Insert(pSubData->GetName(), pSubData->GetFloat());
		}	
	};

	void ParseQualities(KeyValues *pKeyValuesData)
	{
		for (KeyValues *pSubData = pKeyValuesData->GetFirstSubKey(); pSubData != NULL; pSubData = pSubData->GetNextKey())
		{
			EconQuality Quality;
			GET_INT((&Quality), pSubData, value);
			GetItemSchema()->m_Qualities.Insert(pSubData->GetName(), Quality);
		}

	};

	void ParseColors(KeyValues *pKeyValuesData)
	{
		for (KeyValues *pSubData = pKeyValuesData->GetFirstSubKey(); pSubData != NULL; pSubData = pSubData->GetNextKey())
		{
			EconColor ColorDesc;
			GET_STRING((&ColorDesc), pSubData, color_name);
			GetItemSchema()->m_Colors.Insert(pSubData->GetName(), ColorDesc);
		}
	};

	void ParsePrefabs(KeyValues *pKeyValuesData)
	{
		for (KeyValues *pSubData = pKeyValuesData->GetFirstSubKey(); pSubData != NULL; pSubData = pSubData->GetNextKey())
		{
			KeyValues *Values = pSubData->MakeCopy();
			GetItemSchema()->m_PrefabsValues.Insert(pSubData->GetName(), Values);
		}
	};

	void ParseItems(KeyValues *pKeyValuesData)
	{
		for (KeyValues *pSubData = pKeyValuesData->GetFirstSubKey(); pSubData != NULL; pSubData = pSubData->GetNextKey())
		{
			// Skip over default item, not sure why it's there.
			if ( !Q_stricmp( pSubData->GetName(), "default" ) )
				continue;

			EconItemDefinition *Item = new EconItemDefinition;
			int index = atoi( pSubData->GetName() );

			ParseItemRec(pSubData, Item);
			GetItemSchema()->m_Items.Insert( index, Item );
		}
		for (unsigned int i = 0; i < GetItemSchema()->m_PrefabsValues.Count(); i++)
		{
			GetItemSchema()->m_PrefabsValues[i]->deleteThis();
		}
		GetItemSchema()->m_PrefabsValues.RemoveAll();
	};

	void ParseAttributes(KeyValues *pKeyValuesData)
	{
		for (KeyValues *pSubData = pKeyValuesData->GetFirstSubKey(); pSubData != NULL; pSubData = pSubData->GetNextKey())
		{
			EconAttributeDefinition *pAttribute = new EconAttributeDefinition;
			int index = atoi( pSubData->GetName() );

			GET_STRING( pAttribute, pSubData, name );
			GET_STRING( pAttribute, pSubData, attribute_class );
			GET_STRING( pAttribute, pSubData, description_string );

			const char *szFormat = pSubData->GetString( "description_format", "pootis" );
			pAttribute->attribute_format = UTIL_StringFieldToInt( szFormat, g_AttributeDescriptionFormats, 12 );

			GET_BOOL( pAttribute, pSubData, hidden );
			GET_STRING( pAttribute, pSubData, effect_type );
			GET_BOOL( pAttribute, pSubData, stored_as_integer );

			GetItemSchema()->m_Attributes.Insert( index, pAttribute );
		}
	};

	void ParseItemRec(KeyValues *pData, EconItemDefinition* pItem)
	{
		char prefab[64];
		Q_snprintf(prefab, sizeof(prefab), pData->GetString("prefab", ""));	//check if there's prefab for prefab.. PREFABSEPTION
		
		if (Q_strcmp(prefab, ""))
		{
			char * pch;
			pch = strtok(prefab, " ");
			while (pch != NULL)
			{
				KeyValues *pPrefabValues = NULL;
				FIND_ELEMENT(GetItemSchema()->m_PrefabsValues, pch, pPrefabValues);
				if (pPrefabValues)
				{
					ParseItemRec(pPrefabValues, pItem);
				}
				pch = strtok(NULL, " ");
			}
		}

		GET_STRING(pItem, pData, name);
		GET_BOOL(pItem, pData, show_in_armory);

		GET_STRING(pItem, pData, item_class);
		GET_STRING(pItem, pData, item_type_name);
		GET_STRING(pItem, pData, item_quality);
		GET_STRING(pItem, pData, item_logname);
		GET_STRING(pItem, pData, item_iconname);

		GET_STRING_CONVERT(pItem, pData, item_slot)
		{
			STRING_CASE(pItem, item_slot, primary, TF_WPN_TYPE_PRIMARY);
			STRING_CASE(pItem, item_slot, secondary, TF_WPN_TYPE_SECONDARY);
			STRING_CASE(pItem, item_slot, melee, TF_WPN_TYPE_MELEE);
			STRING_CASE(pItem, item_slot, pda, TF_WPN_TYPE_PDA);
			STRING_CASE(pItem, item_slot, pda2, TF_WPN_TYPE_ITEM1);
			STRING_CASE(pItem, item_slot, building, TF_WPN_TYPE_BUILDING);
			STRING_CASE(pItem, item_slot, hat, TF_WPN_TYPE_ITEM2);
			STRING_CASE(pItem, item_slot, misc, TF_WPN_TYPE_GRENADE);
		}

		GET_STRING_CONVERT(pItem, pData, anim_slot)
		{
			STRING_CASE(pItem, anim_slot, primary, TF_WPN_TYPE_PRIMARY);
			STRING_CASE(pItem, anim_slot, secondary, TF_WPN_TYPE_SECONDARY);
			STRING_CASE(pItem, anim_slot, melee, TF_WPN_TYPE_MELEE);
			STRING_CASE(pItem, anim_slot, building, TF_WPN_TYPE_BUILDING);
			STRING_CASE(pItem, anim_slot, item1, TF_WPN_TYPE_ITEM1);
			STRING_CASE(pItem, anim_slot, item2, TF_WPN_TYPE_ITEM2);
			STRING_CASE(pItem, anim_slot, FORCE_NOT_USED, -1);
		}

		GET_INT(pItem, pData, min_ilevel);
		GET_INT(pItem, pData, max_ilevel);

		GET_STRING(pItem, pData, image_inventory);
		GET_INT(pItem, pData, image_inventory_size_w);
		GET_INT(pItem, pData, image_inventory_size_h);

		GET_STRING(pItem, pData, model_player);
		GET_STRING(pItem, pData, model_world);
		
		for (KeyValues *pSubData = pData->GetFirstSubKey(); pSubData != NULL; pSubData = pSubData->GetNextKey())
		{
			if (!Q_stricmp(pSubData->GetName(), "capabilities"))
			{
				GET_VALUES_FAST_BOOL(pItem->capabilities, pSubData);
			}
			if (!Q_stricmp(pSubData->GetName(), "tags"))
			{
				GET_VALUES_FAST_BOOL(pItem->tags, pSubData);
			}
			if (!Q_stricmp(pSubData->GetName(), "model_player_per_class"))
			{
				GET_VALUES_FAST_STRING(pItem->model_player_per_class, pSubData);
			}
			if (!Q_stricmp(pSubData->GetName(), "used_by_classes"))
			{
				GET_VALUES_FAST_BOOL(pItem->used_by_classes, pSubData);
			}
			if (!Q_stricmp(pSubData->GetName(), "attributes"))
			{
				for (KeyValues *pAttribData = pSubData->GetFirstSubKey(); pAttribData != NULL; pAttribData = pAttribData->GetNextKey())
				{
					int iAttributeID = GetItemSchema()->GetAttributeIndex( pAttribData->GetName() );
					
					CEconItemAttribute attribute;
					attribute.m_iAttributeDefinitionIndex = iAttributeID;
					GET_STRING( ( &attribute ), pAttribData, attribute_class );
					GET_FLOAT( ( &attribute ), pAttribData, value );
					pItem->attributes.AddToTail( attribute );
				}
			}
			if (!Q_stricmp(pSubData->GetName(), "visuals"))
			{
				EconItemVisuals *visual = &pItem->visual;

				for (KeyValues *pVisualData = pSubData->GetFirstSubKey(); pVisualData != NULL; pVisualData = pVisualData->GetNextKey())
				{
					if (!Q_stricmp(pVisualData->GetName(), "player_bodygroups"))
					{
						GET_VALUES_FAST_BOOL(visual->player_bodygroups, pVisualData);
					}
					else if (!Q_stricmp(pVisualData->GetName(), "attached_models"))
					{
					}
					else if (!Q_stricmp(pVisualData->GetName(), "animation_replacement"))
					{
						GET_VALUES_FAST_STRING(visual->animation_replacement, pVisualData);
					}
					else if (!Q_stricmp(pVisualData->GetName(), "playback_activity"))
					{
						GET_VALUES_FAST_STRING(visual->playback_activity, pVisualData);
					}
					else if (!Q_stricmp(pVisualData->GetName(), "styles"))
					{
						/*
						for (KeyValues *pStyleData = pVisualData->GetFirstSubKey(); pStyleData != NULL; pStyleData = pStyleData->GetNextKey())
						{
							EconItemStyle *style;
							IF_ELEMENT_FOUND(visual->styles, pStyleData->GetName())
							{
								style = visual->styles.Element(index);
							}
							else
							{
								style = new EconItemStyle();
								visual->styles.Insert(pStyleData->GetName(), style);
							}

							GET_STRING(style, pStyleData, name);
							GET_STRING(style, pStyleData, model_player);
							GET_STRING(style, pStyleData, image_inventory);
							GET_BOOL(style, pStyleData, selectable);
							GET_INT(style, pStyleData, skin_red);
							GET_INT(style, pStyleData, skin_blu);

							for (KeyValues *pStyleModelData = pStyleData->GetFirstSubKey(); pStyleModelData != NULL; pStyleModelData = pStyleModelData->GetNextKey())
							{
								if (!Q_stricmp(pStyleModelData->GetName(), "model_player_per_class"))
								{
									GET_VALUES_FAST_STRING(style->model_player_per_class, pStyleModelData);
								}
							}
						}
						*/
					}
					else
					{
						GET_VALUES_FAST_STRING(visual->misc_info, pVisualData);
					}
				}
			}
		}

	};

private:
};
CEconSchemaParser g_EconSchemaParser;

//-----------------------------------------------------------------------------
// Purpose: constructor
//-----------------------------------------------------------------------------
CEconItemSchema::CEconItemSchema() : CAutoGameSystemPerFrame("CEconItemSchema")
{
	m_Items.SetLessFunc( schemalessFunc );
	m_Attributes.SetLessFunc( schemalessFunc );

	if (!filesystem)
		return;

	m_bInited = false;
	Init();
}

CEconItemSchema::~CEconItemSchema()
{
}

//-----------------------------------------------------------------------------
// Purpose: Initializer
//-----------------------------------------------------------------------------
bool CEconItemSchema::Init()
{
	if (!m_bInited)
	{
		g_EconSchemaParser.InitParser("scripts/items/items_game*.txt", true, false);

		m_bInited = true;
	}

	return true;
}

void CEconItemSchema::Update(float frametime)
{
}

#include <vgui/ILocalize.h>

#if defined( CLIENT_DLL )
void ShowItemDef(const CCommand &args)
{
	int ID = atoi(args[1]);
	EconItemDefinition *itemdef = GetItemSchema()->GetItemDefinition(ID);
	if (itemdef)
	{
		Msg("ItemID %s:\nname %s\nitem_class %s\nitem_type_name %s\n", 
			args[1], itemdef->name, itemdef->item_class, itemdef->item_type_name);

		Msg("Attributes:\n");		
		for ( int i = 0; i < itemdef->attributes.Count(); i++ )
		{
			CEconItemAttribute *pAttribute = &itemdef->attributes[i];
			EconAttributeDefinition *pStatic = pAttribute->GetStaticData();
			if ( pStatic )
			{
				float value = pAttribute->value;
				if ( pStatic->attribute_format == ATTRIB_FORMAT_PERCENTAGE || pStatic->attribute_format == ATTRIB_FORMAT_INVERTED_PERCENTAGE )
				{
					value *= 100;
				}
				wchar_t floatstr[32];
				_snwprintf(floatstr, ARRAYSIZE(floatstr) - 1, L"%i", (int)value);

				wchar_t attrib[32];
				g_pVGuiLocalize->ConstructString(attrib, sizeof(attrib), g_pVGuiLocalize->Find(pStatic->description_string), 1, floatstr);

				char attributename[32];
				g_pVGuiLocalize->ConvertUnicodeToANSI(attrib, attributename, sizeof(attributename));

				Msg("%s\n", attributename);
			}
		}
		Msg("\n");
	}
}
ConCommand itemdef("itemdef", ShowItemDef);
#endif

EconItemDefinition* CEconItemSchema::GetItemDefinition( int id )
{
	EconItemDefinition *itemdef = NULL;
	FIND_ELEMENT( m_Items, id, itemdef );
	return itemdef;
}

EconAttributeDefinition *CEconItemSchema::GetAttributeDefinition( int id )
{
	EconAttributeDefinition *attribdef = NULL;
	FIND_ELEMENT( m_Attributes, id, attribdef );
	return attribdef;
}

EconAttributeDefinition *CEconItemSchema::GetAttributeDefinitionByName( const char *name )
{
	//unsigned int index = m_Attributes.Find(name);
	//if (index < m_Attributes.Count())
	//{
	//	return &m_Attributes[index];
	//}
	FOR_EACH_MAP_FAST( m_Attributes, i )
	{
		if ( !Q_stricmp( m_Attributes[i]->name, name ) )
		{
			return m_Attributes[i];
		}
	}

	return NULL;
}

EconAttributeDefinition *CEconItemSchema::GetAttributeDefinitionByClass(const char *classname)
{
	//unsigned int index = m_Attributes.Find(name);
	//if (index < m_Attributes.Count())
	//{
	//	return &m_Attributes[index];
	//}
	FOR_EACH_MAP_FAST( m_Attributes, i )
	{
		if ( !Q_stricmp( m_Attributes[i]->attribute_class, classname ) )
		{
			return m_Attributes[i];
		}
	}

	return NULL;
}

int CEconItemSchema::GetAttributeIndex( const char *name )
{
	if ( !name )
		return -1;

	FOR_EACH_MAP_FAST( m_Attributes, i )
	{
		if ( !Q_stricmp( m_Attributes[i]->name, name ) )
		{
			return m_Attributes.Key( i );
		}
	}

	return -1;
}

//-----------------------------------------------------------------------------
// Purpose: Event handler
//-----------------------------------------------------------------------------
void CEconItemSchema::FireGameEvent(IGameEvent *event)
{
	//const char *type = event->GetName();

	//if (!TFGameRules())
	//	return;
}


BEGIN_NETWORK_TABLE_NOBASE( CEconItemAttribute, DT_EconItemAttribute )
#ifdef CLIENT_DLL
	RecvPropInt( RECVINFO( m_iAttributeDefinitionIndex ) ),
	RecvPropFloat( RECVINFO( value ) ),
#else
	SendPropInt( SENDINFO( m_iAttributeDefinitionIndex ) ),
	SendPropFloat( SENDINFO( value ) ),
#endif
END_NETWORK_TABLE()

EconAttributeDefinition *CEconItemAttribute::GetStaticData( void )
{
	return GetItemSchema()->GetAttributeDefinition( m_iAttributeDefinitionIndex );
}


CEconItemAttribute *EconItemDefinition::IterateAttributes( string_t strClass )
{
	// Returning the first attribute found.
	for ( int i = 0; i < attributes.Count(); i++ )
	{
		CEconItemAttribute *pAttribute = &attributes[i];
		string_t strMyClass = AllocPooledString( pAttribute->attribute_class );

		if ( strMyClass == strClass )
		{
			return &attributes[i];
		}
	}

	return NULL;
}
