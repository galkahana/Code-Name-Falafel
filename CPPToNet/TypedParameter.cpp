#include "TypedParameter.h"
#include "UsedTypeDescriptor.h"

TypedParameter::TypedParameter()
{
	Type = NULL;
}

TypedParameter::~TypedParameter()
{
	delete Type;
}