#pragma once
#include "IByteReader.h"
#include "PreProcessor.h"
#include "BoxingBase.h"
#include "DeclaratorModifier.h"
#include "CPPElement.h"
#include "TypedParameter.h"
#include "UsedTypeOrExpression.h"

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

struct TokenState
{
	TokenState(string inToken,bool inIsPut){Token = inToken;IsPut = inIsPut;}

	string Token;
	bool IsPut;
};

typedef list<TokenState> TokenStateList;

class CPPStatementsParser
{
public:
	CPPStatementsParser(void);
	~CPPStatementsParser(void);


	EStatusCodeAndHeaderUnit Parse(Hummus::IByteReader* inSourceStream,
						 const string& inSourceFileName,
						 const StringToStringMap& inPreprocessorDefinitions,
						 const StringList& inIncludeFolders);
private:

	PreProcessor mTokensSource;
	ICPPDefinitionsContainerElementList mDefinitionContextStack;
	LocalContextList mLocalContext;
	Hummus::Long mUnnamedSequance;
	HeaderUnit* mWorkingUnit;
	CPPElementList mTemplateParameters;
	bool mIsTemplateInstantiationStatement;
	// Stack for template parameters context. used both in parsing the template parameters, to provide context
	// for template parameters reuse, and in parsing the statement dependent on the template defintion
	CPPElementListList mTemplateParametersStack;
	bool mCollectingTokenState;
	TokenStateList mTokenStateSaver;

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
	CPPElement* GetElementFromCurrentLocation(bool inRequireType);
	CPPElement* GetElementFromCurrentLocation(CPPElement::ECPPElementType inOfType);
	CPPElement* GetElementFromCurrentLocation(const ECPPElementTypeSet& inTypeSet);

	// will return either NULL or an element, if and only if there's just ONE element of this name
	CPPElement* FindQualifiedElement(ICPPElementsContainer* inContainer,const string& inElementName);

	// will return either NULL or an element, if and only if there's just ONE element of this name and of this type
	CPPElement* FindQualifiedElement(ICPPElementsContainer* inContainer,const string& inElementName,CPPElement::ECPPElementType inOfType);

	// will return either NULL or an element, if and only if there's just ONE element of this name and of these types
	CPPElement* FindQualifiedElement(ICPPElementsContainer* inContainer,const string& inElementName,ECPPElementTypeSet inOfTypes);

	Hummus::EStatusCode SkipConstantExpression();

	bool IsTypenamesContainer(CPPElement* inElement);
	string GetNewUnnamedName();

	Hummus::EStatusCode ParseGenericDeclerationStatement(ICPPDeclarationContainerDriver* inContainer);
	Hummus::EStatusCode ParseDeclarators(ICPPDeclarationContainerDriver* inContainer);

	EStatusCodeAndBool ParseFunctionPointerOrFunction(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inReturnTypeModifiersList);
	EStatusCodeAndBool ParseFieldOrFunction(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inFieldModifiersList);
	EStatusCodeAndBool ParseFunctionDefinition(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inReturnTypeModifiersList,const UsedTypeOrExpressionList& inTemplateSpecializationList,const string& inFunctionName);
	EStatusCodeAndBool ParseFieldDefinition(ICPPFieldDeclerator* inFieldDeclerator);

	Hummus::EStatusCode FinalizeFunction(ICPPFunctionDefinitionDeclerator* inFunctionDecleratorDriver,const UsedTypeOrExpressionList& inTemplateSpecializationList,bool inIsDefinition);

	Hummus::EStatusCode SkipInitializer();
	Hummus::EStatusCode SkipBlock();

	string ComputeUnqualifiedNameFromCurrentLocation(string inTypeName,const BoolAndString& inNextToken);
	CPPElement* GetScopingElementFromCurrentLocation();

	void StartLocalContext();
	void EndLocalContext();

	EStatusCode ParseTemplateParameters(CPPElementList& inParametersStorage);
	void Destroy(CPPElementList& inList);
	EStatusCode ParseTemplateSpecializationParameters(UsedTypeOrExpressionList& inParametersStorage);
	void Destroy(UsedTypeOrExpressionList& inList);

	// a probe to current location, checking if this is a used type declaration
	bool IsNowInUsedTypeDeclaration();

	// controlled methods
	BoolAndString GetNextToken();
	void PutBackToken(string inToken);

	void TurnOnTokenRevert();
	void FinalizeTokenRevert();
	void RevertTokens();
};
