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
	CPPUnion(const string& inName);
	~CPPUnion(void);


	virtual CPPElement* FindElement(const string& inElementName); 
	virtual CPPVariable* CreateVariable(const string& inVariableName,
									CPPElement* inType,
									bool inIsAuto,
									bool inIsRegister,
									bool inIsExtern,
									bool inIsConst,
									bool inIsVolatile);

private:

	// variables
	StringToCPPVariableMap mVariables;

};