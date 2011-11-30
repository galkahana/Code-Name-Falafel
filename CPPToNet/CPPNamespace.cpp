#include "Trace.h"
#include "CPPNamespace.h"
#include "CPPEnumerator.h"
#include "CPPVariable.h"
#include "CPPUnion.h"

using namespace Hummus;

CPPNamespace::CPPNamespace(const string& inName):CPPElement(inName,eCPPElementNamespace)
{
}

CPPNamespace::~CPPNamespace(void)
{
	StringToCPPNamespaceMap::iterator itNamespaces = mSubordinateNamespaces.begin();
	for(; itNamespaces != mSubordinateNamespaces.end(); ++itNamespaces)
		delete itNamespaces->second;

	StringToCPPEnumeratorMap::iterator itEnumerators = mEnumerators.begin();
	for(; itEnumerators != mEnumerators.end(); ++itEnumerators)
		delete itEnumerators->second;

	StringToCPPVariableMap::iterator itVariables = mVariables.begin();
	for(; itVariables != mVariables.end(); ++itVariables)
		delete itVariables->second;

	StringToCPPUnionMap::iterator itUnions = mUnions.begin();
	for(; itUnions != mUnions.end(); ++itUnions)
		delete itUnions->second;

	StringToCPPElementMap::iterator itBasicTypes = mBasicTypes.begin();
	for(; itBasicTypes != mBasicTypes.end(); ++itBasicTypes)
		delete itBasicTypes->second;

}


CPPNamespace* CPPNamespace::CreateNewNamespace(const string& inNamespaceName)
{
	CPPNamespace* newNamespace = GetNamespace(inNamespaceName);
	if(newNamespace)
		return newNamespace;

	newNamespace = mSubordinateNamespaces.insert(StringToCPPNamespaceMap::value_type(inNamespaceName,new CPPNamespace(inNamespaceName))).first->second;

	mTypenames.insert(StringToCPPElementMap::value_type(inNamespaceName,newNamespace));
	mDefinitions.insert(StringToCPPElementMap::value_type(inNamespaceName,newNamespace));

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
	StringToCPPElementMap::iterator it = mDefinitions.find(inElementName);

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
		if(inNewElement->IsType())
			mTypenames.insert(StringToCPPElementMap::value_type(inAlias,inNewElement));
		else
			mValueItems.insert(StringToCPPElementMap::value_type(inAlias,inNewElement));
	}
	return eSuccess;	
}


CPPEnumerator* CPPNamespace::CreateEnumerator(const string& inEnumeratorName)
{
	CPPElement* existingElement = FindElement(inEnumeratorName);

	if(existingElement)
	{
		TRACE_LOG1("CPPNamespace::CreateEnumerator, syntax error, typename already exists for %s",
					inEnumeratorName.c_str());
		return NULL;
	}
	else
	{
		CPPEnumerator* newEnumerator = new CPPEnumerator(inEnumeratorName);
		mEnumerators.insert(StringToCPPEnumeratorMap::value_type(inEnumeratorName,newEnumerator));
		mTypenames.insert(StringToCPPElementMap::value_type(inEnumeratorName,newEnumerator));
		mDefinitions.insert(StringToCPPElementMap::value_type(inEnumeratorName,newEnumerator));

		return newEnumerator;
	}
}

CPPEnumeratorValue* CPPNamespace::CreateEnumeratorValue(CPPEnumerator* inEnumerator,const string& inEnumeratorValueName)
{

	CPPElement* existingElement = FindElement(inEnumeratorValueName);

	if(existingElement)
	{
		TRACE_LOG1("CPPNamespace::CreateEnumeratorValue, syntax error, enumerator value is already defined for %s",
					inEnumeratorValueName.c_str());
		return NULL;
	}
	else
	{
		CPPEnumeratorValue* enumeratorValue = inEnumerator->AddEnumeratorValue(inEnumeratorValueName);

		mTypenames.insert(StringToCPPElementMap::value_type(inEnumeratorValueName,enumeratorValue));
		mValueItems.insert(StringToCPPElementMap::value_type(inEnumeratorValueName,enumeratorValue));
		mDefinitions.insert(StringToCPPElementMap::value_type(inEnumeratorValueName,enumeratorValue));


		return enumeratorValue;
	}
}

CPPVariable* CPPNamespace::CreateVariable(const string& inVariableName,
										  CPPElement* inType,
										  bool inIsAuto,
										  bool inIsRegister,
										  bool inIsExtern,
										  bool inIsConst,
										  bool inIsVolatile)
{
	CPPElement* existingElement = FindElement(inVariableName);

	if(existingElement)
	{
		TRACE_LOG1("CPPNamespace::CreateVariable, syntax error, variable value is already defined for %s",
					inVariableName.c_str());
		return NULL;
	}
	else
	{
		CPPVariable* aVariable = new CPPVariable(inType,inVariableName,inIsAuto,inIsRegister,inIsExtern,inIsConst,inIsVolatile);

		mVariables.insert(StringToCPPVariableMap::value_type(inVariableName,aVariable));
		mTypenames.insert(StringToCPPElementMap::value_type(inVariableName,aVariable));
		mValueItems.insert(StringToCPPElementMap::value_type(inVariableName,aVariable));
		mDefinitions.insert(StringToCPPElementMap::value_type(inVariableName,aVariable));

		return aVariable;
	}	
}

CPPUnion* CPPNamespace::CreateUnion(const string& inUnionName)
{
	CPPElement* existingElement = FindElement(inUnionName);

	if(existingElement)
	{
		TRACE_LOG1("CPPNamespace::CreateUnion, syntax error, typename already exists for %s",
					inUnionName.c_str());
		return NULL;
	}
	else
	{
		CPPUnion* newUnion = new CPPUnion(inUnionName);
		mUnions.insert(StringToCPPUnionMap::value_type(inUnionName,newUnion));
		mTypenames.insert(StringToCPPElementMap::value_type(inUnionName,newUnion));
		mDefinitions.insert(StringToCPPElementMap::value_type(inUnionName,newUnion));

		return newUnion;
	}
}

EStatusCode CPPNamespace::AppendBasicType(const string& inTypeName,CPPElement* inNewType)
{
	CPPElement* existingElement = FindElement(inTypeName);

	if(existingElement)
	{
		TRACE_LOG1("CPPNamespace::AppendBasicType, syntax error, typename already exists for %s",
					inTypeName.c_str());
		return eFailure;
	}
	else
	{
		mBasicTypes.insert(StringToCPPElementMap::value_type(inTypeName,inNewType));
		mTypenames.insert(StringToCPPElementMap::value_type(inTypeName,inNewType));
		mDefinitions.insert(StringToCPPElementMap::value_type(inTypeName,inNewType));

		return eSuccess;
	}
}
