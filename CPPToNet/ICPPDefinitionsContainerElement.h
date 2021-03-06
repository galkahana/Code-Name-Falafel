#pragma once

#include "EStatusCode.h"
#include "ICPPVariablesContainerElement.h"
#include "TypedParameter.h"
#include <string>

class CPPElement;
class CPPEnumerator;
class CPPEnumeratorValue;
class CPPUnion;
class CPPTypedef;
class CPPFunction;
class CPPClass;
class CPPStruct;

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

	// class
	virtual CPPClass* CreateClass(const string& inClassName,
								  bool inIsDefinition) = 0;

	// class templates
	virtual CPPClass* CreateClassTemplate(
								const string& inClassName,
								const CPPElementList& inTemplateParameters,
								bool inIsDefinition) = 0;


	// struct
	virtual CPPStruct* CreateStruct(const string& inStructName,
								  bool inIsDefinition) = 0;

	// struct template
	virtual CPPStruct* CreateStructTemplate(const string& inStructName,
								  const CPPElementList& inTemplateParameters,
								  bool inIsDefinition) = 0;
};											