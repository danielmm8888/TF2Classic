///////////// Copyright © 2008 LodleNet. All rights reserved. /////////////
//
//   Project     : Server
//   File        : ge_luamanager.cpp
//   Description :
//      [TODO: Write the purpose of ge_luamanager.cpp.]
//
//   Created On: 3/5/2009 4:58:54 PM
//   Created By:  <mailto:admin@lodle.net>
//   Updated By:  <mailto:matt.shirleys@gmail.com>
////////////////////////////////////////////////////////////////////////////

#include "cbase.h"
#include "ge_luamanager.h"

// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"


// DEFINE THESE FUNCTIONS TO OPEN UP THE LUA LIBRARIES WE WANT --KM
static void _luaOpenLib(lua_State* L, const char* name, lua_CFunction fn)
{
	lua_pushcfunction(L, fn);
	lua_pushstring(L, name);
	lua_call(L, 1, 0);
}

static void _luaOpenLibs(lua_State* L)
{
	_luaOpenLib(L, "", luaopen_base);
	_luaOpenLib(L, LUA_LOADLIBNAME, luaopen_package);
	_luaOpenLib(L, LUA_TABLIBNAME, luaopen_table);
	_luaOpenLib(L, LUA_STRLIBNAME, luaopen_string);
	_luaOpenLib(L, LUA_MATHLIBNAME, luaopen_math);
#ifdef _DEBUG
	_luaOpenLib(L, LUA_DBLIBNAME, luaopen_debug);
#endif
}

void RegisterLUAFuncs(lua_State *L);
void RegisterLUAGlobals(lua_State *L);

CGELUAManager gLuaMng;
CGELUAManager* GELua()
{
	return &gLuaMng;
}

void RegPublicFunctions(lua_State *L)
{
	RegisterLUAFuncs(L);
}

void RegPublicGlobals(lua_State *L)
{
	RegisterLUAGlobals(L);
}

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////

LuaHandle::LuaHandle()
{
	m_bStarted = false;
	pL = NULL;
}

LuaHandle::~LuaHandle()
{
	GELua()->DeRegisterLuaHandle(this);
}

void LuaHandle::InitDll()
{
	if (m_bStarted)
		return;

	//Create an instance; Load the core libs.
	pL = lua_open();
	_luaOpenLibs(pL);

	RegFunctions();
	RegGlobals();

	RegPublicFunctions(pL);
	RegPublicGlobals(pL);

	m_bStarted = true;
}

void LuaHandle::ShutdownDll()
{
	if (!m_bStarted)
		return;

	Shutdown();
	lua_close(pL);

	m_bStarted = false;
}

void LuaHandle::Register()
{
	GELua()->RegisterLuaHandle(this);
}

////////////////////////////////////////////////////////////////
//
////////////////////////////////////////////////////////////////

CGELUAManager::CGELUAManager()
{
	m_bInit = false;
}

CGELUAManager::~CGELUAManager()
{

}

void CGELUAManager::InitDll()
{
	// Register our LUA Functions and Globals so we can call them
	// from .lua scripts
	if (m_bInit)
		return;

	for (size_t x = 0; x<m_vHandles.size(); x++)
	{
		m_vHandles[x]->InitDll();
	}

	m_bInit = true;
}

void CGELUAManager::InitHandles(void)
{
	for (size_t x = 0; x<m_vHandles.size(); x++)
	{
		m_vHandles[x]->Init();
	}
}

void CGELUAManager::ShutdownDll()
{
	if (!m_bInit)
		return;

	for (size_t x = 0; x<m_vHandles.size(); x++)
	{
		m_vHandles[x]->ShutdownDll();
	}

	m_bInit = false;
}

void CGELUAManager::ShutdownHandles(void)
{
	for (size_t x = 0; x<m_vHandles.size(); x++)
	{
		m_vHandles[x]->Shutdown();
	}
}

void CGELUAManager::DeRegisterLuaHandle(LuaHandle* handle)
{
	if (!handle)
		return;

	for (size_t x = 0; x<m_vHandles.size(); x++)
	{
		if (m_vHandles[x] == handle)
		{
			m_vHandles.erase(m_vHandles.begin() + x);
			break;
		}
	}
}

void CGELUAManager::RegisterLuaHandle(LuaHandle* handle)
{
	if (!handle)
		return;

	for (size_t x = 0; x<m_vHandles.size(); x++)
	{
		if (m_vHandles[x] == handle)
			return;
	}

	m_vHandles.push_back(handle);

	//if we are late to the game
	if (m_bInit)
		handle->InitDll();
}