#include "CPPWCharType.h"

CPPPrimitiveValue CPPWCharType::TypeValue(wchar_t inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPWChar;
	value.mWCharValue = inValue;
	return value;
}

wchar_t CPPWCharType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mWCharValue;
}