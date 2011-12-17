#pragma once
#include <string>

struct FunctionParameter;
class UsedTypeDescriptor;

using namespace std;

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

	virtual void SetFunctionPointerType(EFunctionPointerType inFunctionPointerType) = 0;
	virtual FunctionParameter* CreateParameter(const string& inParameterName,
										 UsedTypeDescriptor* inParameterType) = 0;

};