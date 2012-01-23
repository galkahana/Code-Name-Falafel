#include "UsedTypeDescriptor.h"
#include "CPPElement.h"
#include "Trace.h"
#include "CPPTypedef.h"

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

void FieldTypeDescriptor::AppendModifier(const DeclaratorModifier& inModifier)
{
	mModifiers.push_back(inModifier);
}

void FieldTypeDescriptor::AppendModifiers(const DeclaratorModifierList& inModifiers)
{
	mModifiers.insert(mModifiers.end(),inModifiers.begin(),inModifiers.end());
}

void FieldTypeDescriptor::AddSubscript()
{
	++mSubscriptCount;
}

void FieldTypeDescriptor::RemoveSubscript()
{
	--mSubscriptCount;
}

size_t FieldTypeDescriptor::GetModifiersCount()
{
	return mModifiers.size();
}

bool FieldTypeDescriptor::IsEqual(FieldTypeDescriptor* inOtherDescriptor)
{
	if(IsAuto != inOtherDescriptor->IsAuto)
		return false;

	if(IsRegister != inOtherDescriptor->IsRegister)
		return false;

	if(IsExtern != inOtherDescriptor->IsExtern)
		return false;

	if(IsConst != inOtherDescriptor->IsConst)
		return false;

	if(IsVolatile != inOtherDescriptor->IsVolatile)
		return false;

	if(IsStatic != inOtherDescriptor->IsStatic)
		return false;

	if(mModifiers.size() != inOtherDescriptor->mModifiers.size())
		return false;

	DeclaratorModifierList::iterator itThis = mModifiers.begin();
	DeclaratorModifierList::iterator itOther = inOtherDescriptor->mModifiers.begin();

	bool isEqual = true;
	for(; itThis != mModifiers.end() && isEqual; ++itThis,++itOther)
		isEqual = itThis->IsEqual(*itOther);

	if(mSubscriptCount != inOtherDescriptor->mSubscriptCount)
		return false;

	// k. now let's compare the type. to save me some work, i'll make sure to do equality only for what is actually types
	// at this point we have : 	eCPPElementPrimitive, eCPPElementEnumerator, eCPPElementUnion, eCPPElementTypedef
	// that can serve as types
	if(mType->Type != inOtherDescriptor->mType->Type)
		return false;

	// typedefs should be internally compared, being only aliases. anything else, should just have pointer comparison
	if(mType->Type == CPPElement::eCPPElementTypedef)
		return ((CPPTypedef*)mType)->IsEqual((CPPTypedef*)inOtherDescriptor->mType);
	else
		return mType == mType;
	return isEqual;
}

FieldTypeDescriptor* FieldTypeDescriptor::Clone()
{
	FieldTypeDescriptor* result = new FieldTypeDescriptor(mType,IsAuto,IsRegister,IsExtern,IsConst,IsVolatile,IsStatic);
	result->mModifiers = mModifiers;
	result->mSubscriptCount = mSubscriptCount;
	return result;
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

void FunctionPointerTypeDescriptor::AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers)
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

void FunctionPointerTypeDescriptor::SetFunctionPointerHasElipsis()
{
	mHasElipsis = true;
}

bool FunctionPointerTypeDescriptor::HasElipsis()
{
	return mHasElipsis;
}

bool FunctionPointerTypeDescriptor::IsEqual(FunctionPointerTypeDescriptor* inOther)
{
	if(mPointerType != inOther->GetPointerType())
		return false;

	if(!mReturnType->IsEqual(inOther->GetReturnType()))
		return false;

	if(mDeclaredParameters.size() != mDeclaredParameters.size())
		return false;

	FunctionParameterList::iterator itThis,itOther;
	itThis = mDeclaredParameters.begin();
	itOther = inOther->mDeclaredParameters.begin();
	bool isEqual = true;

	// with parameters comparison, only compare the parameter types, because the names
	// don't matter
	for(; itThis != mDeclaredParameters.end() && isEqual; ++itThis,++itOther)
		isEqual = (*itThis)->Type->IsEqual((*itOther)->Type);

	if(!isEqual)
		return false;

	return mHasElipsis == inOther->HasElipsis();
}

FunctionPointerTypeDescriptor* FunctionPointerTypeDescriptor::Clone()
{
	// declared parameters and return type should be cloned
	FunctionPointerTypeDescriptor* result = new FunctionPointerTypeDescriptor(mReturnType->Clone());
	result->mPointerType = mPointerType;
	FunctionParameterList::iterator itParameters = mDeclaredParameters.begin();
	for(; itParameters != mDeclaredParameters.end(); ++itParameters)
		result->CreateParameter((*itParameters)->Name,(*itParameters)->Type->Clone());
	result->mHasElipsis = mHasElipsis;
	return result;
}

UsedTypeDescriptor::UsedTypeDescriptor()
{
	mUsedType = eUsedTypeUndefined;
	mFieldDescriptor = NULL;
	mFunctionPointerDescriptor = NULL;
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


bool UsedTypeDescriptor::IsEqual(UsedTypeDescriptor* inOtherUsedType)
{
	if(mUsedType != inOtherUsedType->GetUsedType())
		return false;

	if(eUsedTypeField == mUsedType)
		return mFieldDescriptor->IsEqual(inOtherUsedType->GetFieldDescriptor());
	else
		return mFunctionPointerDescriptor->IsEqual(inOtherUsedType->GetFunctionPointerDescriptor());
}

UsedTypeDescriptor* UsedTypeDescriptor::Clone()
{
	UsedTypeDescriptor* result = new UsedTypeDescriptor();

	if(eUsedTypeField == mUsedType)
	{
		result->mUsedType = eUsedTypeField;
		result->mFieldDescriptor = mFieldDescriptor->Clone();
	}
	else if(eUsedTypeFunctionPointer == mUsedType)
	{
		result->mUsedType = eUsedTypeFunctionPointer;
		result->mFunctionPointerDescriptor = mFunctionPointerDescriptor->Clone();
	}
	return result;
}