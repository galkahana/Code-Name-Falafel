#pragma once

#include "CPPElement.h"
#include "AbstractCPPContainer.h"

#include <set>
#include <utility>

using namespace std;

enum EAbstractClassOrStructAccessLevel
{
	eAbstractClassOrStructAccessLevelPrivate,
	eAbstractClassOrStructAccessLevelProtected,
	eAbstractClassOrStructAccessLevelPublic
};

class AbstractClassOrStruct;

typedef set<CPPElement*> CPPElementSet;
typedef map<CPPElement*,EAbstractClassOrStructAccessLevel> CPPElementToEAbstractClassOrStructAccessLevelMap;

class AbstractClassOrStruct : public CPPElement, public AbstractCPPContainer
{
public:

	virtual ~AbstractClassOrStruct(void);


	// overrides on ICPPDefinitionsContainerElement to include template parameters, where make sense
	virtual CPPElementList FindElements(const string& inElementName);

	bool IsDefinition();

	// update function flag, when function defintion is found
	void SetIsDefinition();

	// setup base class, will fail if class is already defined as derived
	Hummus::EStatusCode AddBase(CPPElement* inBaseClass,EAbstractClassOrStructAccessLevel inAccessLevel);


	// when parsing, set the current access level, for any definition
	void SetProtectedAccessLevel();
	void SetPrivateAccessLevel();
	void SetPublicAccessLevel();

	// template class/struct methods
	// tempalte functions functionalities
	bool IsTemplate();
	bool IsTemplateSpecializaton();
	Hummus::SingleValueContainerIterator<CPPElementList> GetTemplateParametersIterator();
	CPPElementList& GetTemplateParameters();
	Hummus::SingleValueContainerIterator<UsedTypeOrExpressionList> GetTemplateParameterAssignmentsIterator();
	UsedTypeOrExpressionList& GetTemplateParameterAssignments();

protected:

	// struct/class initialization
	AbstractClassOrStruct(const string& inName,
						  bool inIsDefinition,
						  CPPElement::ECPPElementType inType,
						  EAbstractClassOrStructAccessLevel inDefaultAccessLevel);
	// template struct/class initialization
	AbstractClassOrStruct(const string& inName,
						  bool inIsDefinition,
						  CPPElement::ECPPElementType inType,
						  EAbstractClassOrStructAccessLevel inDefaultAccessLevel,
						  const CPPElementList& inTemplateParameters);
	// template struct/class specialization (either partial or full)
	AbstractClassOrStruct(const string& inName,
						  bool inIsDefinition,
						  CPPElement::ECPPElementType inType,
						  EAbstractClassOrStructAccessLevel inDefaultAccessLevel,
						  const CPPElementList& inTemplateParameters,
						  const UsedTypeOrExpressionList& inTemplateParametersAssignments);

	// all methods overriden to introduce the objects into acces levels
	virtual CPPEnumerator* AppendEnumerator(const string& inEnumeratorName,bool inIsDefinition,CPPEnumerator* inEnumerator);
	virtual CPPUnion* AppendUnion(const string& inUnionName,bool inIsDefinition,CPPUnion* inUnion);
	virtual CPPTypedef* AppendTypedef(const string& inTypedefName,
										UsedTypeDescriptor* inTypeDescriptor,
										CPPTypedef* inTypedef);
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
											const UsedTypeOrExpressionList& inTemplateAssignmentList,
											bool inIsDefinition,
											CPPFunction* inFunction);
	virtual CPPFunction* AppendFunctionTemplate(const string& inFunctionName,
											bool inIsVirtual,
											bool inIsStatic,											
											UsedTypeDescriptor* inReturnTypeDescriptor,
											const TypedParameterList& inParametersList,
											bool inHasElipsis,		
											bool inIsPure,
											const CPPElementList& inTemplateParameters,
											const UsedTypeOrExpressionList& inTemplateParametersSpecialization,
											bool inIsDefinition,
											CPPFunction* inFunctionTemplate);
	virtual CPPClass* AppendClass(const string& inClassName,
								  bool inIsDefinition,
							      CPPClass* inClass);
	virtual CPPClass* AppendClassTemplate(
									const string& inClassName,
									const CPPElementList& inTemplateParameters,
									const UsedTypeOrExpressionList& inTemplateAssigmentList,
									bool inIsDefinition,
									CPPClass* inClass);
	virtual CPPStruct* AppendStruct(const string& inStructName,
									bool inIsDefinition,
									CPPStruct* inStruct);
	virtual CPPStruct* AppendStructTemplate(const string& inStructName,
									const CPPElementList& inTemplateParameters,
									const UsedTypeOrExpressionList& inTemplateAssigmentList,
									bool inIsDefinition,
									CPPStruct* inStruct);

private:
	bool mIsDefinition;

	CPPElementSet* mAccessDefinition;
	CPPElementSet mPrivateDefinitions;
	CPPElementSet mProtectedDefinitions;
	CPPElementSet mPublicDefinitions;
	CPPElementToEAbstractClassOrStructAccessLevelMap mBaseClasses;
	bool mIsTemplate;
	CPPElementList mTemplateParameters;
	UsedTypeOrExpressionList mTemplateParametersAssignments;


	void  SetCommonItems(bool inIsDefinition,EAbstractClassOrStructAccessLevel inDefaultAccessLevel);

};

