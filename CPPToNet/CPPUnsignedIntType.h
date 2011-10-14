#pragma once

#include "CPPStandardIntegerType.h"

class CPPUnsignedIntType : public CPPStandardIntegerType<unsigned,int>
{
protected:
	virtual CPPValue TypeValue(unsigned inValue);
	virtual unsigned ValueType(const CPPValue& inValue);
};
