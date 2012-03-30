#include "TypedParameter.h"
#include "UsedTypeDescriptor.h"

TypedParameter::TypedParameter()
{
	Type = NULL;
}

TypedParameter::TypedParameter(UsedTypeDescriptor* inType)
{
	Type = inType;
}


TypedParameter::~TypedParameter()
{
	delete Type;
}