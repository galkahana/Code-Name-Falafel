#include "UsedTypeDescriptor.h"

using namespace Hummus;

FieldTypeDescriptor::FieldTypeDescriptor(CPPElement* inType, 
										bool inIsAuto,
										bool inIsRegister,
										bool inIsExtern,
										bool inIsConst,
										bool inIsVolatile,
										bool inIsStatic)
{
	mType = inType;
	mSubscriptCount = 0;
	IsAuto = inIsAuto;
	IsRegister = inIsRegister;
	IsExtern = inIsExtern;
	IsConst = inIsConst;
	IsVolatile = inIsVolatile;
	IsStatic = inIsStatic;	
}

SingleValueContainerIterator<DeclaratorModifierList> FieldTypeDescriptor::GetModifiersListIterator()
{
	return SingleValueContainerIterator<DeclaratorModifierList>(mModifiers);
}

unsigned long FieldTypeDescriptor::GetSubscriptCount()
{
	return mSubscriptCount;
}

CPPElement* FieldTypeDescriptor::GetType()
{
	return mType;
}

void FieldTypeDescriptor::AppendModifiers(const DeclaratorModifierList& inModifiers)
{
	mModifiers.insert(mModifiers.end(),inModifiers.begin(),inModifiers.end());
}

void FieldTypeDescriptor::AddSubscript()
{
	++mSubscriptCount;
}

FunctionParameter::FunctionParameter()
{
	Type = NULL;
}

FunctionParameter::~FunctionParameter()
{
	delete Type;
}

FunctionPointerTypeDescriptor::FunctionPointerTypeDescriptor(UsedTypeDescriptor* inReturnType)
{
	mReturnType = inReturnType;
	mHasElipsis = false;
}

FunctionPointerTypeDescriptor::~FunctionPointerTypeDescriptor()
{
	delete mReturnType;
	FunctionParameterList::iterator it = mDeclaredParameters.begin();	

	for(; it != mDeclaredParameters.end(); ++it)
		delete *it;
}

void FunctionPointerTypeDescriptor::SetFunctionPointerType(ICPPFunctionPointerDeclerator::EFunctionPointerType inFunctionPointerType)
{
	mPointerType = inFunctionPointerType;
}

void FunctionPointerTypeDescriptor::AppendModifiersForReturnType(const DeclaratorModifierList& inModifiers)
{
	mReturnType->GetFieldDescriptor()->AppendModifiers(inModifiers);
}


FunctionParameter* FunctionPointerTypeDescriptor::CreateParameter(const string& inParameterName,UsedTypeDescriptor* inParameterType)
{
	FunctionParameter* newParameter = new FunctionParameter();
	newParameter->Type = inParameterType;
	newParameter->Name = inParameterName;

	mDeclaredParameters.push_back(newParameter);
	return newParameter;
}

ICPPFunctionPointerDeclerator::EFunctionPointerType FunctionPointerTypeDescriptor::GetPointerType()
{
	return mPointerType;
}

UsedTypeDescriptor* FunctionPointerTypeDescriptor::GetReturnType()
{
	return mReturnType;
}

Hummus::SingleValueContainerIterator<FunctionParameterList> FunctionPointerTypeDescriptor::GetParametersListIterator()
{
	return Hummus::SingleValueContainerIterator<FunctionParameterList>(mDeclaredParameters);
}

void FunctionPointerTypeDescriptor::SetHasElipsis()
{
	mHasElipsis = true;
}

bool FunctionPointerTypeDescriptor::HasElipsis()
{
	return mHasElipsis;
}


UsedTypeDescriptor::UsedTypeDescriptor(CPPElement* inType, 
										bool inIsAuto,
										bool inIsRegister,
										bool inIsExtern,
										bool inIsConst,
										bool inIsVolatile,
										bool inIsStatic)
{
	mUsedType = eUsedTypeField;
	mFieldDescriptor = new FieldTypeDescriptor(inType,inIsAuto,inIsRegister,inIsExtern,inIsConst,inIsVolatile,inIsStatic);
	mFunctionPointerDescriptor = NULL;	
}

UsedTypeDescriptor::UsedTypeDescriptor(UsedTypeDescriptor* inReturnType)
{
	mUsedType = eUsedTypeFunctionPointer;
	mFieldDescriptor = NULL;
	mFunctionPointerDescriptor = new FunctionPointerTypeDescriptor(inReturnType);
}


UsedTypeDescriptor::~UsedTypeDescriptor()
{
	delete mFieldDescriptor;
	delete mFunctionPointerDescriptor;
}

UsedTypeDescriptor::EUsedType UsedTypeDescriptor::GetUsedType()
{
	return mUsedType;
}

FieldTypeDescriptor* UsedTypeDescriptor::GetFieldDescriptor()
{
	return mFieldDescriptor;
}

FunctionPointerTypeDescriptor* UsedTypeDescriptor::GetFunctionPointerDescriptor()
{
	return mFunctionPointerDescriptor;
}


