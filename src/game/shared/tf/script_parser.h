///////////// Copyright © 2006, Scott Loyd. All rights reserved. /////////////
// 
// File: c_script_parser.h
// Description:  This is an abstract class, that handles finding and loading
//				  keyvalue scripts, then passes off usable keyvalues to extended
//				  classes.  Reason for making this is because I really liked
//				  how valve setup weapon_parse.cpp but in order to use it
//				  for other script parsing, you had to recreate all the guts.
//
// Usage: See the test case in script_parser.cpp
//
// Created On: 6/19/2006 11:03:20 AM
// Created By: Scott Loyd <mailto:scottloyd@gmail.com> 
/////////////////////////////////////////////////////////////////////////////
#ifndef SCRIPT_PARSE_H
#define SCRIPT_PARSE_H
#ifdef _WIN32
#pragma once
#endif

#if defined( CLIENT_DLL )
#define CScriptParser C_ScriptParser
#endif
 
class CScriptParser
{
public:
	CScriptParser();
 
	//Parser Methods to initially open and setup the KeyValues
	void InitParser(const char *pszPath, bool bAllowNonEncryptedSearch = true,
		 bool bAllowEncryptedSearch = true, bool bCustomExtension = false);
 
	//You will need to set this up in your own class.
	virtual void Parse( KeyValues *pKeyValuesData, bool bWildcard, const char *szFileWithoutEXT ) = 0;
 
	//You can override these if you need to...
	virtual const char *GetPlaceHolderEXT() { return "X"; };
	virtual const char *GetNonEncryptedEXT() { return "txt"; };
	virtual const char *GetEncryptedEXT() { return "ctx"; };
 
	//Search path used by IFileSystem
	virtual const char *GetFSSearchPath() { return "MOD"; };
 
protected:
	bool IsParsed() { return m_bParsed; };
 
	bool FileParser(const char *pszFilePath, bool bWildcard, bool bEncrypted);
 
	void SetExtension(char *pszPath, int iPathSize, const char *pszNewEXT);
	void RemoveFileFromPath(char *pszPath);
 
	char *ExtractFileFromPath(const char *pszPath,bool noEXT = false);
 
	bool ExtCMP(const char *pszPath, const char *pszExt); //Compares extension 
 
private:
	bool m_bParsed;
};
 
#endif //SCRIPT_PARSE_H