#pragma once

#include <string>

class CPPElement;
class CPPVariable;
class UsedTypeDescriptor;

using namespace std;

class ICPPVariablesContainerElement
{
public:
	virtual ~ICPPVariablesContainerElement(){};

	virtual CPPElement* FindElement(const string& inElementName) = 0;

	virtual CPPVariable* CreateVariable(const string& inVariableName,
										UsedTypeDescriptor* inTypeDescriptor) = 0;
};