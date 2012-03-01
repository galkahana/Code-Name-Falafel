#pragma once

#include "TypedParameter.h"
#include "UsedTypeOrExpression.h"
#include "ICPPElementsContainer.h"
#include <string>
#include <list>



class CPPElement;
class CPPVariable;
class UsedTypeDescriptor;
class CPPFunction;

using namespace std;

typedef list<CPPElement*> CPPElementList;

class ICPPVariablesContainerElement : public ICPPElementsContainer
{
public:
	virtual ~ICPPVariablesContainerElement(){};

	virtual CPPVariable* CreateVariable(const string& inVariableName,
										UsedTypeDescriptor* inTypeDescriptor) = 0;
	virtual CPPFunction* CreateFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnType,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,
										bool inIsPure,
										const UsedTypeOrExpressionList& inTemplateSpecializationList,
										bool inIsTemplateInstantiation,
										bool inIsDefinition) = 0;
	virtual CPPFunction* CreateFunctionTemplate(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnType,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,
										bool inIsPure,
										bool inIsDefinition,
										const CPPElementList& inTemplateParameters,
										const UsedTypeOrExpressionList& inTemplateParametersSpecialization) = 0;
};