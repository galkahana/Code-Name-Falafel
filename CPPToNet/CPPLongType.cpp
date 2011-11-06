#include "CPPLongType.h"

CPPPrimitiveValue CPPLongType::TypeValue(long inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPLong;
	value.mLongValue = inValue;
	return value;
}

long CPPLongType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mLongValue;
}