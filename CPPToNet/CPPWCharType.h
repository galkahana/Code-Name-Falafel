#pragma once

#include "CPPStandardIntegerType.h"
class CPPWCharType : public CPPStandardIntegerType<wchar_t>
{
protected:
	virtual CPPValue TypeValue(wchar_t inValue);
	virtual wchar_t ValueType(const CPPValue& inValue);
};
