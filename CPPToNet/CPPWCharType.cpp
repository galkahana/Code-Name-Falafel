#include "CPPWCharType.h"

CPPValue CPPWCharType::TypeValue(wchar_t inValue)
{
	CPPValue value;
	value.mType = eCPPWChar;
	value.mWCharValue = inValue;
	return value;
}

wchar_t CPPWCharType::ValueType(const CPPValue& inValue)
{
	return inValue.mWCharValue;
}