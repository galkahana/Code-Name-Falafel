#include "CPPLongType.h"

CPPValue CPPLongType::TypeValue(long inValue)
{
	CPPValue value;
	value.mType = eCPPLong;
	value.mLongValue = inValue;
	return value;
}

long CPPLongType::ValueType(const CPPValue& inValue)
{
	return inValue.mLongValue;
}