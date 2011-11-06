#pragma once

#include "CPPStandardIntegerType.h"

class CPPLongLongType : public CPPStandardIntegerType<long long>
{
protected:
	virtual CPPPrimitiveValue TypeValue(long long inValue);
	virtual long long ValueType(const CPPPrimitiveValue& inValue);
};
