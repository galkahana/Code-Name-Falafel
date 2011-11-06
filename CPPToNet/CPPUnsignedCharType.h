#pragma once

#include "CPPStandardIntegerType.h"

class CPPUnsignedCharType : public CPPStandardIntegerType<unsigned char, char>
{

protected:
	virtual CPPPrimitiveValue TypeValue(unsigned char inValue);
	virtual unsigned char ValueType(const CPPPrimitiveValue& inValue);
};
