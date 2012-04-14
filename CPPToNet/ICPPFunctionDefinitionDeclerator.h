#pragma once
#include "DeclaratorModifier.h"
#include "UsedTypeOrExpression.h"

#include <string>
#include <list>

using namespace std;

class ICPPParametersContainer;
class UsedTypeDescriptor;
class FunctionPointerReturnTypeDeclerator;

typedef list<CPPElement*> CPPElementList;

class ICPPFunctionDefinitionDeclerator
{
public:
	virtual ~ICPPFunctionDefinitionDeclerator(){}

	virtual ICPPParametersContainer* GetParametersContainerForFunctionDefinition() = 0;
	virtual void AppendModifiersForFunctionDefinitionReturnType(const DeclaratorModifierList& inModifiers) = 0;
	virtual void SetFunctionDefinitionHasElipsis() = 0;
	virtual void SetPureFunction() = 0;
	virtual Hummus::EStatusCode FinalizeFunctionDefinition(const UsedTypeOrExpressionList& inTemplateSpecializationList,bool inIsDefinition) = 0;
	virtual Hummus::EStatusCode FinalizeFunctionTemplateDefinition(const CPPElementList& inTemplateParameters,const UsedTypeOrExpressionList& inTemplateSpecializationList,bool inIsDefinition) = 0; 
	
	// specific (at least now) for function pointer return value
	
	// setup an external object with the return type definitions (which actually are meant to be its return type)
	// and reset them for this object
	virtual void SetupFunctionPointerReturnTypeDeclerator(FunctionPointerReturnTypeDeclerator* inReturnTypeDeclerator) = 0;

	// externally setup the return type of the function [to functin pointer type, for instance]
	virtual void SetReturnType(UsedTypeDescriptor* inSetReturnType) = 0;

};
