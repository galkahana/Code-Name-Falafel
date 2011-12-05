#pragma once

#include <string>

class CPPElement;
class CPPVariable;

using namespace std;

class ICPPVariablesContainerElement
{
public:
	virtual ~ICPPVariablesContainerElement(){};

	virtual CPPElement* FindElement(const string& inElementName) = 0;

	virtual CPPVariable* CreateVariable(const string& inVariableName,
										CPPElement* inType,
										bool inIsAuto,
										bool inIsRegister,
										bool inIsExtern,
										bool inIsConst,
										bool inIsVolatile,
										bool inIsStatic) = 0;
};