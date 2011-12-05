#pragma once
#include "DeclaratorModifier.h"

#include <list>

using namespace std;

typedef list<DeclaratorModifier> DeclaratorModifierList;

class ICPPDeclerator
{
public:
	virtual ~ICPPDeclerator(){}


	virtual void AppendModifiers(const DeclaratorModifierList& inModifiers)= 0;
	virtual void AddSubscript() = 0;

};