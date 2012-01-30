#pragma once
#include "DeclaratorModifier.h"

class ICPPParametersContainer;
class UsedTypeDescriptor;
class FunctionPointerReturnTypeDeclerator;

class ICPPFunctionDefinitionDeclerator
{
public:
	virtual ~ICPPFunctionDefinitionDeclerator(){}

	virtual ICPPParametersContainer* GetParametersContainerForFunctionDefinition() = 0;
	virtual void AppendModifiersForFunctionDefinitionReturnType(const DeclaratorModifierList& inModifiers) = 0;
	virtual void SetFunctionDefinitionHasElipsis() = 0;
	virtual Hummus::EStatusCode FinalizeFunctionDefinition(bool inIsDefinition) = 0; // as oppose to just mere decleration
	
	// specifc (at least now) for function pointer return value
	
	// setup an external object with the return type definitions (which actually are meant to be its return type)
	// and reset them for this object
	virtual void SetupFunctionPointerReturnTypeDeclerator(FunctionPointerReturnTypeDeclerator* inReturnTypeDeclerator);

	// externally setup the return type of the function [to functin pointer type, for instance]
	virtual void SetReturnType(UsedTypeDescriptor* inSetReturnType) = 0;

};
