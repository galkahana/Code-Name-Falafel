#include "DecleratorAsParametersContainer.h"
#include "Trace.h"
#include "UsedTypeDescriptor.h"

using namespace Hummus;

DecleratorAsParametersContainer::DecleratorAsParametersContainer()
{
	mFoundStop = false;
	mAlreadyDefinedOne = true;
	mFieldType = NULL;
}

DecleratorAsParametersContainer::DecleratorAsParametersContainer(ICPPParametersContainer* inParametersContainer,string inStopperToken)
{
	mFoundStop = false;
	mParametersContainer = inParametersContainer;
	mAlreadyDefinedOne = true;
	mFieldType = NULL;
	mStopperToken = inStopperToken;
}

DecleratorAsParametersContainer::~DecleratorAsParametersContainer()
{
	delete mFieldType;
}

void DecleratorAsParametersContainer::SetCreator(ICPPParametersContainer* inParametersContainer,string inStopperToken)
{
	mParametersContainer = inParametersContainer;
	mStopperToken = inStopperToken;
}

EStatusCode DecleratorAsParametersContainer::SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic,bool inIsVirtual)
{
	mIsConst = inIsConst;
	mIsVolatile = inIsVolatile;
	mType = inType;

	if(inIsAuto || inIsRegister || inIsExtern || inIsStatic || inIsVirtual)
	{
		TRACE_LOG("DecleratorAsParametersContainer::SetFlags, irelevant flags set for typedef. Should not set auto, extern, register, static or virtual. only const and volatile are allowed.");
		return eFailure;
	}
	else
		return eSuccess;
}

ICPPFieldDeclerator* DecleratorAsParametersContainer::AddFieldDeclerator(const string& inDecleratorName)
{	
	if(mAlreadyDefinedOne)
	{
		TRACE_LOG("DecleratorAsParametersContainer::AddDeclarator, parameter statement contains multiple declarator, this is a syntax error");
		return NULL;
	}

	mAlreadyDefinedOne = true;

	mFieldName = inDecleratorName;
	mFieldType = new UsedTypeDescriptor(mType,false,false,false,mIsConst,mIsVolatile,false);
	return this;
}

ICPPFunctionPointerDeclerator* DecleratorAsParametersContainer::AddFunctionPointerDeclarator(const string& inDecleratorName)
{
	if(mAlreadyDefinedOne)
	{
		TRACE_LOG("DecleratorAsParametersContainer::AddFunctionPointerDeclarator, parameter statement contains multiple declarator, this is a syntax error");
		return NULL;
	}

	mAlreadyDefinedOne = true;

	UsedTypeDescriptor* returnTypeDescriptor = new UsedTypeDescriptor(mType,false,false,false,mIsConst,mIsVolatile,false);
	mFieldType = new UsedTypeDescriptor(returnTypeDescriptor);
	mFieldName = inDecleratorName;
	return this;
}

ICPPFunctionDefinitionDeclerator* DecleratorAsParametersContainer::AddFunctionDefinition(const string& inFunctionName)
{
	// function definition is irrelevant for parameters containers, so avoid
	return NULL;
}

void DecleratorAsParametersContainer::Reset()
{
	mAlreadyDefinedOne = false;
	mIsConst = false;
	mIsVolatile = false;
	delete mFieldType;
	mFieldType = NULL;
	mStopparCarry.clear();
}

bool DecleratorAsParametersContainer::VerifyDeclaratorStopper(const string& inTokenToExamine)
{
	if(inTokenToExamine == ",")
		return true;
	else if(inTokenToExamine == mStopperToken)
	{
		mFoundStop = true;
		return true;
	}
	else if(mStopperToken == ">" && inTokenToExamine.at(0) == '>') // special case to avoid the template problem with >>
	{
		if(inTokenToExamine.size() > 1)
			mStopparCarry = inTokenToExamine.substr(1);
		mFoundStop = true;
		return true;
	}
	else 
		return false;
}

bool DecleratorAsParametersContainer::ResetVariablesContainer(ICPPVariablesContainerElement* inNewContainer)
{
	// variables container is irrelevant for parameters collecting scope, hence refuse.
	return false;
}

bool DecleratorAsParametersContainer::FoundStop()
{
	return mFoundStop;
}

void DecleratorAsParametersContainer::AppendModifiers(const DeclaratorModifierList& inModifiers)
{
	mFieldType->GetFieldDescriptor()->AppendModifiers(inModifiers);
}

void DecleratorAsParametersContainer::AddSubscript()
{
	mFieldType->GetFieldDescriptor()->AddSubscript();
}

EStatusCode DecleratorAsParametersContainer::FinalizeFieldDefinition()
{
	EStatusCode status  = mParametersContainer->CreateParameter(mFieldName,mFieldType);

	if(eSuccess == status)
		return eSuccess;

	// if failed - move to cleanup

	// kill return type
	delete mFieldType;
	mFieldType = NULL;

	return eFailure;
}

EStatusCode DecleratorAsParametersContainer::CreateParameter(const string& inParameterName,  UsedTypeDescriptor* inParameterType)
{
	return mFieldType->GetFunctionPointerDescriptor()->CreateParameter(inParameterName,inParameterType);
}

ICPPParametersContainer* DecleratorAsParametersContainer::GetParametersContainerForFunctionPointer()
{
	return this;
}

void DecleratorAsParametersContainer::SetFunctionPointerType(EFunctionPointerType inFunctionPointerType)
{
	mFieldType->GetFunctionPointerDescriptor()->SetFunctionPointerType(inFunctionPointerType);
}

void DecleratorAsParametersContainer::AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers)
{
	mFieldType->GetFunctionPointerDescriptor()->AppendModifiersForFunctionPointerReturnType(inModifiers);
}

void DecleratorAsParametersContainer::SetFunctionPointerHasElipsis()
{
	mFieldType->GetFunctionPointerDescriptor()->SetFunctionPointerHasElipsis();
}

EStatusCode DecleratorAsParametersContainer::FinalizeFunctionPointerDefinition()
{
	EStatusCode status = mParametersContainer->CreateParameter(mFieldName,mFieldType);

	if(eSuccess == status)
		return eSuccess;

	// if failed - move to cleanup

	// kill return type
	delete mFieldType;
	mFieldType = NULL;

	return eFailure;
}

const string& DecleratorAsParametersContainer::GetStopperCarry()
{
	return mStopparCarry;
}
