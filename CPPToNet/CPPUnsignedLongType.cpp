#include "CPPUnsignedLongType.h"

CPPValue CPPUnsignedLongType::TypeValue(unsigned long inValue)
{
	CPPValue value;
	value.mType = eCPPUnsignedLong;
	value.mULongValue = inValue;
	return value;
}

unsigned long CPPUnsignedLongType::ValueType(const CPPValue& inValue)
{
	return inValue.mULongValue;
}