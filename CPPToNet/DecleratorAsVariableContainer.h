#pragma once

#include "ICPPDeclarationContainerDriver.h"
#include "ICPPFunctionDefinitionDeclerator.h"
#include "ICPPFieldDeclerator.h"
#include "ICPPFunctionPointerDeclerator.h"
#include "ICPPParametersContainer.h"
#include "FunctionParameter.h"

class ICPPVariablesContainerElement;

class DecleratorAsVariableContainer : public ICPPDeclarationContainerDriver, public ICPPFunctionDefinitionDeclerator, public ICPPFieldDeclerator, public ICPPFunctionPointerDeclerator , public ICPPParametersContainer
{
public:
	DecleratorAsVariableContainer(ICPPVariablesContainerElement* inStorage);
	virtual ~DecleratorAsVariableContainer();

	// ICPPDeclarationContainerDriver implementation
	virtual Hummus::EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic);
	virtual ICPPFieldDeclerator* AddFieldDeclerator(const string& inDecleratorName);
	virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName);
	virtual ICPPFunctionDefinitionDeclerator* AddFunctionDefinition(const string& inFunctionName);
	virtual bool VerifyDeclaratorStopper(const string& inTokenToExamine);

	// ICPPFunctionDefinitionDeclerator implementation
	virtual ICPPParametersContainer* GetParametersContainerForFunctionDefinition();
	virtual void AppendModifiersForFunctionDefinitionReturnType(const DeclaratorModifierList& inModifiers);
	virtual void SetFunctionDefinitionHasElipsis();
	virtual Hummus::EStatusCode FinalizeFunctionDefinition(bool inIsDefinition); // passing bool to let one know if this is a declaration or definiton

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
	virtual FunctionParameter* CreateParameter(const string& inParameterName,  UsedTypeDescriptor* inParameterType);

private:

	ICPPVariablesContainerElement* mStorage;
	CPPElement* mType;
	bool mIsAuto;
	bool mIsRegister;
	bool mIsExtern;
	bool mIsConst;
	bool mIsVolatile;
	bool mIsStatic;

	bool mIsFunctionDefinitionParametersImplementation;

	// function decleration functionality
	string mFunctionName;
	UsedTypeDescriptor* mReturnType;
	bool mHasElipsis;
	FunctionParameterList mDeclaredParameters;

	// field && function pointer declaration functionality
	string mFieldName;
	UsedTypeDescriptor* mFieldType;
};