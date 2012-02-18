#include "CPPClass.h"
#include "Trace.h"
#include "CPPEnumerator.h"
#include "CPPVariable.h"
#include "CPPUnion.h"
#include "CPPTypedef.h"
#include "CPPFunction.h"

using namespace Hummus;

CPPClass::CPPClass(const string& inName,
					bool inIsDefinition):CPPElement(inName,eCPPElementNamespace)
{
	mIsDefinition = inIsDefinition;
	mAccessDefinition = &mPrivateDefinitions;
}


CPPClass::~CPPClass(void)
{
}

bool CPPClass::IsDefinition()
{
	return mIsDefinition;
}

void CPPClass::SetIsDefinition()
{
	mIsDefinition = true;
}

void CPPClass::SetProtectedAccessLevel()
{
	mAccessDefinition = &mProtectedDefinitions;
}

void CPPClass::SetPrivateAccessLevel()
{
	mAccessDefinition = &mPrivateDefinitions;
}

void CPPClass::SetPublicAccessLevel()
{
	mAccessDefinition = &mPublicDefinitions;
}

EStatusCode CPPClass::AddBaseClass(CPPClass* inBaseClass,ECPPClassAccessLevel inAccessLevel)
{
	if(mBaseClasses.find(inBaseClass) == mBaseClasses.end())
	{
		mBaseClasses.insert(CPPClassToECPPClassAccessLevelMap::value_type(inBaseClass,inAccessLevel));
		return eSuccess;
	}
	else
	{
		TRACE_LOG1("CPPClass::AddBaseClass, syntax error, base class %s is derived from multipe times",inBaseClass->Name);
		return eFailure;
	}
}

CPPEnumerator* CPPClass::AppendEnumerator(const string& inEnumeratorName,bool inIsDefinition,CPPEnumerator* inEnumerator)
{
	CPPEnumerator* appendResult = AbstractCPPContainer::AppendEnumerator(inEnumeratorName,inIsDefinition,inEnumerator);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPUnion* CPPClass::AppendUnion(const string& inUnionName,bool inIsDefinition,CPPUnion* inUnion)
{
	CPPUnion* appendResult = AbstractCPPContainer::AppendUnion(inUnionName,inIsDefinition,inUnion);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPTypedef* CPPClass::AppendTypedef(const string& inTypedefName,
									UsedTypeDescriptor* inTypeDescriptor,
									CPPTypedef* inTypedef)
{
	CPPTypedef* appendResult = AbstractCPPContainer::AppendTypedef(inTypedefName,inTypeDescriptor,inTypedef);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPVariable* CPPClass::AppendVariable(const string& inVariableName,
										UsedTypeDescriptor* inTypeDescriptor,
										CPPVariable* inVariable)
{
	CPPVariable* appendResult = AbstractCPPContainer::AppendVariable(inVariableName,inTypeDescriptor,inVariable);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPFunction* CPPClass::AppendFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,											
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const FunctionParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsPure,
										bool inIsDefinition,
										CPPFunction* inFunction)
{
	if(inIsPure && !inIsVirtual)
	{
		TRACE_LOG1("CPPClass::AppendFunction, syntax error, pure function defined which is not virtual, for function %s",inFunctionName.c_str());
		return NULL;
	}

	CPPFunction* appendResult = AbstractCPPContainer::AppendFunction(inFunctionName,inIsVirtual,inIsStatic,inReturnTypeDescriptor,inParametersList,
																	inHasElipsis,inIsPure,inIsDefinition,inFunction);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPClass* CPPClass::AppendClass(const string& inClassName,
								bool inIsDefinition,
								CPPClass* inClass)
{
	CPPClass* appendResult = AbstractCPPContainer::AppendClass(inClassName,inIsDefinition,inClass);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}
