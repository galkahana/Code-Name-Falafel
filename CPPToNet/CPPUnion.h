#pragma once

#include "CPPElement.h"
#include "ICPPVariablesContainerElement.h"

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
										UsedTypeDescriptor* inReturnType,
										const FunctionParameterList& inParametersList,
										bool inHasElipsis,
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
