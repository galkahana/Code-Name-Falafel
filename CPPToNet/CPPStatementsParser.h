#pragma once
#include "IByteReader.h"
#include "PreProcessor.h"
#include "BoxingBase.h"

#include <string>
#include <map>
#include <list>
#include <utility>
#include <set>

using namespace std;

class HeaderUnit;
class CPPNamespace;
class CPPElement;
class ICPPContainerElement;

typedef map<string,string> StringToStringMap;
typedef list<string> StringList;
typedef pair<Hummus::EStatusCode,HeaderUnit*> EStatusCodeAndHeaderUnit;
typedef pair<Hummus::EStatusCode,bool> EStatusCodeAndBool;
typedef list<CPPNamespace*> CPPNamespaceList;
typedef set<CPPNamespace*> CPPNamespaceSet;
typedef list<string> StringList;
typedef list<ICPPContainerElement*> ICPPContainerElementList;


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
	ICPPContainerElementList mDefinitionContextStack;
	LocalContextList mLocalContext;
	Hummus::Long mUnnamedSequance;
	HeaderUnit* mWorkingUnit;
	StringList mTokensStack;
	

	EStatusCodeAndHeaderUnit ParseUnit();
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
	void AddNamespaceToUnqualifiedSearch(CPPNamespace* inNamespace);
	Hummus::EStatusCode SkipSemiColon();
	CPPElement* GetElementFromCurrentLocation();

	BoolAndString GetNextToken();
	void PutBackToken(const string& inToken);

	bool IsTypenamesContainer(CPPElement* inElement);

	void StartLocalContext();
	void EndLocalContext();

};
