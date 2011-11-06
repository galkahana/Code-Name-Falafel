#pragma once

#include "EStatusCode.h"
#include "MapIterator.h"
#include "CPPElement.h"
#include "ICPPContainerElement.h"

#include <string>
#include <map>

using namespace std;

class CPPNamespace;

typedef map<string,CPPNamespace*> StringToCPPNamespaceMap;
typedef map<string,CPPElement*> StringToCPPElementMap;

class CPPNamespace : public CPPElement, public ICPPContainerElement
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

	// ICCPContainerElement implementation
	virtual CPPElement* FindElement(const string& inElementName); 
	virtual Hummus::EStatusCode DefineAlias(const string& inAlias,CPPElement* inNewElement);


private:

	StringToCPPNamespaceMap mSubordinateNamespaces;
	StringToCPPNamespaceMap mNamespaceAliases;
	StringToCPPElementMap mTypenames;

};
