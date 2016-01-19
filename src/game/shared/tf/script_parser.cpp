///////////// Copyright © 2006, Scott Loyd. All rights reserved. /////////////
// 
// File: c_script_parser.cpp
// Description:
//      see c_script_parser.h
//
// Created On: 6/19/2006 11:03:45 AM
// Created By: Scott Loyd <mailto:scottloyd@gmail.com> 
/////////////////////////////////////////////////////////////////////////////
// Update: 8-06-06, Scott
//  Fixed occurrences of "error C2660: 'V_strcat' : function does not take 2 
//   arguments"  after merging this with the new SDK code.
//--
// Update: 8-10-06, Scott
//  Previous fix was done incorrectly (usage of Q_strcat was off).
//--
#include "cbase.h"
#include <KeyValues.h>
#include "filesystem.h"
#include <tier0/mem.h>
#include "gamerules.h"
 
#include "script_parser.h"
 
// memdbgon must be the last include file in a .cpp file!!!
#include "tier0/memdbgon.h"
 
extern IFileSystem *filesystem;
 
 
#define FILE_PATH_MAX_LENGTH 256
char g_szSwap[FILE_PATH_MAX_LENGTH];
 
 
CScriptParser::CScriptParser()
{
	m_bParsed = false;
}
 
 
//
// This method starts the parser that will either load one file
//	or many based on pszFilePath.
// Input: pszFilePath - FS path to KeyValue file(s) for parsing.
//						extensions are defined using .X
//
void CScriptParser::InitParser(const char *pszPath,
							   bool bAllowNonEncryptedSearch,
							   bool bAllowEncryptedSearch,
							   bool bCustomExtension)
{
	//If you hit this assert, check to see where your instancing 
	//CScriptParser, make sure it isn't before the filesystem inits
	Assert(filesystem);
 
	Assert(pszPath);
 
	//Already Parsed... ?
	if(m_bParsed)
		return;
 
	//If pszPath is a wildcarded path, f.e. "scripts/weapon_*.X"
	if(strchr(pszPath,'*'))
	{
		FileFindHandle_t findHandle;
		const char *pFileName = NULL;
		char szFileBase[FILE_PATH_MAX_LENGTH];
		char szWildcardPath[FILE_PATH_MAX_LENGTH];
		char szFullFilePath[FILE_PATH_MAX_LENGTH];
 
		//Copy the file path to tweak the extensions
		Q_strncpy(szWildcardPath,pszPath,FILE_PATH_MAX_LENGTH);
		Q_strncpy(szFileBase,pszPath,FILE_PATH_MAX_LENGTH);
 
		RemoveFileFromPath(szFileBase);
 
		if(bAllowNonEncryptedSearch)
		{
			SetExtension(szWildcardPath,FILE_PATH_MAX_LENGTH,GetNonEncryptedEXT());
 
			//Search for non-encrypted files!
			pFileName = filesystem->FindFirstEx( szWildcardPath, GetFSSearchPath(), &findHandle );
			while ( pFileName != NULL )
			{
				Q_strcpy(szFullFilePath,szFileBase);
				Q_strcat(szFullFilePath,pFileName,FILE_PATH_MAX_LENGTH);
				if(!FileParser(szFullFilePath, true, false))
				{
					DevMsg("[script_parser.cpp] Unable to find '%s' for parsing!",pszPath);
					Assert(!"[script_parser.cpp] File not found for parsing!");
				}
				pFileName = filesystem->FindNext( findHandle );
			}
			filesystem->FindClose( findHandle );
		}
 
		if(bAllowEncryptedSearch)
		{
			//Search for encrypted files!
			SetExtension(szWildcardPath,FILE_PATH_MAX_LENGTH,GetEncryptedEXT());
 
			pFileName = filesystem->FindFirstEx( szWildcardPath, GetFSSearchPath(), &findHandle );
			while ( pFileName != NULL )
			{
				Q_strcpy(szFullFilePath,szFileBase);
				Q_strcat(szFullFilePath,pFileName,FILE_PATH_MAX_LENGTH);
				if(!FileParser(szFullFilePath, true, true))
				{
					DevMsg("[script_parser.cpp] Unable to find '%s' for parsing!",pszPath);
					Assert(!"[script_parser.cpp] File not found for parsing!");
				}
				pFileName = filesystem->FindNext( findHandle );
			}
			filesystem->FindClose( findHandle );
		}
	}
	else //Only one file needs to be parsed (not a wildcard).
	{
		Q_strcpy(g_szSwap,pszPath);
		if (!bCustomExtension)
		{
			SetExtension(g_szSwap, FILE_PATH_MAX_LENGTH, GetNonEncryptedEXT());
		}
		if(!filesystem->FileExists(g_szSwap, GetFSSearchPath())
			&& bAllowEncryptedSearch)
		{
			SetExtension(g_szSwap,FILE_PATH_MAX_LENGTH,GetEncryptedEXT());
			if(!filesystem->FileExists(g_szSwap, GetFSSearchPath()))
			{
				DevMsg("[script_parser.cpp] Unable to find '%s' for parsing!",pszPath);
				Assert(!"[script_parser.cpp] File not found for parsing!");
				return;
			}
		} 
		if(!FileParser(g_szSwap, false,ExtCMP(g_szSwap,GetEncryptedEXT())))
		{
			DevMsg("[script_parser.cpp] ERROR: Unable to Parse Passed Script File!");
		}
	}
	m_bParsed = true;
}
 
//
// This method handles parsing a single file, it can be called
//  many times if a wildcard path was passed to InitParser.
// Input: pszFilePath - FS path to a KeyValue file for parsing.
//		  bWildcard - Is this file the only one to be parsed? or will more come...
//
bool CScriptParser::FileParser(const char *pszFilePath, bool bWildcard, bool bEncrypted)
{
	const unsigned char *pICEKey = (bEncrypted ? g_pGameRules->GetEncryptionKey() : NULL); //
	KeyValues *pKV = new KeyValues( "KVDataFile" );
 
	//Open up the file and load it into memory!
	FileHandle_t fileHandle = filesystem->Open( pszFilePath, "rb", GetFSSearchPath() );
	if(!fileHandle)
	{
		pKV->deleteThis();
		return false;
	}
	// load file into a null-terminated buffer
	int fileSize = filesystem->Size(fileHandle);
	char *buffer = (char*)MemAllocScratch(fileSize + 1);
 
	Assert(buffer);
 
	filesystem->Read(buffer, fileSize, fileHandle); // read into local buffer
	buffer[fileSize] = NULL; // null terminate file as EOF
	filesystem->Close( fileHandle );	// close file after reading
 
	if(bEncrypted)
		UTIL_DecodeICE( (unsigned char*)buffer, fileSize, pICEKey );
 
	if(!pKV->LoadFromBuffer( pszFilePath, buffer, filesystem ))
	{
		MemFreeScratch();
		pKV->deleteThis();
		return false;
	}
 
	Parse(pKV,bWildcard,ExtractFileFromPath(pszFilePath,true));
 
	MemFreeScratch();
	pKV->deleteThis();
	return true;
}
 
//
// Changes the extension to whatever is pszNewEXT
//  i.e: pszPath = "/test/me.SI" pszNewEXT = "txt"
//		 pszPath would end up as "/test/me.txt"
//
void CScriptParser::SetExtension(char *pszPath,int iPathSize, const char *pszNewEXT)
{
	int mover = Q_strlen(pszPath);
	while(mover > 0)
	{
		if(pszPath[mover] == '.')
			break;
		mover--;
	}
	//No dot... Just append it to the end?
	if(mover == 0)
	{
		Q_strcat(pszPath,pszNewEXT,iPathSize);
	} 
	else
	{
		Q_strcpy(&pszPath[mover+1],pszNewEXT);
	}
}
//
// Removes anything past the last /
//  if passed "/test/this/stuff" would end up with "/test/this/"
//
void CScriptParser::RemoveFileFromPath(char *pszPath)
{
	int mover = Q_strlen(pszPath);
	while(mover > 0)
	{
		if(pszPath[mover] == '/')
			break;
		mover--;
	}
	pszPath[mover+1] = '\0';
}
 
//
// Takes a path, and returns just the filename
//  if noEXT = true, it removes the extension from the end.
//
char *CScriptParser::ExtractFileFromPath(const char *pszPath,bool noEXT)
{
	//Q_strncpy(g_szSwap,pszPath,FILE_PATH_MAX_LENGTH);
	int mover = Q_strlen(pszPath);
	while(mover > 0)
	{
		if(pszPath[mover] == '/')
			break;
		mover--;
	}
	Q_strncpy(g_szSwap,(pszPath + mover +1),FILE_PATH_MAX_LENGTH);
	if(noEXT)
	{
		int mover2 = Q_strlen(g_szSwap);
		while(mover2 > 0)
		{
			if(g_szSwap[mover2] == '.')
				break;
			mover2--;
		}
		g_szSwap[mover2] = '\0';
	}
	return g_szSwap;
}
 
//
// Takes in pszPath, and compares the extension on it to pszExt
//  if they match, returns true, otherwise false.
//
bool CScriptParser::ExtCMP(const char *pszPath, const char *pszExt)
{
	int mover = Q_strlen(pszPath);
	while(mover > 0)
	{
		if(pszPath[mover] == '.')
			break;
		mover--;
	}
	if(mover == 0)
		return false;
	const char *pszPathEXT = &pszPath[mover+1];
	return Q_strcmp(pszPathEXT,pszExt) == 0;
}