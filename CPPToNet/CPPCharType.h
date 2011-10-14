#pragma once

#include "CPPStandardIntegerType.h"

class CPPCharType : public CPPStandardIntegerType<char>
{
protected:
	virtual CPPValue TypeValue(char inValue);
	virtual char ValueType(const CPPValue& inValue);

};
