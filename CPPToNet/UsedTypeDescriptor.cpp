#include "UsedTypeDescriptor.h"
#include "CPPElement.h"
#include "Trace.h"
#include "CPPTypedef.h"
#include "CPPTemplateTemplateParameter.h"
#include "CPPTemplateTypename.h"
#include "CPPTemplateValue.h"

using namespace Hummus;

FieldTypeDescriptor::FieldTypeDescriptor(CPPElement* inType, 
										bool inIsAuto,
										bool inIsRegister,
										bool inIsExtern,
										bool inIsConst,
										bool inIsVolatile,
										bool inIsStatic):mModifiers()
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
	if(inModifiers.size() > 0)
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

bool FieldTypeDescriptor::IsEqual(FieldTypeDescriptor* inOtherDescriptor) const
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

	DeclaratorModifierList::const_iterator itThis = mModifiers.begin();
	DeclaratorModifierList::const_iterator itOther = inOtherDescriptor->mModifiers.begin();

	bool isEqual = true;
	for(; itThis != mModifiers.end() && isEqual; ++itThis,++itOther)
		isEqual = itThis->IsEqual(*itOther);
	if(!isEqual)
		return false;

	if(mSubscriptCount != inOtherDescriptor->mSubscriptCount)
		return false;

	// k. now let's compare the type. to save me some work, i'll make sure to do equality only for what is actually types
	// at this point we have : 	eCPPElementPrimitive, eCPPElementEnumerator, eCPPElementUnion, eCPPElementTypedef
	// that can serve as types
	if(mType->Type != inOtherDescriptor->mType->Type)
		return false;

	// typedefs should be internally compared, being only aliases. anything else, should just have pointer comparison
	// templates have their own equivalence rules
	switch(mType->Type)
	{
		case CPPElement::eCPPElementTypedef:
			isEqual =  ((CPPTypedef*)mType)->IsEqual((CPPTypedef*)inOtherDescriptor->mType);
			break;
		case CPPElement::eCPPElemeentTemplateTemplateParameter:
			isEqual	= ((CPPTemplateTemplateParameter*)mType)->IsEqual((CPPTemplateTemplateParameter*)inOtherDescriptor->mType);
			break;
		case CPPElement::eCPPElemeentTemplateTypename:
			isEqual	= ((CPPTemplateTypename*)mType)->IsEqual((CPPTemplateTypename*)inOtherDescriptor->mType);
			break;
		case CPPElement::eCPPElementTemplateValue:
			isEqual	= ((CPPTemplateValue*)mType)->IsEqual((CPPTemplateValue*)inOtherDescriptor->mType);
			break;
		default:
			isEqual = (mType == inOtherDescriptor->mType);

	}
	return isEqual;
}

bool FieldTypeDescriptor::IsLess(FieldTypeDescriptor* inOtherDescriptor)
{
	if(IsAuto != inOtherDescriptor->IsAuto)
		return !IsAuto;

	if(IsRegister != inOtherDescriptor->IsRegister)
		return !IsRegister;

	if(IsExtern != inOtherDescriptor->IsExtern)
		return !IsExtern;

	if(IsConst != inOtherDescriptor->IsConst)
		return !IsConst;

	if(IsVolatile != inOtherDescriptor->IsVolatile)
		return !IsVolatile;

	if(IsStatic != inOtherDescriptor->IsStatic)
		return !IsStatic;

	if(mModifiers.size() != inOtherDescriptor->mModifiers.size())
		return mModifiers.size() < inOtherDescriptor->mModifiers.size();

	DeclaratorModifierList::iterator itThis = mModifiers.begin();
	DeclaratorModifierList::iterator itOther = inOtherDescriptor->mModifiers.begin();

	bool isEqual = true;
	bool isLess = false;
	for(; itThis != mModifiers.end() && isEqual; ++itThis,++itOther)
	{
		isEqual = itThis->IsEqual(*itOther);
		if(!isEqual)
			isLess = itThis->IsLess(*itOther);
	}
	if(!isEqual)
		return isLess;

	if(mSubscriptCount != inOtherDescriptor->mSubscriptCount)
		return mSubscriptCount < inOtherDescriptor->mSubscriptCount;

	// k. now let's compare the type. to save me some work, i'll make sure to do equality only for what is actually types
	// at this point we have : 	eCPPElementPrimitive, eCPPElementEnumerator, eCPPElementUnion, eCPPElementTypedef
	// that can serve as types
	if(mType->Type != inOtherDescriptor->mType->Type)
		return mType->Type < inOtherDescriptor->mType->Type;

	// typedefs should be internally compared, being only aliases. anything else, should just have identity comparison
	// templates have their own equivalence rules
	switch(mType->Type)
	{
		case CPPElement::eCPPElementTypedef:
			if(!((CPPTypedef*)mType)->IsEqual((CPPTypedef*)inOtherDescriptor->mType))
				isLess = ((CPPTypedef*)mType)->IsLess((CPPTypedef*)inOtherDescriptor->mType);
			break;
		case CPPElement::eCPPElemeentTemplateTemplateParameter:
			if(!((CPPTemplateTemplateParameter*)mType)->IsEqual((CPPTemplateTemplateParameter*)inOtherDescriptor->mType))
				isLess = ((CPPTemplateTemplateParameter*)mType)->IsLess((CPPTemplateTemplateParameter*)inOtherDescriptor->mType);
			break;
		case CPPElement::eCPPElemeentTemplateTypename:
			if(!((CPPTemplateTypename*)mType)->IsEqual((CPPTemplateTypename*)inOtherDescriptor->mType))
				isLess = ((CPPTemplateTypename*)mType)->IsLess((CPPTemplateTypename*)inOtherDescriptor->mType);
			break;
		case CPPElement::eCPPElementTemplateValue:
			if(!((CPPTemplateValue*)mType)->IsEqual((CPPTemplateValue*)inOtherDescriptor->mType))
				isLess = ((CPPTemplateTypename*)mType)->IsLess((CPPTemplateTypename*)inOtherDescriptor->mType);
			break;
		default:
			isLess = (mType < inOtherDescriptor->mType);
	}
	return isLess;
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
	mSubscriptCount = 0;
}

FunctionPointerTypeDescriptor::~FunctionPointerTypeDescriptor()
{
	delete mReturnType;
	TypedParameterList::iterator it = mDeclaredParameters.begin();	

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


Hummus::EStatusCode FunctionPointerTypeDescriptor::CreateParameter(const string& inParameterName,UsedTypeDescriptor* inParameterType)
{
	TypedParameter* newParameter = new TypedParameter();
	newParameter->Type = inParameterType;
	newParameter->Name = inParameterName;

	mDeclaredParameters.push_back(newParameter);
	return eSuccess;
}

void FunctionPointerTypeDescriptor::AppendParameter(TypedParameter* inParameter)
{
	mDeclaredParameters.push_back(inParameter);
}


ICPPFunctionPointerDeclerator::EFunctionPointerType FunctionPointerTypeDescriptor::GetPointerType()
{
	return mPointerType;
}

UsedTypeDescriptor* FunctionPointerTypeDescriptor::GetReturnType()
{
	return mReturnType;
}

Hummus::SingleValueContainerIterator<TypedParameterList> FunctionPointerTypeDescriptor::GetParametersListIterator()
{
	return Hummus::SingleValueContainerIterator<TypedParameterList>(mDeclaredParameters);
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


	if(mSubscriptCount != inOther->GetSubscriptCount())
		return false;

	if(!mReturnType->IsEqual(inOther->GetReturnType()))
		return false;

	if(mDeclaredParameters.size() != mDeclaredParameters.size())
		return false;

	TypedParameterList::iterator itThis,itOther;
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

bool FunctionPointerTypeDescriptor::IsLess(FunctionPointerTypeDescriptor* inOther)
{
	if(mPointerType != inOther->GetPointerType())
		return mPointerType < inOther->GetPointerType();

	if(mSubscriptCount != inOther->GetSubscriptCount())
		return mSubscriptCount < inOther->GetSubscriptCount();

	if(!mReturnType->IsEqual(inOther->GetReturnType()))
		return mReturnType->IsLess(inOther->GetReturnType());

	if(mDeclaredParameters.size() != inOther->mDeclaredParameters.size())
		return mDeclaredParameters.size() < inOther->mDeclaredParameters.size();

	TypedParameterList::iterator itThis,itOther;
	itThis = mDeclaredParameters.begin();
	itOther = inOther->mDeclaredParameters.begin();
	bool isEqual = true;
	bool isLess = false;

	// with parameters comparison, only compare the parameter types, because the names
	// don't matter
	for(; itThis != mDeclaredParameters.end() && isEqual; ++itThis,++itOther)
	{
		isEqual = (*itThis)->Type->IsEqual((*itOther)->Type);
		if(!isEqual)
			isLess = (*itThis)->Type->IsLess((*itOther)->Type);
	}

	if(!isEqual)
		return isLess;

	return (mHasElipsis  == inOther->HasElipsis()) ? false : !mHasElipsis;
}


FunctionPointerTypeDescriptor* FunctionPointerTypeDescriptor::Clone()
{
	// declared parameters and return type should be cloned
	FunctionPointerTypeDescriptor* result = new FunctionPointerTypeDescriptor(mReturnType->Clone());
	result->mPointerType = mPointerType;
	TypedParameterList::iterator itParameters = mDeclaredParameters.begin();
	for(; itParameters != mDeclaredParameters.end(); ++itParameters)
		result->CreateParameter((*itParameters)->Name,(*itParameters)->Type->Clone());
	result->mHasElipsis = mHasElipsis;
	result->mSubscriptCount = mSubscriptCount;
	return result;
}

void FunctionPointerTypeDescriptor::AddSubscript()
{
	++mSubscriptCount;
}

void FunctionPointerTypeDescriptor::RemoveSubscript()
{
	--mSubscriptCount;
}

unsigned long FunctionPointerTypeDescriptor::GetSubscriptCount()
{
	return mSubscriptCount;
}


void FunctionPointerTypeDescriptor::DetachReturnType()
{
	mReturnType = false;
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

bool UsedTypeDescriptor::IsLess(UsedTypeDescriptor* inOtherUsedType)
{
	if(mUsedType != inOtherUsedType->GetUsedType())
		return mUsedType < inOtherUsedType->GetUsedType();

	if(eUsedTypeField == mUsedType)
		return mFieldDescriptor->IsLess(inOtherUsedType->GetFieldDescriptor());
	else
		return mFunctionPointerDescriptor->IsLess(inOtherUsedType->GetFunctionPointerDescriptor());
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