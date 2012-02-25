#include "AbstractClassOrStruct.h"
#include "Trace.h"
#include "CPPEnumerator.h"
#include "CPPVariable.h"
#include "CPPUnion.h"
#include "CPPTypedef.h"
#include "CPPFunction.h"
#include "CPPClass.h"
#include "CPPStruct.h"

using namespace Hummus;

AbstractClassOrStruct::AbstractClassOrStruct(const string& inName,
					bool inIsDefinition,
					CPPElement::ECPPElementType inType,
					EAbstractClassOrStructAccessLevel inDefaultAccessLevel
					):CPPElement(inName,inType)
{
	mIsDefinition = inIsDefinition;
	switch(inDefaultAccessLevel)
	{
		case eAbstractClassOrStructAccessLevelPrivate:
			mAccessDefinition = &mPrivateDefinitions;
			break;
		case eAbstractClassOrStructAccessLevelProtected:
			mAccessDefinition = &mProtectedDefinitions;
			break;
		case eAbstractClassOrStructAccessLevelPublic:
			mAccessDefinition = &mPublicDefinitions;
			break;
	}
}


AbstractClassOrStruct::~AbstractClassOrStruct(void)
{
}

bool AbstractClassOrStruct::IsDefinition()
{
	return mIsDefinition;
}

void AbstractClassOrStruct::SetIsDefinition()
{
	mIsDefinition = true;
}

void AbstractClassOrStruct::SetProtectedAccessLevel()
{
	mAccessDefinition = &mProtectedDefinitions;
}

void AbstractClassOrStruct::SetPrivateAccessLevel()
{
	mAccessDefinition = &mPrivateDefinitions;
}

void AbstractClassOrStruct::SetPublicAccessLevel()
{
	mAccessDefinition = &mPublicDefinitions;
}

EStatusCode AbstractClassOrStruct::AddBaseClassOrStruct(AbstractClassOrStruct* inBaseClass,EAbstractClassOrStructAccessLevel inAccessLevel)
{
	if(mBaseClasses.find(inBaseClass) == mBaseClasses.end())
	{
		mBaseClasses.insert(AbstractClassOrStructToEAbstractClassOrStructAccessLevelMap::value_type(inBaseClass,inAccessLevel));
		return eSuccess;
	}
	else
	{
		TRACE_LOG1("AbstractClassOrStruct::AddBaseClassOrStruct, syntax error, base class %s is derived from multipe times",inBaseClass->Name);
		return eFailure;
	}
}

CPPEnumerator* AbstractClassOrStruct::AppendEnumerator(const string& inEnumeratorName,bool inIsDefinition,CPPEnumerator* inEnumerator)
{
	CPPEnumerator* appendResult = AbstractCPPContainer::AppendEnumerator(inEnumeratorName,inIsDefinition,inEnumerator);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPUnion* AbstractClassOrStruct::AppendUnion(const string& inUnionName,bool inIsDefinition,CPPUnion* inUnion)
{
	CPPUnion* appendResult = AbstractCPPContainer::AppendUnion(inUnionName,inIsDefinition,inUnion);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPTypedef* AbstractClassOrStruct::AppendTypedef(const string& inTypedefName,
									UsedTypeDescriptor* inTypeDescriptor,
									CPPTypedef* inTypedef)
{
	CPPTypedef* appendResult = AbstractCPPContainer::AppendTypedef(inTypedefName,inTypeDescriptor,inTypedef);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPVariable* AbstractClassOrStruct::AppendVariable(const string& inVariableName,
										UsedTypeDescriptor* inTypeDescriptor,
										CPPVariable* inVariable)
{
	CPPVariable* appendResult = AbstractCPPContainer::AppendVariable(inVariableName,inTypeDescriptor,inVariable);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPFunction* AbstractClassOrStruct::AppendFunction(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,											
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsPure,
										bool inIsDefinition,
										CPPFunction* inFunction)
{
	if(inIsPure && !inIsVirtual)
	{
		TRACE_LOG1("AbstractClassOrStruct::AppendFunction, syntax error, pure function defined which is not virtual, for function %s",inFunctionName.c_str());
		return NULL;
	}

	CPPFunction* appendResult = AbstractCPPContainer::AppendFunction(inFunctionName,inIsVirtual,inIsStatic,inReturnTypeDescriptor,inParametersList,
																	inHasElipsis,inIsPure,inIsDefinition,inFunction);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPClass* AbstractClassOrStruct::AppendClass(const string& inClassName,
								bool inIsDefinition,
								CPPClass* inClass)
{
	CPPClass* appendResult = AbstractCPPContainer::AppendClass(inClassName,inIsDefinition,inClass);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}


CPPStruct* AbstractClassOrStruct::AppendStruct(const string& inStructName,
								bool inIsDefinition,
								CPPStruct* inStruct)
{
	CPPStruct* appendResult = AbstractCPPContainer::AppendStruct(inStructName,inIsDefinition,inStruct);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}
