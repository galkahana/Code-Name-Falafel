#pragma once
#include "DeclaratorModifier.h"

class ICPPDeclerator
{
public:
	virtual ~ICPPDeclerator(){}


	virtual void AppendModifiers(const DeclaratorModifierList& inModifiers)= 0;
	virtual void AddSubscript() = 0;

};