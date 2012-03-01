#pragma once

#include "ICPPDefinitionsContainerElement.h"
#include "UsedTypeOrExpression.h"

#include <string>
#include <map>
#include <set>


using namespace std;

class CPPEnumerator;
class CPPUnion;
class CPPTypedef;
class CPPFunction;
class CPPEnumeratorValue;
class CPPClass;
class CPPStruct;

typedef map<string, CPPEnumerator*> StringToCPPEnumeratorMap;
typedef map<string,CPPElement*> StringToCPPElementMap;
typedef map<string,CPPVariable*> StringToCPPVariableMap;
typedef map<string,CPPUnion*> StringToCPPUnionMap;
typedef map<string,CPPTypedef*> StringToCPPTypedefMap;
typedef map<string,CPPEnumeratorValue*> StringToCPPEnumeratorValueMap;
typedef map<string,CPPClass*> StringToCPPClassMap;
typedef map<string,CPPStruct*> StringToCPPStructMap;

typedef set<CPPElement*> CPPElementSet;
typedef list<CPPFunction*> CPPFunctionList;
typedef map<string,CPPFunctionList> StringToCPPFunctionListMap;

typedef list<CPPElement*> CPPElementList;
typedef map<string,CPPElementList> StringToCPPElementListMap;

class AbstractCPPContainer : public ICPPDefinitionsContainerElement
{
public:
	AbstractCPPContainer(void);
	virtual ~AbstractCPPContainer(void);

	// ICPPDefinitionsContainerElement implementation
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
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsPure,
										const UsedTypeOrExpressionList& inTemplateSpecializationList,
										bool inIsTemplateInstantiation,
										bool inIsDefinition);
	virtual CPPFunction* CreateFunctionTemplate(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,
										UsedTypeDescriptor* inReturnType,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,
										bool inIsPure,
										bool inIsDefinition,
										const CPPElementList& inTemplateParameters,
										const UsedTypeOrExpressionList& inTemplateParametersSpecialization);
	virtual CPPClass* CreateClass(const string& inClassName,
								  bool inIsDefinition);
	virtual CPPStruct* CreateStruct(const string& inStructName,
								  bool inIsDefinition);


protected:

	// aliases
	CPPElementSet mAliases;


	// The following appending functions allow the client to either define a new element, or alias to an existing one.
	// parameters are passed both for the creation of a new element (in case needed), and for the validation
	// of the new definition (new or existing);
	virtual CPPEnumerator* AppendEnumerator(const string& inEnumeratorName,bool inIsDefinition,CPPEnumerator* inEnumerator);
	virtual CPPUnion* AppendUnion(const string& inUnionName,bool inIsDefinition,CPPUnion* inUnion);
	virtual CPPTypedef* AppendTypedef(const string& inTypedefName,
										UsedTypeDescriptor* inTypeDescriptor,
										CPPTypedef* inTypedef);
	virtual CPPEnumeratorValue* AppendEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName,CPPEnumeratorValue* inEnumeratorValue);
	virtual CPPVariable* AppendVariable(const string& inVariableName,
										  UsedTypeDescriptor* inTypeDescriptor,
										  CPPVariable* inVariable);
	virtual CPPFunction* AppendFunction(const string& inFunctionName,
											bool inIsVirtual,
											bool inIsStatic,											
											UsedTypeDescriptor* inReturnTypeDescriptor,
											const TypedParameterList& inParametersList,
											bool inHasElipsis,		
											bool inIsPure,
											const UsedTypeOrExpressionList& inTemplateSpecializationList,
											bool inIsTemplateInstantiation,
											bool inIsDefinition,
											CPPFunction* inFunction);
	virtual CPPFunction* AppendFunctionTemplate(const string& inFunctionName,
											bool inIsVirtual,
											bool inIsStatic,											
											UsedTypeDescriptor* inReturnTypeDescriptor,
											const TypedParameterList& inParametersList,
											bool inHasElipsis,		
											bool inIsPure,
											bool inIsDefinition,
											const CPPElementList& inTemplateParameters,
											CPPFunction* inFunctionTemplate);
	virtual CPPFunction* AppendFunctionTemplateSpecialization(
											const string& inFunctionName,
											bool inIsVirtual,
											bool inIsStatic,											
											UsedTypeDescriptor* inReturnTypeDescriptor,
											const TypedParameterList& inParametersList,
											bool inHasElipsis,		
											bool inIsPure,
											bool inIsDefinition,
											const UsedTypeOrExpressionList& inTemplateParametersSpecialization,
											CPPFunction* inFunctionTemplate);
	virtual CPPClass* AppendClass(const string& inClassName,
									bool inIsDefinition,
									CPPClass* inClass);
	virtual CPPStruct* AppendStruct(const string& inStructName,
									bool inIsDefinition,
									CPPStruct* inStruct);

private:

	// enumerators
	StringToCPPEnumeratorMap mEnumerators;

	// enumerator values (shortcut for finders)
	StringToCPPEnumeratorValueMap mEnumeratorValues;

	// variables
	StringToCPPVariableMap mVariables;

	// unions
	StringToCPPUnionMap mUnions;

	// typedefs
	StringToCPPTypedefMap mTypedefs;

	// functions
	StringToCPPFunctionListMap mFunctions;

	// function templates
	StringToCPPFunctionListMap mFunctionTemplates;

	// classes
	StringToCPPClassMap mClasses;

	// structs
	StringToCPPStructMap mStructs;

	bool HasNonTypesWithName(const string& inName);
	
	// this function builds a type descriptor based on recursing an input type descriptor.
	// the recursion will happen in case the pointed type (possibly repeatedly) is a typedef.
	// Since typedefs may point to other types and add modifications on them...there needs be an accumulation
	// hence the returned type is "accumulated".
	UsedTypeDescriptor* CreateRealTypeDescriptor(UsedTypeDescriptor* inTypeDescriptor);

	// for function parameters (and can handle template function parameters as well)
	bool IsEquivalentParametersList(const TypedParameterList& inParametersListLeft,const TypedParameterList& inParametersListRight);
	// for templates
	bool IsEquivalentParametersList(const TypedParameterList& inParametersListLeft,
										const CPPElementList& inTemplateParametersListLeft,
										const TypedParameterList& inParametersListRight,
										const CPPElementList& inTemplateParametersListRight);
	bool IsEquivalentTemplateParametersList(const CPPElementList& inTemplateParametersListLeft,const CPPElementList& inTemplateParametersListRight);

};

