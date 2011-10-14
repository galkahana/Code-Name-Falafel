#pragma once

#include "CPPStandardIntegerType.h"

class CPPIntType : public CPPStandardIntegerType<int>
{

protected:
	virtual CPPValue TypeValue(int inValue);
	virtual int ValueType(const CPPValue& inValue);
};
