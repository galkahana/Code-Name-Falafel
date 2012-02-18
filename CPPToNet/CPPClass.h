#pragma once

#include "CPPElement.h"
#include "AbstractCPPContainer.h"

#include <set>
#include <utility>

using namespace std;

enum ECPPClassAccessLevel
{
	eCPPClassAccessLevelPrivate,
	eCPPClassAccessLevelProtected,
	eCPPClassAccessLevelPublic
};

class CPPClass;

typedef set<CPPElement*> CPPElementSet;
typedef map<CPPClass*,ECPPClassAccessLevel> CPPClassToECPPClassAccessLevelMap;

class CPPClass : public CPPElement, public AbstractCPPContainer
{
public:



	CPPClass(const string& inName,
				bool inIsDefinition);
	virtual ~CPPClass(void);


	bool IsDefinition();

	// update function flag, when function defintion is found
	void SetIsDefinition();

	// setup base class, will fail if class is already defined as derived
	Hummus::EStatusCode AddBaseClass(CPPClass* inBaseClass,ECPPClassAccessLevel inAccessLevel);


	// when parsing, set the current access level, for any definition
	void SetProtectedAccessLevel();
	void SetPrivateAccessLevel();
	void SetPublicAccessLevel();

protected:

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
											const FunctionParameterList& inParametersList,
											bool inHasElipsis,		
											bool inIsDefinition,
											bool inIsPure,
											CPPFunction* inFunction);
	virtual CPPClass* AppendClass(const string& inClassName,
									bool inIsDefinition,
									CPPClass* inClass);
private:
	bool mIsDefinition;

	CPPElementSet* mAccessDefinition;
	CPPElementSet mPrivateDefinitions;
	CPPElementSet mProtectedDefinitions;
	CPPElementSet mPublicDefinitions;
	CPPClassToECPPClassAccessLevelMap mBaseClasses;

};

