#include "CPPCharType.h"

CPPPrimitiveValue CPPCharType::TypeValue(char inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPChar;
	value.mCharValue = inValue;
	return value;
}

char CPPCharType::ValueType(const CPPPrimitiveValue& inValue)
{
	return inValue.mCharValue;
}

