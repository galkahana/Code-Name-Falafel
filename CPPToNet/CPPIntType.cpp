#include "CPPIntType.h"

CPPValue CPPIntType::TypeValue(int inValue)
{
	CPPValue value;
	value.mType = eCPPInt;
	value.mIntValue = inValue;
	return value;
}

int CPPIntType::ValueType(const CPPValue& inValue)
{
	return inValue.mIntValue;
}