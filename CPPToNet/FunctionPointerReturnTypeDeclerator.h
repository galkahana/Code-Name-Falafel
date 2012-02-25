#pragma once
#include "ICPPFunctionPointerDeclerator.h"
#include "ICPPParametersContainer.h"

class UsedTypeDescriptor;
class CPPElement;

class FunctionPointerReturnTypeDeclerator : public ICPPFunctionPointerDeclerator, public ICPPParametersContainer
{
public:
	FunctionPointerReturnTypeDeclerator(void);
	~FunctionPointerReturnTypeDeclerator(void);


	// ICPPFunctionPointerDeclerator implementation
	virtual ICPPParametersContainer* GetParametersContainerForFunctionPointer();
	virtual void SetFunctionPointerType(EFunctionPointerType inFunctionPointerType);
	virtual void AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers);
	virtual void SetFunctionPointerHasElipsis();
	virtual Hummus::EStatusCode FinalizeFunctionPointerDefinition();

	// ICPPParametersContainer implementation
	virtual TypedParameter* CreateParameter(const string& inParameterName,
										 UsedTypeDescriptor* inParameterType);

	void SetFlags(CPPElement* inType,bool inIsConst, bool inIsVolatile);
	UsedTypeDescriptor* DetachUsedTypeDescriptor();

private:

	UsedTypeDescriptor* mUsedTypeDescriptor;



};

