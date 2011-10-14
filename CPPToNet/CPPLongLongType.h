#pragma once

#include "CPPStandardIntegerType.h"

class CPPLongLongType : public CPPStandardIntegerType<long long>
{
protected:
	virtual CPPValue TypeValue(long long inValue);
	virtual long long ValueType(const CPPValue& inValue);
};
