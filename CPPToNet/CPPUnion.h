#pragma once

#include "CPPElement.h"
#include "ICPPVariablesContainerElement.h"
#include "UsedTypeOrExpression.h"

#include <string>
#include <map>

using namespace std;

class CPPVariable;

typedef map<string,CPPVariable*> StringToCPPVariableMap;

class CPPUnion : public CPPElement, public ICPPVariablesContainerElement
{
public:
	CPPUnion(const string& inName,bool inIsDefinition);
	~CPPUnion(void);


	virtual CPPElementList FindElements(const string& inElementName); 
	virtual CPPVariable* CreateVariable(const string& inVariableName,
										UsedTypeDescriptor* inTypeDescriptor);

	// unions do not contain function defintions, so always return NULL
	virtual CPPFunction* CreateFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnType,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,
										bool inIsPure,
										const UsedTypeOrExpressionList& inTemplateAssignmentList,
										bool inIsDefinition);
	virtual CPPFunction* CreateFunctionTemplate(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnType,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,
										bool inIsPure,
										const CPPElementList& inTemplateParameters,
										const UsedTypeOrExpressionList& inTemplateParametersSpecialization,
										bool inIsDefinition);


	CPPElement* FindElement(const string& inElementName); 

	bool IsDefinition();

	// update function flag, when function defintion is found
	void SetIsDefinition();

private:

	// variables
	StringToCPPVariableMap mVariables;
	bool mIsDefinition;
};
