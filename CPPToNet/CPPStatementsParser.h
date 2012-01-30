#pragma once
#include "IByteReader.h"
#include "PreProcessor.h"
#include "BoxingBase.h"
#include "DeclaratorModifier.h"
#include "CPPElement.h"

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

typedef map<string,string> StringToStringMap;
typedef list<string> StringList;
typedef pair<Hummus::EStatusCode,HeaderUnit*> EStatusCodeAndHeaderUnit;
typedef pair<Hummus::EStatusCode,bool> EStatusCodeAndBool;
typedef list<CPPNamespace*> CPPNamespaceList;
typedef set<CPPNamespace*> CPPNamespaceSet;
typedef list<string> StringList;
typedef list<ICPPDefinitionsContainerElement*> ICPPDefinitionsContainerElementList;
typedef set<CPPElement::ECPPElementType> ECPPElementTypeSet;

struct LocalContext
{
	CPPNamespaceSet mUsedNamespaces;
};

typedef list<LocalContext> LocalContextList;

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
	

	EStatusCodeAndHeaderUnit ParseUnit();
	void SetupPrimitiveTypes();
	EStatusCodeAndBool ParseStatement(HeaderUnit* inUnitModel);

	Hummus::EStatusCode ParseNamespaceDeclaration();
	Hummus::EStatusCode ParseUsingDirective();
	Hummus::EStatusCode ParseUsingDeclaration();
	Hummus::EStatusCode ParseClassDeclaration();
	Hummus::EStatusCode ParseStructDeclaration();
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

	// will return either NULL or an element, if and only if there's just ONE element of this name
	CPPElement* FindElement(ICPPDefinitionsContainerElement* inContainer,const string& inElementName);

	// will return either NULL or an element, if and only if there's just ONE element of this name and of this type
	CPPElement* FindElement(ICPPDefinitionsContainerElement* inContainer,const string& inElementName,CPPElement::ECPPElementType inOfType);

	// will return either NULL or an element, if and only if there's just ONE element of this name and of these types
	CPPElement* FindElement(ICPPDefinitionsContainerElement* inContainer,const string& inElementName,ECPPElementTypeSet inOfTypes);

	Hummus::EStatusCode SkipConstantExpression();

	bool IsTypenamesContainer(CPPElement* inElement);
	string GetNewUnnamedName();

	Hummus::EStatusCode ParseGenericDeclerationStatement(ICPPDeclarationContainerDriver* inContainer);
	Hummus::EStatusCode ParseDeclarators(ICPPDeclarationContainerDriver* inContainer);

	EStatusCodeAndBool ParseFunctionPointerOrFunction(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inReturnTypeModifiersList);
	EStatusCodeAndBool ParseFieldOrFunction(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inFieldModifiersList);
	EStatusCodeAndBool ParseFunctionDefinition(ICPPDeclarationContainerDriver* inContainer,const DeclaratorModifierList& inReturnTypeModifiersList,const string& inFunctionName);
	EStatusCodeAndBool ParseFieldDefinition(ICPPFieldDeclerator* inFieldDeclerator);


	Hummus::EStatusCode SkipInitializer();
	Hummus::EStatusCode SkipBlock();

	void StartLocalContext();
	void EndLocalContext();

};
