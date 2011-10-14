#pragma once

#include "CPPStandardIntegerType.h"

class CPPUnsignedLongType : public CPPStandardIntegerType<unsigned long, long>
{
protected:

	virtual CPPValue TypeValue(unsigned long inValue);
	virtual unsigned long ValueType(const CPPValue& inValue);
};
