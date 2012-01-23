#pragma once
#include "DeclaratorModifier.h"
#include "EStatusCode.h"


class ICPPParametersContainer;

class ICPPFunctionPointerDeclerator 
{
public:
	// Indicates whether this is a function pointer or function reference
	enum EFunctionPointerType
	{
		eFunctionPointerTypeNone,
		eFunctionPointerTypeReference,
		eFunctionPointerTypePointer
	};

	virtual ICPPParametersContainer* GetParametersContainerForFunctionPointer() = 0;
	virtual void SetFunctionPointerType(EFunctionPointerType inFunctionPointerType) = 0;
	virtual void AppendModifiersForFunctionPointerReturnType(const DeclaratorModifierList& inModifiers) = 0;
	virtual void SetFunctionPointerHasElipsis() = 0;
	virtual Hummus::EStatusCode FinalizeFunctionPointerDefinition() = 0;
};