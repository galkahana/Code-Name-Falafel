#pragma once

#include "CPPStandardIntegerType.h"

class CPPUnsignedLongType : public CPPStandardIntegerType<unsigned long, long>
{
protected:

	virtual CPPPrimitiveValue TypeValue(unsigned long inValue);
	virtual unsigned long ValueType(const CPPPrimitiveValue& inValue);
};
