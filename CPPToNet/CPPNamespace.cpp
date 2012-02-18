#include "Trace.h"
#include "CPPNamespace.h"
#include "CPPPrimitive.h"

using namespace Hummus;

CPPNamespace::CPPNamespace(const string& inName):CPPElement(inName,eCPPElementNamespace)
{
}

CPPNamespace::~CPPNamespace(void)
{
	StringToCPPNamespaceMap::iterator itNamespaces = mSubordinateNamespaces.begin();
	for(; itNamespaces != mSubordinateNamespaces.end(); ++itNamespaces)
		if(mAliases.find(itNamespaces->second) == mAliases.end())
			delete itNamespaces->second;

	StringToCPPPrimitiveMap::iterator itPrimitives = mPrimitives.begin();
	for(; itPrimitives != mPrimitives.end(); ++itPrimitives)
		if(mAliases.find(itPrimitives->second) == mAliases.end())
			delete itPrimitives->second;

}



CPPNamespace* CPPNamespace::CreateNewNamespace(const string& inNamespaceName)
{
	return AppendNewNamespace(inNamespaceName,NULL);
}

CPPNamespace* CPPNamespace::AppendNewNamespace(const string& inNamespaceName,CPPNamespace* inNewNamespace)
{
	CPPNamespace* newNamespace = GetNamespace(inNamespaceName);
	if(newNamespace)
		return newNamespace;

	// namespaces cannot share the name with anything else (much like primitives), so if creating a nwe one, make sure not
	// to create one with a name that already exists. note that at this point, if something is found, it is surely not a namespace

	CPPElementList existingElements = FindElements(inNamespaceName);
	if(existingElements.size() > 0)
		return NULL;
	else
		return mSubordinateNamespaces.insert(StringToCPPNamespaceMap::value_type(inNamespaceName,inNewNamespace ? inNewNamespace:new CPPNamespace(inNamespaceName))).first->second;
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
	// namespace aliases, in definition, work a lot like regular namespaces, not allowing other names, unless they are the identical namespace
	CPPNamespace* existingNamespace = GetNamespace(inNamespaceName);
	
	if(existingNamespace)
	{
		if(existingNamespace == inNamespace)
		{
			return eSuccess;
		}
		else
		{
			TRACE_LOG1("CPPNamespace::CreateNewNamespaceAlias, error in defining a namespace alias. a namespace already exists with the same name...but it's not the same namespace, for %s",inNamespaceName.c_str());
			return eFailure;
		}
	}
	else
	{
		CPPElementList existingElements = FindElements(inNamespaceName);
		if(existingElements.size() > 0)
		{
			return eFailure;
		}
		else
		{
			mNamespaceAliases.insert(StringToCPPNamespaceMap::value_type(inNamespaceName,inNamespace));
			return eSuccess;
		}
	}
}

CPPElementList CPPNamespace::FindElements(const string& inElementName)
{
	CPPElementList result = AbstractCPPContainer::FindElements(inElementName);

	StringToCPPNamespaceMap::iterator itNamespaces = mSubordinateNamespaces.find(inElementName);
	if(itNamespaces != mSubordinateNamespaces.end())
		result.push_back(itNamespaces->second);

	StringToCPPNamespaceMap::iterator itNamespacesAliases = mNamespaceAliases.find(inElementName);
	if(itNamespacesAliases != mNamespaceAliases.end())
		result.push_back(itNamespacesAliases->second);


	StringToCPPPrimitiveMap::iterator itPrimitives = mPrimitives.begin();
	if(itPrimitives != mPrimitives.end())
		result.push_back(itPrimitives->second);

	return result;
}

Hummus::EStatusCode CPPNamespace::DefineAlias(const string& inAlias,CPPElement* inNewElement)
{
	EStatusCode status = eSuccess;

	switch(inNewElement->Type)
	{
		case eCPPElementPrimitive:
			{
				status = AppendPrimitive(inAlias,(CPPPrimitive*)inNewElement);
				if(eSuccess == status)
					mAliases.insert(inNewElement);
				break;
			}
		case eCPPElementNamespace:
			{
				CPPNamespace* createdElement = AppendNewNamespace(inAlias,(CPPNamespace*)inNewElement);
			
				if(createdElement != inNewElement)
				{
					TRACE_LOG1("CPPNamespace::DefineAlias, failed to define new namespace alias %s . an incompatible element already exists.",inAlias.c_str());
					status = eFailure;
					
				}
				else
					mAliases.insert(inNewElement);
				break;
			}
		default:
			{
				status = AbstractCPPContainer::DefineAlias(inAlias,inNewElement);
			}
	}
	return status;	
}


EStatusCode CPPNamespace::AppendPrimitive(const string& inTypeName,CPPPrimitive* inNewType)
{
	CPPElementList existingElements = FindElements(inTypeName);

	// k. primitive never allow anything else with the same name. m'k?

	if(existingElements.size() > 0)
	{
		TRACE_LOG1("CPPNamespace::AppendPrimitive, syntax error, typename already exists for %s",
					inTypeName.c_str());
		return eFailure;
	}
	else
	{
		mPrimitives.insert(StringToCPPPrimitiveMap::value_type(inTypeName,inNewType));
		return eSuccess;
	}
}


CPPFunction* CPPNamespace::AppendFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,											
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const FunctionParameterList& inParametersList,
										bool inHasElipsis,	
										bool inIsPure,
										bool inIsDefinition,
										CPPFunction* inFunction)
{
	if(inIsVirtual || inIsPure)
	{
		TRACE_LOG1("CPPNamespace::AppendFunction, syntax error,cannot have virtual  or pure specified on a non-class method %s",
					inFunctionName.c_str());	
		return NULL;
	}
	else
		return AbstractCPPContainer::AppendFunction(inFunctionName,false,inIsStatic,inReturnTypeDescriptor,inParametersList,
																		inHasElipsis,inIsPure,inIsDefinition,inFunction);
}
