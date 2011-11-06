#include "CPPULongLongType.h"

CPPPrimitiveValue CPPULongLongType::TypeValue(unsigned long long inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPUnsignedLongLong;
	value.mULongLongValue = inValue;
	return value;
}

unsigned long long CPPULongLongType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mULongLongValue;
}