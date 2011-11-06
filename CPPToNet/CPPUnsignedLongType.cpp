#include "CPPUnsignedLongType.h"

CPPPrimitiveValue CPPUnsignedLongType::TypeValue(unsigned long inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPUnsignedLong;
	value.mULongValue = inValue;
	return value;
}

unsigned long CPPUnsignedLongType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mULongValue;
}