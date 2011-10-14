#pragma once

#include "CPPStandardIntegerType.h"

class CPPULongLongType : public CPPStandardIntegerType<unsigned long long, long long>
{
protected:
	virtual CPPValue TypeValue(unsigned long long inValue);
	virtual unsigned long long ValueType(const CPPValue& inValue);
};
