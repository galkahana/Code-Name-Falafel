#pragma once

#include "CPPStandardIntegerType.h"
class CPPWCharType : public CPPStandardIntegerType<wchar_t>
{
protected:
	virtual CPPPrimitiveValue TypeValue(wchar_t inValue);
	virtual wchar_t ValueType(const CPPPrimitiveValue& inValue);
};
