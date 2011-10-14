#pragma once

#include "CPPStandardIntegerType.h"
class CPPLongType : public CPPStandardIntegerType<long>
{
protected:
	virtual CPPValue TypeValue(long inValue);
	virtual long ValueType(const CPPValue& inValue);
};
