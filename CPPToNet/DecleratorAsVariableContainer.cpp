#include "DecleratorAsVariableContainer.h"
#include "UsedTypeDescriptor.h"
#include "ICPPVariablesContainerElement.h"

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
	mReturnType = NULL;
	mFieldType = NULL;
	mIsFunctionDefinitionParametersImplementation = false;
}

DecleratorAsVariableContainer::~DecleratorAsVariableContainer()
{
	delete mReturnType;
	delete mFieldType;

	FunctionParameterList::iterator it = mDeclaredParameters.begin();

	for(; it != mDeclaredParameters.end(); ++it)
		delete *it;
	mDeclaredParameters.clear();
}

EStatusCode DecleratorAsVariableContainer::SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic)
{
	mType = inType;
	mIsAuto = inIsAuto;
	mIsRegister = inIsRegister;
	mIsExtern = inIsExtern;
	mIsConst = inIsConst;
	mIsVolatile = inIsVolatile;
	mIsStatic = inIsStatic;	

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

ICPPParametersContainer* DecleratorAsVariableContainer::GetParametersContainerForFunctionDefinition()
{
	mIsFunctionDefinitionParametersImplementation = true;
	return this;
}

ICPPFunctionDefinitionDeclerator* DecleratorAsVariableContainer::AddFunctionDefinition(const string& inFunctionName)
{
	mFunctionName = inFunctionName;
	mReturnType = new UsedTypeDescriptor(mType,mIsAuto,mIsRegister,mIsExtern,mIsConst,mIsVolatile,mIsStatic);
	return this;
}

void DecleratorAsVariableContainer::SetFunctionDefinitionHasElipsis()
{
	mHasElipsis = true;
}

EStatusCode DecleratorAsVariableContainer::FinalizeFunctionDefinition(bool inIsDefinition)
{
	CPPFunction* aFunction = mStorage->CreateFunction(mFunctionName,mReturnType,mDeclaredParameters,mHasElipsis,inIsDefinition);
	
	if(aFunction)
		return eSuccess;

	// if failed - move to cleanup

	// kill return type
	delete mReturnType;
	mReturnType = NULL;

	// and empty parameters list
	FunctionParameterList::iterator it = mDeclaredParameters.begin();

	for(; it != mDeclaredParameters.end(); ++it)
		delete *it;
	mDeclaredParameters.clear();

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

FunctionParameter* DecleratorAsVariableContainer::CreateParameter(const string& inParameterName,
															     UsedTypeDescriptor* inParameterType)
{
	if(mIsFunctionDefinitionParametersImplementation)
	{
		FunctionParameter* newParameter = new FunctionParameter();
		newParameter->Type = inParameterType;
		newParameter->Name = inParameterName;

		mDeclaredParameters.push_back(newParameter);
		return newParameter;
	}
	else
	{
		// function pointer case, use the field type to define the parameter
		return mFieldType->GetFunctionPointerDescriptor()->CreateParameter(inParameterName,inParameterType);
	}
}