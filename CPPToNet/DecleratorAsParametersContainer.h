#pragma once
#include "ICPPDeclarationContainerDriver.h"
#include "ICPPFieldDeclerator.h"
#include "ICPPFunctionPointerDeclerator.h"
#include "ICPPParametersContainer.h"
#include "TypedParameter.h"


class DecleratorAsParametersContainer : public ICPPDeclarationContainerDriver, public ICPPFieldDeclerator, public ICPPFunctionPointerDeclerator, public ICPPParametersContainer
{
public:

	DecleratorAsParametersContainer();
	DecleratorAsParametersContainer(ICPPParametersContainer* inParametersContainer,string inStopperToken);
	virtual ~DecleratorAsParametersContainer();
	
	// must be called if empty constructor was called...before any usage
	void SetCreator(ICPPParametersContainer* inParametersContainer,string inStopperToken);

	// ICPPDeclarationContainerDriver implementation
	virtual Hummus::EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic, bool inIsVirtual);
	virtual ICPPFieldDeclerator* AddFieldDeclerator(const string& inDecleratorName);
	virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName);
	virtual ICPPFunctionDefinitionDeclerator* AddFunctionDefinition(const string& inFunctionName);
	virtual bool VerifyDeclaratorStopper(const string& inTokenToExamine);
	virtual bool ResetVariablesContainer(ICPPVariablesContainerElement* inNewContainer);


	// ICPPFieldDeclerator implementation
	virtual void AppendModifiers(const DeclaratorModifierList& inModifiers);
	virtual void AddSubscript();
	virtual Hummus::EStatusCode FinalizeFieldDefinition();

	// ICPPFunctionPointerDeclerator implementation
	virtual ICPPParametersContainer* GetParametersContainerForFunctionPointer();
	virtual void SetFunctionPointerType(EFunctionPointerType inFunctionPointerType);
	virtual void AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers);
	virtual void SetFunctionPointerHasElipsis();	
	virtual Hummus::EStatusCode FinalizeFunctionPointerDefinition();

	// ICPPParametersContainer implementation
	virtual TypedParameter* CreateParameter(const string& inParameterName,  UsedTypeDescriptor* inParameterType);

	bool FoundStop();
	void Reset();

private:
	ICPPParametersContainer* mParametersContainer;

	bool mIsConst;
	bool mIsVolatile;	
	CPPElement* mType;
	bool mFoundStop;
	bool mAlreadyDefinedOne;
	string mStopperToken;

	TypedParameterList mDeclaredParameters;

	// field && function pointer declaration functionality
	string mFieldName;
	UsedTypeDescriptor* mFieldType;

};