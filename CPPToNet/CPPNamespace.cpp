#include "CPPNamespace.h"
#include "Trace.h"

using namespace Hummus;

CPPNamespace::CPPNamespace(const string& inName):CPPElement(inName,eCPPElementNamespace)
{
}

CPPNamespace::~CPPNamespace(void)
{
	StringToCPPNamespaceMap::iterator itNamespaces = mSubordinateNamespaces.begin();

	for(; itNamespaces != mSubordinateNamespaces.end(); ++itNamespaces)
		delete itNamespaces->second;
}


CPPNamespace* CPPNamespace::CreateNewNamespace(const string& inNamespaceName)
{
	CPPNamespace* newNamespace = GetNamespace(inNamespaceName);
	if(newNamespace)
		return newNamespace;

	newNamespace = mSubordinateNamespaces.insert(StringToCPPNamespaceMap::value_type(inNamespaceName,new CPPNamespace(inNamespaceName))).first->second;

	mTypenames.insert(StringToCPPElementMap::value_type(inNamespaceName,newNamespace));
	return newNamespace;
}

CPPNamespace* CPPNamespace::GetNamespace(const string& inNamespaceName)
{
	StringToCPPNamespaceMap::iterator itNamespaces = mSubordinateNamespaces.find(inNamespaceName);
	if(itNamespaces == mSubordinateNamespaces.end())
	{
		itNamespaces = mNamespaceAliases.find(inNamespaceName);
		if(itNamespaces == mNamespaceAliases.end())
			return NULL;
		else
			return itNamespaces->second;
	}
	else
		return itNamespaces->second;
}	

MapIterator<StringToCPPNamespaceMap> CPPNamespace::GetNamespaceIterator()
{
	return MapIterator<StringToCPPNamespaceMap>(mSubordinateNamespaces);
}

EStatusCode CPPNamespace::CreateNewNamespaceAlias(const string& inNamespaceName,CPPNamespace* inNamespace)
{
	EStatusCode status = DefineAlias(inNamespaceName,inNamespace);
	if(status == eSuccess)
		mNamespaceAliases.insert(StringToCPPNamespaceMap::value_type(inNamespaceName,inNamespace));
	return status;
}

CPPElement* CPPNamespace::FindElement(const string& inElementName)
{
	StringToCPPElementMap::iterator it = mTypenames.find(inElementName);

	if(it == mTypenames.end())
		return NULL;
	else
		return it->second;
}

Hummus::EStatusCode CPPNamespace::DefineAlias(const string& inAlias,CPPElement* inNewElement)
{
	CPPElement* existingAlias = FindElement(inAlias);

	if(existingAlias)
	{
		if(existingAlias != inNewElement)
		{
			TRACE_LOG3("CPPNamespace::DefineAlias, syntax error, alias %s defined for element %s, however it already exists for a different element %s",
						inAlias.c_str(),
						inNewElement->Name.c_str(),
						existingAlias->Name.c_str());
			return eFailure;
		}
	}
	else
	{
		mTypenames.insert(StringToCPPElementMap::value_type(inAlias,inNewElement));
	}
	return eSuccess;	
}

