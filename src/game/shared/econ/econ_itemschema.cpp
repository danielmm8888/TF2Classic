#include "cbase.h"
#include "econ/econ_itemschema.h"
#include "tier3/tier3.h"
#include "engine/IEngineSound.h"
#include "soundenvelope.h"
#include "script_parser.h"
#include "tf_gamerules.h"


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
		if (Q_strcmp(pData->GetString(#name, ""), #str))								\
		{																				\
			copyto->name = val;															\
		}
				

	void Parse(KeyValues *pKeyValuesData, bool bWildcard, const char *szFileWithoutEXT)
	{
		for (KeyValues *pData = pKeyValuesData->GetFirstSubKey(); pData != NULL; pData = pData->GetNextKey())
		{
			if (!Q_stricmp(pData->GetName(), "game_info"))
			{
				ParseGameInfo(pData);
			}

			if (!Q_stricmp(pData->GetName(), "qualities"))
			{
				ParseQualities(pData);
			}

			if (!Q_stricmp(pData->GetName(), "colors"))
			{
				ParseColors(pData);
			}

			if (!Q_stricmp(pData->GetName(), "prefabs"))
			{
				ParsePrefabs(pData);
			}

			if (!Q_stricmp(pData->GetName(), "items"))
			{
				ParseItems(pData);
			}

			if (!Q_stricmp(pData->GetName(), "attributes"))
			{
				ParseAttributes(pData);
			}
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
			EconItemDefinition *Item = new EconItemDefinition();
			ParseItemRec(pSubData, Item);
			GetItemSchema()->m_Items.Insert(pSubData->GetName(), Item);
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
			EconAttributeDefinition Attribute;
			GET_STRING((&Attribute), pSubData, name);
			GET_STRING((&Attribute), pSubData, attribute_class);
			GET_STRING((&Attribute), pSubData, description_string);
			GET_STRING((&Attribute), pSubData, description_format);
			GET_BOOL((&Attribute), pSubData, hidden);
			GET_STRING((&Attribute), pSubData, effect_type);
			GET_BOOL((&Attribute), pSubData, stored_as_integer);
			GetItemSchema()->m_Attributes.Insert(pSubData->GetName(), Attribute);
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
				pItem->capabilities.Insert(pSubData->GetName(), pSubData->GetBool());
			}
			if (!Q_stricmp(pSubData->GetName(), "tags"))
			{
				for (KeyValues *pTagData = pSubData->GetFirstSubKey(); pTagData != NULL; pTagData = pTagData->GetNextKey())
				{
					bool tag = false;
					FIND_ELEMENT(pItem->tags, pTagData->GetName(), tag);
					if (!tag)	//insert tag if it wasn't added before
					{
						pItem->tags.Insert(pTagData->GetName(), pTagData->GetBool());
					}
				}
			}
			if (!Q_stricmp(pSubData->GetName(), "used_by_classes"))
			{
				for (KeyValues *pInfoData = pSubData->GetFirstSubKey(); pInfoData != NULL; pInfoData = pInfoData->GetNextKey())
				{
					bool used_by_classes = false;
					FIND_ELEMENT(pItem->used_by_classes, pInfoData->GetName(), used_by_classes);
					if (!used_by_classes)	//insert info if it wasn't added before
					{
						pItem->tags.Insert(pInfoData->GetName(), pInfoData->GetBool());
					}
				}
			}
			if (!Q_stricmp(pSubData->GetName(), "attributes"))
			{
				for (KeyValues *pAttribData = pSubData->GetFirstSubKey(); pAttribData != NULL; pAttribData = pAttribData->GetNextKey())
				{
					IF_ELEMENT_FOUND(pItem->attributes, pAttribData->GetName())
					{
						EconItemAttribute *attribute = &pItem->attributes.Element(index);
						if (attribute)
						{
							GET_STRING(attribute, pAttribData, attribute_class);
							GET_FLOAT(attribute, pAttribData, value);
						}						
					}
					else
					{
						EconItemAttribute attribute;
						GET_STRING((&attribute), pAttribData, attribute_class);
						GET_FLOAT((&attribute), pAttribData, value);
						pItem->attributes.Insert(pAttribData->GetName(), attribute);
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
		for (unsigned int i = 0; i < itemdef->attributes.Count(); i++)
		{
			EconAttributeDefinition *attribute = GetItemSchema()->GetAttributeDefinition(itemdef->attributes.GetElementName(i));
			if (attribute)
			{
				float value = itemdef->attributes[i].value;
				if (!Q_stricmp(attribute->description_format, "value_is_percentage") || !Q_stricmp(attribute->description_format, "value_is_inverted_percentage"))
				{
					value *= 100;
				}
				wchar_t floatstr[32];
				_snwprintf(floatstr, ARRAYSIZE(floatstr) - 1, L"%i", (int)value);

				wchar_t attrib[32];
				g_pVGuiLocalize->ConstructString(attrib, sizeof(attrib), g_pVGuiLocalize->Find(attribute->description_string), 1, floatstr);

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

EconItemDefinition* CEconItemSchema::GetItemDefinition(int id)
{
	EconItemDefinition *itemdef = NULL;
	char buffer[33];
	itoa(id, buffer, 10);
	FIND_ELEMENT(m_Items, buffer, itemdef);
	return itemdef;
}

EconAttributeDefinition *CEconItemSchema::GetAttributeDefinition(const char* name)
{
	//unsigned int index = m_Attributes.Find(name);
	//if (index < m_Attributes.Count())
	//{
	//	return &m_Attributes[index];
	//}
	for (unsigned int i = 0; i < m_Attributes.Count(); i++)
	{
		if (!Q_stricmp(m_Attributes[i].name, name))
		{
			return &m_Attributes[i];
		}
	}
	return NULL;
}

EconAttributeDefinition *CEconItemSchema::GetAttributeDefinitionByClass(const char* name)
{
	//unsigned int index = m_Attributes.Find(name);
	//if (index < m_Attributes.Count())
	//{
	//	return &m_Attributes[index];
	//}
	for (unsigned int i = 0; i < m_Attributes.Count(); i++)
	{
		if (!Q_stricmp(m_Attributes[i].attribute_class, name))
		{
			return &m_Attributes[i];
		}
	}
	return NULL;
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