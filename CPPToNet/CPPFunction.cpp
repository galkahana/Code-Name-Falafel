#include "CPPFunction.h"
#include "UsedTypeDescriptor.h"

using namespace Hummus;

CPPFunction::CPPFunction(const string& inFunctionName,
						bool inIsVirtual,
						bool inIsStatic,
						UsedTypeDescriptor* inReturnType,
						const TypedParameterList& inParametersList,
						bool inHasElipsis,
						bool inIsPure,
						bool inIsDefinition) : CPPElement(inFunctionName,CPPElement::eCPPElementFunction)
{
	mIsVirtual = inIsVirtual;
	mIsStatic = inIsStatic;
	mReturnType = inReturnType;
	mDeclaredParameters = inParametersList;
	mHasElipsis = inHasElipsis;
	mIsDefinition = inIsDefinition;
	mIsPure = inIsPure;
	mIsTemplate = false;
	mIsTemplateInsantiation = false;
}

CPPFunction::CPPFunction(const string& inFunctionName,
				bool inIsVirtual,
				bool inIsStatic,
				UsedTypeDescriptor* inReturnType,
				const TypedParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsPure,
				bool inIsDefinition,
				const CPPElementList& inTemplateParameters) : CPPElement(inFunctionName,CPPElement::eCPPElementFunction)
{
	mIsVirtual = inIsVirtual;
	mIsStatic = inIsStatic;
	mReturnType = inReturnType;
	mDeclaredParameters = inParametersList;
	mHasElipsis = inHasElipsis;
	mIsDefinition = inIsDefinition;
	mIsPure = inIsPure;
	mIsTemplate = true;
	mIsTemplateInsantiation = false;
	mTemplateParameters = inTemplateParameters;
}

CPPFunction::CPPFunction(const string& inFunctionName,
				bool inIsVirtual,
				bool inIsStatic,
				UsedTypeDescriptor* inReturnType,
				const TypedParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsPure,
				bool inIsDefinition,
				const UsedTypeOrExpressionList& inTemplateSpecializationParameters) : CPPElement(inFunctionName,CPPElement::eCPPElementFunction)
{
	mIsVirtual = inIsVirtual;
	mIsStatic = inIsStatic;
	mReturnType = inReturnType;
	mDeclaredParameters = inParametersList;
	mHasElipsis = inHasElipsis;
	mIsDefinition = inIsDefinition;
	mIsPure = inIsPure;
	mIsTemplate = false;
	mIsTemplateInsantiation = true;
	mTemplateSpecializationParameters = inTemplateSpecializationParameters;
}

CPPFunction::CPPFunction(const string& inFunctionName,
				bool inIsVirtual,
				bool inIsStatic,
				UsedTypeDescriptor* inReturnType,
				const TypedParameterList& inParametersList,
				bool inHasElipsis,
				bool inIsPure,
				bool inIsDefinition,
				const CPPElementList& inTemplateParameters,
				const UsedTypeOrExpressionList& inTemplateSpecializationParameters) : CPPElement(inFunctionName,CPPElement::eCPPElementFunction)
{
	mIsVirtual = inIsVirtual;
	mIsStatic = inIsStatic;
	mReturnType = inReturnType;
	mDeclaredParameters = inParametersList;
	mHasElipsis = inHasElipsis;
	mIsDefinition = inIsDefinition;
	mIsPure = inIsPure;
	mIsTemplate = true;
	mIsTemplateInsantiation = false;
	mTemplateParameters = inTemplateParameters;
	mTemplateSpecializationParameters = inTemplateSpecializationParameters;
}

CPPFunction::~CPPFunction(void)
{
	delete mReturnType;
	TypedParameterList::iterator itParameters = mDeclaredParameters.begin();	

	for(; itParameters != mDeclaredParameters.end(); ++itParameters)
		delete *itParameters;

	CPPElementList::iterator itTemplateParameters = mTemplateParameters.begin();
	for(; itTemplateParameters != mTemplateParameters.end(); ++itTemplateParameters)
		delete *itTemplateParameters;


	UsedTypeOrExpressionList::iterator itSpecialization = mTemplateSpecializationParameters.begin();
	for(; itSpecialization != mTemplateSpecializationParameters.end(); ++itSpecialization)
		delete *itSpecialization;
}

UsedTypeDescriptor* CPPFunction::GetReturnType()
{
	return mReturnType;
}

SingleValueContainerIterator<TypedParameterList> CPPFunction::GetParametersListIterator()
{
	return SingleValueContainerIterator<TypedParameterList>(mDeclaredParameters);
}

bool CPPFunction::HasElipsis()
{
	return mHasElipsis;
}

bool CPPFunction::IsDefinition()
{
	return mIsDefinition;
}

void CPPFunction::SetIsDefinition()
{
	mIsDefinition = true;
}

TypedParameterList& CPPFunction::GetDeclaredParameterList()
{
	return mDeclaredParameters;
}

bool CPPFunction::IsVirtual()
{
	return mIsVirtual;
}

bool CPPFunction::IsStatic()
{
	return mIsStatic;
}

bool CPPFunction::IsPure()
{
	return mIsPure;
}

bool CPPFunction::IsFunctionTemplate()
{
	return mIsTemplate;
}

CPPElementList& CPPFunction::GetFunctionTemplateParameters()
{
	return mTemplateParameters;
}

Hummus::SingleValueContainerIterator<CPPElementList> CPPFunction::GetFunctionTemplateParametersIterator()
{
	return Hummus::SingleValueContainerIterator<CPPElementList>(mTemplateParameters);
}

Hummus::SingleValueContainerIterator<UsedTypeOrExpressionList> CPPFunction::GetFunctionTemplateSpecializationParametersIterator()
{
	return Hummus::SingleValueContainerIterator<UsedTypeOrExpressionList>(mTemplateSpecializationParameters);
}

UsedTypeOrExpressionList& CPPFunction::GetFunctionTemplateSpecializationParametrs()
{
	return mTemplateSpecializationParameters;
}

bool CPPFunction::IsTemplateInsantiation()
{
	return mIsTemplateInsantiation;
}
