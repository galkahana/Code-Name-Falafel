#include "CPPUnsignedIntType.h"

CPPPrimitiveValue CPPUnsignedIntType::TypeValue(unsigned inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPUnsigned;
	value.mUIntValue = inValue;
	return value;
}

unsigned CPPUnsignedIntType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mUIntValue;
}

