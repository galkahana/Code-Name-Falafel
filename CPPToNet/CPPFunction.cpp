#include "CPPFunction.h"

using namespace Hummus;

CPPFunction::CPPFunction(const string& inFunctionName,
						UsedTypeDescriptor* inReturnType,
						const FunctionParameterList& inParametersList,
						bool inHasElipsis,
						bool inIsDefinition) : CPPElement(inFunctionName,CPPElement::eCPPElementFunction)
{
	mReturnType = inReturnType;
	mDeclaredParameters = inParametersList;
	mHasElipsis = inHasElipsis;
	mIsDefinition = inIsDefinition;
}

CPPFunction::~CPPFunction(void)
{
	delete mReturnType;
	FunctionParameterList::iterator it = mDeclaredParameters.begin();	

	for(; it != mDeclaredParameters.end(); ++it)
		delete *it;
}

UsedTypeDescriptor* CPPFunction::GetReturnType()
{
	return mReturnType;
}

SingleValueContainerIterator<FunctionParameterList> CPPFunction::GetParametersListIterator()
{
	return SingleValueContainerIterator<FunctionParameterList>(mDeclaredParameters);
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

FunctionParameterList& CPPFunction::GetDeclaredParameterList()
{
	return mDeclaredParameters;
}
