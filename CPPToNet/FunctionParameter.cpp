#include "FunctionParameter.h"
#include "UsedTypeDescriptor.h"

FunctionParameter::FunctionParameter()
{
	Type = NULL;
}

FunctionParameter::~FunctionParameter()
{
	delete Type;
}