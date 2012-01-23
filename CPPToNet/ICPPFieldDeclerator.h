#pragma once
#include "DeclaratorModifier.h"
#include "EStatusCode.h"

class ICPPFieldDeclerator
{
public:
	virtual ~ICPPFieldDeclerator(){}


	virtual void AppendModifiers(const DeclaratorModifierList& inModifiers)= 0;
	virtual void AddSubscript() = 0;
	virtual Hummus::EStatusCode FinalizeFieldDefinition() = 0;

};