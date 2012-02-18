#include "DecleratorAsTypedefContainer.h"
#include "Trace.h"
#include "UsedTypeDescriptor.h"
#include "ICPPDefinitionsContainerElement.h"

using namespace Hummus;

DecleratorAsTypedefContainer::DecleratorAsTypedefContainer(ICPPDefinitionsContainerElement* inStorage)
{
	mStorage = inStorage;
	mAlreadyDefinedOne = false;
	mFieldType = NULL;
}

DecleratorAsTypedefContainer::~DecleratorAsTypedefContainer()
{
	delete mFieldType;
}

EStatusCode DecleratorAsTypedefContainer::SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic,bool inIsVirtual)
{
	mIsConst = inIsConst;
	mIsVolatile = inIsVolatile;
	mType = inType;

	if(inIsAuto || inIsRegister || inIsExtern || inIsStatic || inIsVirtual)
	{
		TRACE_LOG("DecleratorAsTypedefContainer::SetFlags, irelevant flags set for typedef. Should not set auto, extern, register, static or virtual. only const and volatile are allowed.");
		return eFailure;
	}
	else
		return eSuccess;
}

ICPPFieldDeclerator* DecleratorAsTypedefContainer::AddFieldDeclerator(const string& inDeclaratorName)
{
	if(mAlreadyDefinedOne)
	{
		TRACE_LOG("DecleratorAsTypedefContainer::AddDeclarator, typedef statement contains multiple declarator, this is a syntax error");
		return NULL;
	}

	mAlreadyDefinedOne = true;

	mFieldName = inDeclaratorName;
	mFieldType = new UsedTypeDescriptor(mType,false,false,false,mIsConst,mIsVolatile,false);
	return this;
}

ICPPFunctionPointerDeclerator* DecleratorAsTypedefContainer::AddFunctionPointerDeclarator(const string& inDecleratorName)
{
	if(mAlreadyDefinedOne)
	{
		TRACE_LOG("DecleratorAsTypedefContainer::AddDeclarator, typedef statement contains multiple declarator, this is a syntax error");
		return NULL;
	}

	mAlreadyDefinedOne = true;

	UsedTypeDescriptor* returnTypeDescriptor = new UsedTypeDescriptor(mType,false,false,false,mIsConst,mIsVolatile,false);
	mFieldType = new UsedTypeDescriptor(returnTypeDescriptor);
	mFieldName = inDecleratorName;
	return this;
}

ICPPFunctionDefinitionDeclerator* DecleratorAsTypedefContainer::AddFunctionDefinition(const string& inFunctionName)
{
	// function definition is not appropriate for typedef, so avoid
	return NULL;	
}

bool DecleratorAsTypedefContainer::VerifyDeclaratorStopper(const string& inTokenToExamine)
{
	return inTokenToExamine== ";";
}

bool DecleratorAsTypedefContainer::ResetVariablesContainer(ICPPVariablesContainerElement* inNewContainer)
{
	// variables container is irrelevant for typedef, hence refuse to reset with this new container
	return false;
}

ICPPParametersContainer* DecleratorAsTypedefContainer::GetParametersContainerForFunctionPointer()
{
	return this;
}

FunctionParameter* DecleratorAsTypedefContainer::CreateParameter(const string& inParameterName,  UsedTypeDescriptor* inParameterType)
{
	return mFieldType->GetFunctionPointerDescriptor()->CreateParameter(inParameterName,inParameterType);
}

void DecleratorAsTypedefContainer::AppendModifiers(const DeclaratorModifierList& inModifiers)
{
	mFieldType->GetFieldDescriptor()->AppendModifiers(inModifiers);
}

void DecleratorAsTypedefContainer::AddSubscript()
{
	mFieldType->GetFieldDescriptor()->AddSubscript();
}

EStatusCode DecleratorAsTypedefContainer::FinalizeFieldDefinition()
{
	CPPTypedef* aTypedef = mStorage->CreateTypedef(mFieldName,mFieldType);

	if(aTypedef)
		return eSuccess;

	// if failed - move to cleanup

	// kill return type
	delete mFieldType;
	mFieldType = NULL;

	return eFailure;
}

void DecleratorAsTypedefContainer::SetFunctionPointerType(EFunctionPointerType inFunctionPointerType)
{
	mFieldType->GetFunctionPointerDescriptor()->SetFunctionPointerType(inFunctionPointerType);
}

void DecleratorAsTypedefContainer::AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers)
{
	mFieldType->GetFunctionPointerDescriptor()->AppendModifiersForFunctionPointerReturnType(inModifiers);
}

void DecleratorAsTypedefContainer::SetFunctionPointerHasElipsis()
{
	mFieldType->GetFunctionPointerDescriptor()->SetFunctionPointerHasElipsis();
}

EStatusCode DecleratorAsTypedefContainer::FinalizeFunctionPointerDefinition()
{
	CPPTypedef* aTypedef = mStorage->CreateTypedef(mFieldName,mFieldType);
	if(aTypedef)
		return eSuccess;

	// if failed - move to cleanup

	// kill return type
	delete mFieldType;
	mFieldType = NULL;

	return eFailure;
}