#pragma once
#include "IByteReader.h"
#include "PreProcessor.h"
#include "BoxingBase.h"
#include "DeclaratorModifier.h"
#include "CPPElement.h"
#include "TypedParameter.h"
#include "UsedTypeOrExpression.h"
#include "ITokenProvider.h"
#include "TokenProviderStateRecovery.h"
#include "ITypeParserHelper.h"

#include <string>
#include <map>
#include <list>
#include <utility>
#include <set>

using namespace std;

class HeaderUnit;
class CPPNamespace;
class ICPPDefinitionsContainerElement;
class ICPPVariablesContainerElement;
class ICPPDeclarationContainerDriver;
class ICPPFieldDeclerator;
class ICPPFunctionDefinitionDeclerator;
class ICPPElementsContainer;
class AbstractClassOrStruct;

typedef map<string,string> StringToStringMap;
typedef list<string> StringList;
typedef pair<Hummus::EStatusCode,HeaderUnit*> EStatusCodeAndHeaderUnit;
typedef pair<Hummus::EStatusCode,bool> EStatusCodeAndBool;
typedef list<CPPNamespace*> CPPNamespaceList;
typedef set<CPPNamespace*> CPPNamespaceSet;
typedef list<string> StringList;
typedef list<ICPPDefinitionsContainerElement*> ICPPDefinitionsContainerElementList;
typedef set<CPPElement::ECPPElementType> ECPPElementTypeSet;
typedef list<CPPElement*> CPPElementList;
typedef list<CPPElementList*> CPPElementListList;

struct LocalContext
{
	CPPNamespaceSet mUsedNamespaces;
};

typedef list<LocalContext> LocalContextList;

class CPPStatementsParser : public ITypeParserHelper
{
public:
	CPPStatementsParser(void);
	~CPPStatementsParser(void);


	EStatusCodeAndHeaderUnit Parse(Hummus::IByteReader* inSourceStream,
						 const string& inSourceFileName,
						 const StringToStringMap& inPreprocessorDefinitions,
						 const StringList& inIncludeFolders);


	//ITypeParserHelper implementation [internal usage...]
	virtual bool IsAboutToParseType(ITokenProvider* inTokenProvider);
	virtual TypedParameter* ParseType(ITokenProvider* inTokenProvider,const string& inTypeDelimeter);
	virtual TypedParameter* ParseTypeForNew(ITokenProvider* inTokenProvider);
	virtual CPPElement* ParseTypename(ITokenProvider* inTokenProvider);
	virtual CPPElement* ParseTypename(ITokenProvider* inTokenProvider,ECPPElementTypeSet inOfTypes);



private:

	PreProcessor mTokensSource;
	ICPPDefinitionsContainerElementList mDefinitionContextStack;
	LocalContextList mLocalContext;
	Hummus::Long mUnnamedSequance;
	HeaderUnit* mWorkingUnit;
	CPPElementList mTemplateParameters;
	// Stack for template parameters context. used both in parsing the template parameters, to provide context
	// for template parameters reuse, and in parsing the statement dependent on the template defintion
	CPPElementListList mTemplateParametersStack;

	EStatusCodeAndHeaderUnit ParseUnit();
	void SetupPrimitiveTypes();
	EStatusCodeAndBool ParseStatement(HeaderUnit* inUnitModel);

	Hummus::EStatusCode ParseNamespaceDeclaration();
	Hummus::EStatusCode ParseUsingDirective();
	Hummus::EStatusCode ParseUsingDeclaration();
	Hummus::EStatusCode ParseClassOrStructDeclaration(bool inIsClass);
	Hummus::EStatusCode ParseUnionDeclaration();
	Hummus::EStatusCode ParseTypedefDeclaration();
	Hummus::EStatusCode ParseEnumeratorDeclaration();
	Hummus::EStatusCode ParseTemplateDeclaration();

	CPPElement* FindUnqualifiedElement(const string& inElementName);
	CPPElement* FindUnqualifiedElement(const string& inElementName,CPPElement::ECPPElementType inOfType);
	CPPElement* FindUnqualifiedElement(const string& inElementName,ECPPElementTypeSet inOfTypes);
	void AddNamespaceToUnqualifiedSearch(CPPNamespace* inNamespace);
	Hummus::EStatusCode SkipSemiColon();
	CPPElement* GetElementFromCurrentLocation(ITokenProvider* inTokenProvider, bool inRequireType);
	CPPElement* GetElementFromCurrentLocation(ITokenProvider* inTokenProvider,CPPElement::ECPPElementType inOfType);
	CPPElement* GetElementFromCurrentLocation(ITokenProvider* inTokenProvider,const ECPPElementTypeSet& inTypeSet);

	// will return either NULL or an element, if and only if there's just ONE element of this name
	CPPElement* FindQualifiedElement(ICPPElementsContainer* inContainer,const string& inElementName);

	// will return either NULL or an element, if and only if there's just ONE element of this name and of this type
	CPPElement* FindQualifiedElement(ICPPElementsContainer* inContainer,const string& inElementName,CPPElement::ECPPElementType inOfType);

	// will return either NULL or an element, if and only if there's just ONE element of this name and of these types
	CPPElement* FindQualifiedElement(ICPPElementsContainer* inContainer,const string& inElementName,ECPPElementTypeSet inOfTypes);

	Hummus::EStatusCode SkipExpression(ITokenProvider* inTokenProvider);

	bool IsTypenamesContainer(CPPElement* inElement);
	string GetNewUnnamedName();

	Hummus::EStatusCode ParseGenericDeclerationStatement(ITokenProvider* inTokenProvider,ICPPDeclarationContainerDriver* inContainer);
	Hummus::EStatusCode ParseDeclarators(ITokenProvider* inTokenProvider,ICPPDeclarationContainerDriver* inContainer);

	EStatusCodeAndBool ParseFunctionPointerOrFunction(ITokenProvider* inTokenProvider,ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inReturnTypeModifiersList);
	EStatusCodeAndBool ParseFieldOrFunction(ITokenProvider* inTokenProvider , ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inFieldModifiersList);
	EStatusCodeAndBool ParseFunctionDefinition(
												ITokenProvider* inTokenProvider,
												ICPPDeclarationContainerDriver* inContainer,
												const DeclaratorModifierList& inReturnTypeModifiersList,
												const UsedTypeOrExpressionList& inTemplateAssignmentList,
												const string& inFunctionName,
												bool inIsOutOfLineDefinition);
	EStatusCodeAndBool ParseFieldDefinition(ITokenProvider* inTokenProvider,
											ICPPFieldDeclerator* inFieldDeclerator,
											bool inIsOutOfLineDefinition);

	Hummus::EStatusCode FinalizeFunction(ICPPFunctionDefinitionDeclerator* inFunctionDecleratorDriver,
										 const UsedTypeOrExpressionList& inTemplateAssignmentList,
										 bool inIsDefinition,
										 bool inIsOutOfLineDefinition);
	void CleanupTemplateParametersForOutOfLineDefinition();

	Hummus::EStatusCode SkipInitializer(ITokenProvider* inTokenProvider);
	Hummus::EStatusCode SkipBlock(ITokenProvider* inTokenProvider);

	string ComputeUnqualifiedNameFromCurrentLocation(string inTypeName,const BoolAndString& inNextToken);
	// The passed parameter "inInDefinitionContext" differs cases of looking for scoping element to scope an out-of-class definition or
	// just specification of a used type. the cases determine how template usages are intepreted - instances or specialization/main template
	CPPElement* GetScopingElementFromCurrentLocation(ITokenProvider* inTokenProvider,bool inInDefinitionContext);

	void StartLocalContext();
	void EndLocalContext();

	EStatusCode ParseTemplateParameters(CPPElementList& inParametersStorage);
	void Destroy(CPPElementList& inList);
	EStatusCode ParseTemplateAssignmentParameters(ITokenProvider* inTokenProvider,UsedTypeOrExpressionList& inParametersStorage);
	void Destroy(UsedTypeOrExpressionList& inList);
	CPPElement* FromTemplateToTemplateInstance(ITokenProvider* inTokenProvider,AbstractClassOrStruct* inTemplate);
	CPPElement* FromTemplateToTemplateSpecialization(ITokenProvider* inTokenProvider,AbstractClassOrStruct* inTemplate);
	EStatusCode SkipStatement();

};
