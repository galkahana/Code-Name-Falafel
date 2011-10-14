#include "CPPLongLongType.h"

CPPValue CPPLongLongType::TypeValue(long long inValue)
{
	CPPValue value;
	value.mType = eCPPLongLong;
	value.mLongLongValue = inValue;
	return value;
}

long long CPPLongLongType::ValueType(const CPPValue& inValue)
{
	return inValue.mLongLongValue;
}