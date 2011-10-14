#include "CPPCharType.h"

CPPValue CPPCharType::TypeValue(char inValue)
{
	CPPValue value;
	value.mType = eCPPChar;
	value.mCharValue = inValue;
	return value;
}

char CPPCharType::ValueType(const CPPValue& inValue)
{
	return inValue.mCharValue;
}

