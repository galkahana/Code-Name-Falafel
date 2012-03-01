#include "FunctionPointerReturnTypeDeclerator.h"
#include "UsedTypeDescriptor.h"

using namespace Hummus;

FunctionPointerReturnTypeDeclerator::FunctionPointerReturnTypeDeclerator(void)
{
	mUsedTypeDescriptor = NULL;

}


FunctionPointerReturnTypeDeclerator::~FunctionPointerReturnTypeDeclerator(void)
{
	delete mUsedTypeDescriptor;
}

ICPPParametersContainer* FunctionPointerReturnTypeDeclerator::GetParametersContainerForFunctionPointer()
{
	return this;
}

void FunctionPointerReturnTypeDeclerator::SetFunctionPointerType(EFunctionPointerType inFunctionPointerType)
{
	mUsedTypeDescriptor->GetFunctionPointerDescriptor()->SetFunctionPointerType(inFunctionPointerType);
}

void FunctionPointerReturnTypeDeclerator::AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers)
{
	mUsedTypeDescriptor->GetFunctionPointerDescriptor()->AppendModifiersForFunctionPointerReturnType(inModifiers);
}

void FunctionPointerReturnTypeDeclerator::SetFunctionPointerHasElipsis()
{
	mUsedTypeDescriptor->GetFunctionPointerDescriptor()->SetFunctionPointerHasElipsis();
}

EStatusCode FunctionPointerReturnTypeDeclerator::FinalizeFunctionPointerDefinition()
{
	// nothing much to do here :) consider removing from common interface
	return eSuccess;
}

EStatusCode FunctionPointerReturnTypeDeclerator::CreateParameter(const string& inParameterName,
																		UsedTypeDescriptor* inParameterType)
{
	return mUsedTypeDescriptor->GetFunctionPointerDescriptor()->CreateParameter(inParameterName,inParameterType);
}

void FunctionPointerReturnTypeDeclerator::SetFlags(CPPElement* inType,bool inIsConst, bool inIsVolatile)
{
	UsedTypeDescriptor* returnTypeDescriptor = new UsedTypeDescriptor(inType,false,false,false,inIsConst,inIsVolatile,false);
	mUsedTypeDescriptor = new UsedTypeDescriptor(returnTypeDescriptor);

}

UsedTypeDescriptor* FunctionPointerReturnTypeDeclerator::DetachUsedTypeDescriptor()
{
	UsedTypeDescriptor* result = mUsedTypeDescriptor;
	mUsedTypeDescriptor = NULL;
	return mUsedTypeDescriptor;
}