#pragma once

#include "CPPStandardIntegerType.h"

class CPPUnsignedIntType : public CPPStandardIntegerType<unsigned,int>
{
protected:
	virtual CPPPrimitiveValue TypeValue(unsigned inValue);
	virtual unsigned ValueType(const CPPPrimitiveValue& inValue);
};
