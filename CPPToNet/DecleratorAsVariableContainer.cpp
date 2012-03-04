#include "DecleratorAsVariableContainer.h"
#include "UsedTypeDescriptor.h"
#include "ICPPVariablesContainerElement.h"
#include "FunctionPointerReturnTypeDeclerator.h"

using namespace Hummus;

DecleratorAsVariableContainer::DecleratorAsVariableContainer(ICPPVariablesContainerElement* inStorage)
{
	mStorage = inStorage;
	mIsAuto = false;
	mIsRegister = false;
	mIsExtern = false;
	mIsConst = false;
	mIsVolatile = false;
	mIsStatic = false;	
	mHasElipsis = false;
	mIsVirtual = false;
	mIsPure = false;
	mType = NULL;
	mReturnType = NULL;
	mFieldType = NULL;
	mIsFunctionDefinitionParametersImplementation = false;
}

DecleratorAsVariableContainer::~DecleratorAsVariableContainer()
{
	delete mReturnType;
	delete mFieldType;

	TypedParameterList::iterator it = mDeclaredParameters.begin();

	for(; it != mDeclaredParameters.end(); ++it)
		delete *it;
	mDeclaredParameters.clear();
}

EStatusCode DecleratorAsVariableContainer::SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic,bool inIsVirtual)
{
	mType = inType;
	mIsAuto = inIsAuto;
	mIsRegister = inIsRegister;
	mIsExtern = inIsExtern;
	mIsConst = inIsConst;
	mIsVolatile = inIsVolatile;
	mIsStatic = inIsStatic;	
	mIsVirtual = inIsVirtual;

	return eSuccess;
}

ICPPFieldDeclerator* DecleratorAsVariableContainer::AddFieldDeclerator(const string& inDecleratorName)
{
	mFieldName = inDecleratorName;
	mFieldType = new UsedTypeDescriptor(mType,mIsAuto,mIsRegister,mIsExtern,mIsConst,mIsVolatile,mIsStatic);
	return this;
}


ICPPFunctionPointerDeclerator* DecleratorAsVariableContainer::AddFunctionPointerDeclarator(const string& inDecleratorName)
{
	UsedTypeDescriptor* returnTypeDescriptor = new UsedTypeDescriptor(mType,mIsAuto,mIsRegister,mIsExtern,mIsConst,mIsVolatile,mIsStatic);
	mFieldType = new UsedTypeDescriptor(returnTypeDescriptor);
	mFieldName = inDecleratorName;
	return this;
}


bool DecleratorAsVariableContainer::VerifyDeclaratorStopper(const string& inTokenToExamine)
{
	return inTokenToExamine == ";";
}

bool DecleratorAsVariableContainer::ResetVariablesContainer(ICPPVariablesContainerElement* inNewContainer)
{
	mStorage = inNewContainer;
	return true;
}


ICPPParametersContainer* DecleratorAsVariableContainer::GetParametersContainerForFunctionDefinition()
{
	mIsFunctionDefinitionParametersImplementation = true;
	return this;
}

ICPPFunctionDefinitionDeclerator* DecleratorAsVariableContainer::AddFunctionDefinition(const string& inFunctionName)
{
	mFunctionName = inFunctionName;
	return this;
}

void DecleratorAsVariableContainer::SetFunctionDefinitionHasElipsis()
{
	mHasElipsis = true;
}

void DecleratorAsVariableContainer::SetPureFunction()
{
	mIsPure = true;
}

void DecleratorAsVariableContainer::SetReturnType(UsedTypeDescriptor* inSetReturnType)
{
	mReturnType = inSetReturnType;
}

void DecleratorAsVariableContainer::SetupFunctionPointerReturnTypeDeclerator(
	FunctionPointerReturnTypeDeclerator* inReturnTypeDeclerator)
{
	// out of the flags, only const and volatile will refer to the function pointer return type
	// so set only them

	inReturnTypeDeclerator->SetFlags(mType,mIsConst,mIsVolatile);
	
	// null flags (just for kicks, they are irrelevant)
	mType = NULL;
	mIsConst = false;
	mIsVolatile = false;
}


EStatusCode DecleratorAsVariableContainer::FinalizeFunctionDefinition(const UsedTypeOrExpressionList& inTemplateSpecializationList,
																	bool inIsDefinition)
{
	if(!mReturnType) // return type may have been set, in the case of function pointer return type
		mReturnType = new UsedTypeDescriptor(mType,mIsAuto,mIsRegister,mIsExtern,mIsConst,mIsVolatile,false);

	CPPFunction* aFunction = mStorage->CreateFunction(mFunctionName,
														mIsVirtual,
														mIsStatic,
														mReturnType,
														mDeclaredParameters,
														mHasElipsis,
														mIsPure,
														inTemplateSpecializationList,
														inIsDefinition);
	
	if(aFunction)
		return eSuccess;

	// if failed - move to cleanup
	CleanupFunction();

	return eFailure;
}

void DecleratorAsVariableContainer::CleanupFunction()
{
	// kill return type
	delete mReturnType;
	mReturnType = NULL;

	// and empty parameters list
	TypedParameterList::iterator it = mDeclaredParameters.begin();

	for(; it != mDeclaredParameters.end(); ++it)
		delete *it;
	mDeclaredParameters.clear();
}

EStatusCode DecleratorAsVariableContainer::FinalizeFunctionTemplateDefinition(const CPPElementList& inTemplateParameters,const UsedTypeOrExpressionList& inTemplateSpecializationList,bool inIsDefinition)
{
	if(!mReturnType) // return type may have been set, in the case of function pointer return type
		mReturnType = new UsedTypeDescriptor(mType,mIsAuto,mIsRegister,mIsExtern,mIsConst,mIsVolatile,false);

	CPPFunction* aFunction = mStorage->CreateFunctionTemplate(mFunctionName,
															 mIsVirtual,
															 mIsStatic,
															 mReturnType,
															 mDeclaredParameters,
															 mHasElipsis,
															 mIsPure,
															 inTemplateParameters,
															 inTemplateSpecializationList,
															 inIsDefinition);
	
	if(aFunction)
		return eSuccess;

	// if failed - move to cleanup
	CleanupFunction();

	return eFailure;
}

void DecleratorAsVariableContainer::AppendModifiers(const DeclaratorModifierList& inModifiers)
{
	mFieldType->GetFieldDescriptor()->AppendModifiers(inModifiers);
}

void DecleratorAsVariableContainer::AddSubscript()
{
	mFieldType->GetFieldDescriptor()->AddSubscript();
}

EStatusCode DecleratorAsVariableContainer::FinalizeFieldDefinition()
{
	CPPVariable* aVariable = mStorage->CreateVariable(mFieldName,mFieldType);

	if(aVariable)
		return eSuccess;

	// if failed - move to cleanup

	// kill return type
	delete mFieldType;
	mFieldType = NULL;

	return eFailure;
}

ICPPParametersContainer* DecleratorAsVariableContainer::GetParametersContainerForFunctionPointer()
{
	mIsFunctionDefinitionParametersImplementation = false;
	return this;	
}

void DecleratorAsVariableContainer::SetFunctionPointerType(EFunctionPointerType inFunctionPointerType)
{
	mFieldType->GetFunctionPointerDescriptor()->SetFunctionPointerType(inFunctionPointerType);
}

void DecleratorAsVariableContainer::AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers)
{
	mFieldType->GetFunctionPointerDescriptor()->AppendModifiersForFunctionPointerReturnType(inModifiers);
}

void DecleratorAsVariableContainer::SetFunctionPointerHasElipsis()
{
	mFieldType->GetFunctionPointerDescriptor()->SetFunctionPointerHasElipsis();
}

EStatusCode DecleratorAsVariableContainer::FinalizeFunctionPointerDefinition()
{
	CPPVariable* aFunctionPointer = mStorage->CreateVariable(mFieldName,mFieldType);
	if(aFunctionPointer)
		return eSuccess;

	// if failed - move to cleanup

	// kill return type
	delete mFieldType;
	mFieldType = NULL;

	return eFailure;
}

EStatusCode DecleratorAsVariableContainer::CreateParameter(const string& inParameterName,
															     UsedTypeDescriptor* inParameterType)
{
	if(mIsFunctionDefinitionParametersImplementation)
	{
		TypedParameter* newParameter = new TypedParameter();
		newParameter->Type = inParameterType;
		newParameter->Name = inParameterName;

		mDeclaredParameters.push_back(newParameter);
		return eSuccess;
	}
	else
	{
		// function pointer case, use the field type to define the parameter
		return mFieldType->GetFunctionPointerDescriptor()->CreateParameter(inParameterName,inParameterType);
	}
}