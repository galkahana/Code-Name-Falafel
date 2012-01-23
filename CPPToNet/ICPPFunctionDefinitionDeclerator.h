#pragma once
#include "DeclaratorModifier.h"

class ICPPParametersContainer;

class ICPPFunctionDefinitionDeclerator
{
public:
	virtual ~ICPPFunctionDefinitionDeclerator(){}

	virtual ICPPParametersContainer* GetParametersContainerForFunctionDefinition() = 0;
	virtual void AppendModifiersForFunctionDefinitionReturnType(const DeclaratorModifierList& inModifiers) = 0;
	virtual void SetFunctionDefinitionHasElipsis() = 0;
	virtual Hummus::EStatusCode FinalizeFunctionDefinition(bool inIsDefinition) = 0; // as oppose to just mere decleration
};
