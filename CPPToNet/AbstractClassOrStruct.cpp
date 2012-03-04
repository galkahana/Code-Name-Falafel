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

// struct/class initialization
AbstractClassOrStruct::AbstractClassOrStruct(
					const string& inName,
					bool inIsDefinition,
					CPPElement::ECPPElementType inType,
					EAbstractClassOrStructAccessLevel inDefaultAccessLevel
					):CPPElement(inName,inType)
{
	SetCommonItems(inIsDefinition,inDefaultAccessLevel);
	mIsTemplate = false;
}

void  AbstractClassOrStruct::SetCommonItems(bool inIsDefinition,EAbstractClassOrStructAccessLevel inDefaultAccessLevel)
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

// template struct/class initialization
AbstractClassOrStruct::AbstractClassOrStruct(
						const string& inName,
						bool inIsDefinition,
						CPPElement::ECPPElementType inType,
						EAbstractClassOrStructAccessLevel inDefaultAccessLevel,
						const CPPElementList& inTemplateParameters):CPPElement(inName,inType)
{
	SetCommonItems(inIsDefinition,inDefaultAccessLevel);
	mIsTemplate = true;
	mTemplateParameters = inTemplateParameters;
}

// template struct/class specialization (either partial or full)
AbstractClassOrStruct::AbstractClassOrStruct(
						const string& inName,
						bool inIsDefinition,
						CPPElement::ECPPElementType inType,
						EAbstractClassOrStructAccessLevel inDefaultAccessLevel,
						const CPPElementList& inTemplateParameters,
						const UsedTypeOrExpressionList& inTemplateParametersAssignments):CPPElement(inName,inType)
{
	SetCommonItems(inIsDefinition,inDefaultAccessLevel);
	mIsTemplate = true;
	mTemplateParameters = inTemplateParameters;
	mTemplateParametersAssignments = inTemplateParametersAssignments;
}

AbstractClassOrStruct::~AbstractClassOrStruct(void)
{
	CPPElementList::iterator itTemplateParameters = mTemplateParameters.begin();
	for(; itTemplateParameters != mTemplateParameters.end(); ++itTemplateParameters)
		delete *itTemplateParameters;


	UsedTypeOrExpressionList::iterator itAssignments = mTemplateParametersAssignments.begin();
	for(; itAssignments != mTemplateParametersAssignments.end(); ++itAssignments)
		delete *itAssignments;
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

EStatusCode AbstractClassOrStruct::AddBase(CPPElement* inBaseClass,EAbstractClassOrStructAccessLevel inAccessLevel)
{
	if(mBaseClasses.find(inBaseClass) == mBaseClasses.end())
	{
		mBaseClasses.insert(CPPElementToEAbstractClassOrStructAccessLevelMap::value_type(inBaseClass,inAccessLevel));
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
										const UsedTypeOrExpressionList& inTemplateSpecializationList,
										bool inIsDefinition,
										CPPFunction* inFunction)
{
	if(inIsPure && !inIsVirtual)
	{
		TRACE_LOG1("AbstractClassOrStruct::AppendFunction, syntax error, pure function defined which is not virtual, for function %s",inFunctionName.c_str());
		return NULL;
	}

	CPPFunction* appendResult = AbstractCPPContainer::AppendFunction(
																	inFunctionName,
																	inIsVirtual,
																	inIsStatic,
																	inReturnTypeDescriptor,
																	inParametersList,
																	inHasElipsis,
																	inIsPure,
																	inTemplateSpecializationList,
																	inIsDefinition,
																	inFunction);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

CPPFunction* AbstractClassOrStruct::AppendFunctionTemplate(const string& inFunctionName,
										bool inIsVirtual,
										bool inIsStatic,											
										UsedTypeDescriptor* inReturnTypeDescriptor,
										const TypedParameterList& inParametersList,
										bool inHasElipsis,		
										bool inIsPure,
										const CPPElementList& inTemplateParameters,
										const UsedTypeOrExpressionList& inTemplateSpecializationList,
										bool inIsDefinition,
										CPPFunction* inFunctionTemplate)
{
	if(inIsPure && !inIsVirtual)
	{
		TRACE_LOG1("AbstractClassOrStruct::AppendFunctionTemplate, syntax error, pure function defined which is not virtual, for function template %s",inFunctionName.c_str());
		return NULL;
	}

	CPPFunction* appendResult = AbstractCPPContainer::AppendFunctionTemplate(inFunctionName,
																			 inIsVirtual,
																			 inIsStatic,
																			 inReturnTypeDescriptor,
																			 inParametersList,
																			 inHasElipsis,
																			 inIsPure,
																			 inTemplateParameters,
																			 inTemplateSpecializationList,
																			 inIsDefinition,
																			 inFunctionTemplate);
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

CPPClass* AbstractClassOrStruct::AppendClassTemplate(
									const string& inClassName,
									const CPPElementList& inTemplateParameters,
									const UsedTypeOrExpressionList& inTemplateAssigmentList,
									bool inIsDefinition,
									CPPClass* inClass)
{
	CPPClass* appendResult = AbstractCPPContainer::AppendClassTemplate(inClassName,inTemplateParameters,inTemplateAssigmentList,inIsDefinition,inClass);
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

CPPStruct* AbstractClassOrStruct::AppendStructTemplate(
									const string& inStructName,
									const CPPElementList& inTemplateParameters,
									const UsedTypeOrExpressionList& inTemplateAssigmentList,
									bool inIsDefinition,
									CPPStruct* inStruct)
{
	CPPStruct* appendResult = AbstractCPPContainer::AppendStructTemplate(inStructName,inTemplateParameters,inTemplateAssigmentList,inIsDefinition,inStruct);
	if(appendResult)
		mAccessDefinition->insert(appendResult);

	return appendResult;
}

bool AbstractClassOrStruct::IsTemplate()
{
	return mIsTemplate;
}

bool AbstractClassOrStruct::IsTemplateSpecializaton()
{
	return mIsTemplate && mTemplateParametersAssignments.size() > 0;
}

Hummus::SingleValueContainerIterator<CPPElementList> AbstractClassOrStruct::GetTemplateParametersIterator()
{
	return Hummus::SingleValueContainerIterator<CPPElementList>(mTemplateParameters);
}

CPPElementList& AbstractClassOrStruct::GetTemplateParameters()
{
	return mTemplateParameters;
}

Hummus::SingleValueContainerIterator<UsedTypeOrExpressionList> AbstractClassOrStruct::GetTemplateParameterAssignmentsIterator()
{
	return Hummus::SingleValueContainerIterator<UsedTypeOrExpressionList>(mTemplateParametersAssignments);
}

UsedTypeOrExpressionList& AbstractClassOrStruct::GetTemplateParameterAssignments()
{
	return mTemplateParametersAssignments;
}

CPPElementList AbstractClassOrStruct::FindElements(const string& inElementName)
{
	if(mIsTemplate)
	{
		CPPElementList result = AbstractCPPContainer::FindElements(inElementName);

		CPPElementList::iterator it = mTemplateParameters.begin();
		for(; it != mTemplateParameters.end(); ++it)
			if((*it)->Name == inElementName)
				result.push_back(*it);
		return result;
	}
	else 	
		return AbstractCPPContainer::FindElements(inElementName);

}
