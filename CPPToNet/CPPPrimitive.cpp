#include "CPPPrimitive.h"

CPPPrimitive::CPPPrimitive(const string& inName,ECPPPrimitiveType inPrimitiveType):CPPElement(inName,eCPPElementPrimitive)
{
	PrimitiveType = inPrimitiveType;
}

CPPPrimitive::~CPPPrimitive(void)
{
}

bool CPPPrimitive::IsEqual(CPPPrimitive* inOther)
{
	return PrimitiveType == inOther->PrimitiveType;
}
