#include "CPPFunction.h"
#include "UsedTypeDescriptor.h"

using namespace Hummus;

CPPFunction::CPPFunction(const string& inFunctionName,
						bool inIsVirtual,
						bool inIsStatic,
						UsedTypeDescriptor* inReturnType,
						const FunctionParameterList& inParametersList,
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
