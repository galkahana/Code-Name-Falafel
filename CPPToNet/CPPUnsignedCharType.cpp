#include "CPPUnsignedCharType.h"

CPPPrimitiveValue CPPUnsignedCharType::TypeValue(unsigned char inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPUnsignedChar;
	value.mUCharValue = inValue;
	return value;
}

unsigned char CPPUnsignedCharType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mUCharValue;
}