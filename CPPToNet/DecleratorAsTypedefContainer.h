#pragma once

#include "ICPPDeclarationContainerDriver.h"
#include "ICPPFieldDeclerator.h"
#include "ICPPFunctionPointerDeclerator.h"
#include "ICPPParametersContainer.h"

class ICPPDefinitionsContainerElement;

class DecleratorAsTypedefContainer : public ICPPDeclarationContainerDriver, public ICPPFieldDeclerator, public ICPPFunctionPointerDeclerator, public ICPPParametersContainer
{

public:
	DecleratorAsTypedefContainer(ICPPDefinitionsContainerElement* inStorage);
	virtual ~DecleratorAsTypedefContainer();

	// ICPPDeclarationContainerDriver implementation
	virtual Hummus::EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic,bool inIsVirtual);
	virtual ICPPFieldDeclerator* AddFieldDeclerator(const string& inDeclaratorName);
	virtual ICPPFunctionDefinitionDeclerator* AddFunctionDefinition(const string& inFunctionName);
	virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName);
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
	virtual Hummus::EStatusCode CreateParameter(const string& inParameterName,  UsedTypeDescriptor* inParameterType);

private:
	ICPPDefinitionsContainerElement* mStorage;
	CPPElement* mType;
	bool mIsConst;
	bool mIsVolatile;
	bool mAlreadyDefinedOne;

	// field && function pointer declaration functionality
	string mFieldName;
	UsedTypeDescriptor* mFieldType;
};