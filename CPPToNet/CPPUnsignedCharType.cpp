#include "CPPUnsignedCharType.h"

CPPValue CPPUnsignedCharType::TypeValue(unsigned char inValue)
{
	CPPValue value;
	value.mType = eCPPUnsignedChar;
	value.mUCharValue = inValue;
	return value;
}

unsigned char CPPUnsignedCharType::ValueType(const CPPValue& inValue)
{
	return inValue.mUCharValue;
}