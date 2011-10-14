#include "CPPUnsignedIntType.h"

CPPValue CPPUnsignedIntType::TypeValue(unsigned inValue)
{
	CPPValue value;
	value.mType = eCPPUnsigned;
	value.mUIntValue = inValue;
	return value;
}

unsigned CPPUnsignedIntType::ValueType(const CPPValue& inValue)
{
	return inValue.mUIntValue;
}

