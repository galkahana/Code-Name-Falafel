#include "CPPULongLongType.h"

CPPValue CPPULongLongType::TypeValue(unsigned long long inValue)
{
	CPPValue value;
	value.mType = eCPPUnsignedLongLong;
	value.mULongLongValue = inValue;
	return value;
}

unsigned long long CPPULongLongType::ValueType(const CPPValue& inValue)
{
	return inValue.mULongLongValue;
}