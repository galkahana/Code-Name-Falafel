#pragma once

#include "CPPStandardIntegerType.h"

class CPPIntType : public CPPStandardIntegerType<int>
{

protected:
	virtual CPPPrimitiveValue TypeValue(int inValue);
	virtual int ValueType(const CPPPrimitiveValue& inValue);
};
