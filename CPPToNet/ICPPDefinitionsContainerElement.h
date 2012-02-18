#pragma once

#include "EStatusCode.h"
#include "ICPPVariablesContainerElement.h"
#include "FunctionParameter.h"
#include <string>

class CPPElement;
class CPPEnumerator;
class CPPEnumeratorValue;
class CPPUnion;
class CPPTypedef;
class CPPFunction;
class CPPClass;

using namespace std;

class ICPPDefinitionsContainerElement : public ICPPVariablesContainerElement
{
public:
	virtual ~ICPPDefinitionsContainerElement(){};

	virtual Hummus::EStatusCode DefineAlias(const string& inAlias,CPPElement* inNewElement) = 0;
	
	// enumrators
	virtual CPPEnumerator* CreateEnumerator(const string& inEnumeratorName, bool inIsDefinition) = 0;
	virtual CPPEnumeratorValue* CreateEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName) = 0;

	// unions
	virtual CPPUnion* CreateUnion(const string& inUnionName,bool inIsDefinition) = 0;

	// typedefs
	virtual CPPTypedef* CreateTypedef(const string& inTypedefName,
									  UsedTypeDescriptor* inTypeDescriptor) = 0;

	// function [function should be defined as a whole, for overloads to be identified]
	virtual CPPFunction* CreateFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const FunctionParameterList& inParametersList,
										bool inHasElipsis,	
										bool inIsPure,
										bool inIsDefinition) = 0;

	// class
	virtual CPPClass* CreateClass(const string& inClassName,
								  bool inIsDefinition) = 0;

};											