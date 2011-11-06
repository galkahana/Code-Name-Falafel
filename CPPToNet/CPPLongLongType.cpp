#include "CPPLongLongType.h"

CPPPrimitiveValue CPPLongLongType::TypeValue(long long inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPLongLong;
	value.mLongLongValue = inValue;
	return value;
}

long long CPPLongLongType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mLongLongValue;
}