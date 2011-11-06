#include "CPPIntType.h"

CPPPrimitiveValue CPPIntType::TypeValue(int inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPInt;
	value.mIntValue = inValue;
	return value;
}

int CPPIntType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mIntValue;
}