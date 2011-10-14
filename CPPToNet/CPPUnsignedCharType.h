#pragma once

#include "CPPStandardIntegerType.h"

class CPPUnsignedCharType : public CPPStandardIntegerType<unsigned char, char>
{

protected:
	virtual CPPValue TypeValue(unsigned char inValue);
	virtual unsigned char ValueType(const CPPValue& inValue);
};
