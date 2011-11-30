#pragma once

#include "EStatusCode.h"
#include "MapIterator.h"
#include "CPPElement.h"
#include "ICPPDefinitionsContainerElement.h"

#include <string>
#include <map>

using namespace std;

class CPPNamespace;
class CPPEnumerator;
class CPPUnion;

typedef map<string,CPPNamespace*> StringToCPPNamespaceMap;
typedef map<string, CPPEnumerator*> StringToCPPEnumeratorMap;
typedef map<string,CPPElement*> StringToCPPElementMap;
typedef map<string,CPPVariable*> StringToCPPVariableMap;
typedef map<string,CPPUnion*> StringToCPPUnionMap;

class CPPNamespace : public CPPElement, public ICPPDefinitionsContainerElement
{
public:
	CPPNamespace(const string& inName);
	~CPPNamespace(void);

	// Create  a new namespace with the input name. will return a namespace if one already exists
	CPPNamespace* CreateNewNamespace(const string& inNamespaceName);

	// Get an existing namespace. will returhn NULL if not found
	CPPNamespace* GetNamespace(const string& inNamespaceName);

	Hummus::MapIterator<StringToCPPNamespaceMap> GetNamespaceIterator();

	Hummus::EStatusCode CreateNewNamespaceAlias(const string& inNamespaceName,CPPNamespace* inNamespace);

	// add predefined type, that does not require another definition
	Hummus::EStatusCode AppendBasicType(const string& inTypeName,CPPElement* inNewType);

	// ICCPContainerElement implementation
	virtual CPPElement* FindElement(const string& inElementName); 
	virtual Hummus::EStatusCode DefineAlias(const string& inAlias,CPPElement* inNewElement);
	virtual CPPEnumerator* CreateEnumerator(const string& inEnumeratorName);
	virtual CPPEnumeratorValue* CreateEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName);
	virtual CPPVariable* CreateVariable(const string& inVariableName,
									CPPElement* inType,
									bool inIsAuto,
									bool inIsRegister,
									bool inIsExtern,
									bool inIsConst,
									bool inIsVolatile);
	virtual CPPUnion* CreateUnion(const string& inUnionName);



private:

	StringToCPPElementMap mDefinitions; // generic lookup
	StringToCPPElementMap mTypenames; // types lookup
	StringToCPPElementMap mValueItems; // values (enums, variables, functions) lookup

	// namespaces
	StringToCPPNamespaceMap mSubordinateNamespaces;
	StringToCPPNamespaceMap mNamespaceAliases;

	// enumerators
	StringToCPPEnumeratorMap mEnumerators;

	// variables
	StringToCPPVariableMap mVariables;

	// unions
	StringToCPPUnionMap mUnions;

	// basic types
	StringToCPPElementMap mBasicTypes;

	bool IsTypename(CPPElement* inElement);

};
