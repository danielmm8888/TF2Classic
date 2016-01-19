#ifndef IHASATTRIBUTES_H
#define IHASATTRIBUTES_H
#ifdef _WIN32
#pragma once
#endif

class CAttributeManager;
class CAttributeContainer;

//-----------------------------------------------------------------------------
// Purpose: Allows to determine if entity has attribute related members
//-----------------------------------------------------------------------------
class IHasAttributes
{
public:
	virtual CAttributeManager *GetAttributeManager( void ) = 0;
	virtual CAttributeContainer *GetAttributeContainer( void ) = 0;
	virtual CBaseEntity *GetAttributeOwner( void ) = 0;
	virtual void ReapplyProvision( void ) = 0;
};

#endif // IHASATTRIBUTES_H
