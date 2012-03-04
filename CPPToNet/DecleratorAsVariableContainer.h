#pragma once

#include "ICPPDeclarationContainerDriver.h"
#include "ICPPFunctionDefinitionDeclerator.h"
#include "ICPPFieldDeclerator.h"
#include "ICPPFunctionPointerDeclerator.h"
#include "ICPPParametersContainer.h"
#include "TypedParameter.h"

class ICPPVariablesContainerElement;

class DecleratorAsVariableContainer : public ICPPDeclarationContainerDriver, public ICPPFunctionDefinitionDeclerator, public ICPPFieldDeclerator, public ICPPFunctionPointerDeclerator , public ICPPParametersContainer
{
public:
	DecleratorAsVariableContainer(ICPPVariablesContainerElement* inStorage);
	virtual ~DecleratorAsVariableContainer();

	// ICPPDeclarationContainerDriver implementation
	virtual Hummus::EStatusCode SetFlags(CPPElement* inType,bool inIsAuto,bool inIsRegister,bool inIsExtern,bool inIsConst,bool inIsVolatile, bool inIsStatic,bool inIsVirtual);
	virtual ICPPFieldDeclerator* AddFieldDeclerator(const string& inDecleratorName);
	virtual ICPPFunctionPointerDeclerator* AddFunctionPointerDeclarator(const string& inDecleratorName);
	virtual ICPPFunctionDefinitionDeclerator* AddFunctionDefinition(const string& inFunctionName);
	virtual bool VerifyDeclaratorStopper(const string& inTokenToExamine);
	virtual bool ResetVariablesContainer(ICPPVariablesContainerElement* inNewContainer);

	// ICPPFunctionDefinitionDeclerator implementation
	virtual ICPPParametersContainer* GetParametersContainerForFunctionDefinition();
	virtual void AppendModifiersForFunctionDefinitionReturnType(const DeclaratorModifierList& inModifiers);
	virtual void SetFunctionDefinitionHasElipsis();
	virtual void SetPureFunction();
	virtual void SetReturnType(UsedTypeDescriptor* inSetReturnType);
	virtual void SetupFunctionPointerReturnTypeDeclerator(FunctionPointerReturnTypeDeclerator* inReturnTypeDeclerator);
	virtual Hummus::EStatusCode FinalizeFunctionDefinition(const UsedTypeOrExpressionList& inTemplateSpecializationList,bool inIsDefinition); // for function definitions. passing bool to let one know if this is a declaration or definiton
	virtual Hummus::EStatusCode FinalizeFunctionTemplateDefinition(const CPPElementList& inTemplateParameters,const UsedTypeOrExpressionList& inTemplateSpecializationList,bool inIsDefinition); // for function template definitions, passing 

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

	ICPPVariablesContainerElement* mStorage;
	CPPElement* mType;
	bool mIsAuto;
	bool mIsRegister;
	bool mIsExtern;
	bool mIsConst;
	bool mIsVolatile;
	bool mIsStatic;
	bool mIsVirtual;
	bool mIsPure;

	bool mIsFunctionDefinitionParametersImplementation;

	// function decleration functionality
	string mFunctionName;
	UsedTypeDescriptor* mReturnType;
	bool mHasElipsis;
	TypedParameterList mDeclaredParameters;

	// field && function pointer declaration functionality
	string mFieldName;
	UsedTypeDescriptor* mFieldType;

	void CleanupFunction();
};