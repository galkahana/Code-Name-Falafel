#pragma once

#include "TypedParameter.h"
#include <string>
#include <list>


class CPPElement;
class CPPVariable;
class UsedTypeDescriptor;
class CPPFunction;

using namespace std;

typedef list<CPPElement*> CPPElementList;

class ICPPVariablesContainerElement
{
public:
	virtual ~ICPPVariablesContainerElement(){};

	virtual CPPElementList FindElements(const string& inElementName) = 0;

	virtual CPPVariable* CreateVariable(const string& inVariableName,
										UsedTypeDescriptor* inTypeDescriptor) = 0;
	virtual CPPFunction* CreateFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnType,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,
										bool inIsPure,
										bool inIsDefinition) = 0;
};