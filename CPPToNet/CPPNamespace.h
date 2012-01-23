#pragma once

#include "EStatusCode.h"
#include "MapIterator.h"
#include "CPPElement.h"
#include "ICPPDefinitionsContainerElement.h"

#include <string>
#include <map>
#include <set>

using namespace std;

class CPPNamespace;
class CPPEnumerator;
class CPPUnion;
class CPPTypedef;
class CPPFunction;
class CPPPrimitive;
class CPPEnumeratorValue;

typedef map<string,CPPNamespace*> StringToCPPNamespaceMap;
typedef map<string, CPPEnumerator*> StringToCPPEnumeratorMap;
typedef map<string,CPPElement*> StringToCPPElementMap;
typedef map<string,CPPVariable*> StringToCPPVariableMap;
typedef map<string,CPPUnion*> StringToCPPUnionMap;
typedef map<string,CPPTypedef*> StringToCPPTypedefMap;
typedef map<string,CPPPrimitive*> StringToCPPPrimitiveMap;
typedef map<string,CPPEnumeratorValue*> StringToCPPEnumeratorValueMap;
typedef set<CPPElement*> CPPElementSet;

typedef list<CPPFunction*> CPPFunctionList;
typedef map<string,CPPFunctionList> StringToCPPFunctionListMap;

typedef list<CPPElement*> CPPElementList;
typedef map<string,CPPElementList> StringToCPPElementListMap;

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

	// add predefined primitive (normally should just be done on the global namespace)
	Hummus::EStatusCode AppendPrimitive(const string& inTypeName,CPPPrimitive* inNewType);

	// ICCPContainerElement implementation
	virtual CPPElementList FindElements(const string& inElementName);
	virtual Hummus::EStatusCode DefineAlias(const string& inAlias,CPPElement* inNewElement);
	virtual CPPEnumerator* CreateEnumerator(const string& inEnumeratorName, bool inIsDefinition);
	virtual CPPEnumeratorValue* CreateEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName);
	virtual CPPVariable* CreateVariable(const string& inVariableName,
										UsedTypeDescriptor* inTypeDescriptor);
	virtual CPPUnion* CreateUnion(const string& inUnionName,bool inIsDefinition);
	virtual CPPTypedef* CreateTypedef(const string& inTypedefName,
									  UsedTypeDescriptor* inTypeDescriptor);
	virtual CPPFunction* CreateFunction(const string& inFunctionName,
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const FunctionParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsDefinition);


private:

	// namespaces
	StringToCPPNamespaceMap mSubordinateNamespaces;
	StringToCPPNamespaceMap mNamespaceAliases;

	// enumerators
	StringToCPPEnumeratorMap mEnumerators;

	// enumerator values (shortcut for finders)
	StringToCPPEnumeratorValueMap mEnumeratorValues;

	// variables
	StringToCPPVariableMap mVariables;

	// unions
	StringToCPPUnionMap mUnions;

	// basic types
	StringToCPPPrimitiveMap mPrimitives;

	// typedefs
	StringToCPPTypedefMap mTypedefs;

	// functions
	StringToCPPFunctionListMap mFunctions;

	// aliases
	CPPElementSet mAliases;

	bool IsTypename(CPPElement* inElement);
	bool HasNonTypesWithName(const string& inName);

	// The following appending functions allow the client to either define a new element, or alias to an existing one.
	// parameters are passed both for the creation of a new element (in case needed), and for the validation
	// of the new definition (new or existing);
	CPPNamespace* AppendNewNamespace(const string& inNamespaceName,CPPNamespace* inNewNamespace);
	CPPEnumerator* AppendEnumerator(const string& inEnumeratorName,bool inIsDefinition,CPPEnumerator* inEnumerator);
	CPPUnion* AppendUnion(const string& inUnionName,bool inIsDefinition,CPPUnion* inUnion);
	CPPTypedef* AppendTypedef(const string& inTypedefName,
										UsedTypeDescriptor* inTypeDescriptor,
										CPPTypedef* inTypedef);
	CPPEnumeratorValue* AppendEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName,CPPEnumeratorValue* inEnumeratorValue);
	CPPVariable* AppendVariable(const string& inVariableName,
										  UsedTypeDescriptor* inTypeDescriptor,
										  CPPVariable* inVariable);
	CPPFunction* AppendFunction(const string& inFunctionName,
											UsedTypeDescriptor* inReturnTypeDescriptor,
											const FunctionParameterList& inParametersList,
											bool inHasElipsis,		
											bool inIsDefinition,
											CPPFunction* inFunction);
	
	// this function builds a type descriptor based on recursing an input type descriptor.
	// the recursion will happen in case the pointed type (possibly repeatedly) is a typedef.
	// Since typedefs may point to other types and add modifications on them...there needs be an accumulation
	// hence the returned type is "accumulated".
	UsedTypeDescriptor* CreateRealTypeDescriptor(UsedTypeDescriptor* inTypeDescriptor);



};
