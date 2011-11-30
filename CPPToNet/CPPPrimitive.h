#pragma once

#include "CPPPrimitiveTypes.h"
#include "CPPElement.h"

class CPPPrimitive : public CPPElement
{
public:
	CPPPrimitive(const string& inName,ECPPPrimitiveType inPrimitiveType);
	~CPPPrimitive(void);

	ECPPPrimitiveType PrimitiveType;
};
