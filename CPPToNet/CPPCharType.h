#pragma once

#include "CPPStandardIntegerType.h"

class CPPCharType : public CPPStandardIntegerType<char>
{
protected:
	virtual CPPPrimitiveValue TypeValue(char inValue);
	virtual char ValueType(const CPPPrimitiveValue& inValue);

};
