#pragma once

#include "CPPStandardIntegerType.h"
class CPPLongType : public CPPStandardIntegerType<long>
{
protected:
	virtual CPPPrimitiveValue TypeValue(long inValue);
	virtual long ValueType(const CPPPrimitiveValue& inValue);
};
