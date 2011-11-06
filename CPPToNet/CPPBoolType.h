#pragma once

#include "ICPPPrimitiveType.h"
#include "CPPPrimitiveValue.h"

class CPPBoolType : public ICPPPrimitiveType
{
public:
	CPPBoolType();
	~CPPBoolType(void);

	// Unary operators implementation
	virtual CPPPrimitiveValue OnesComplement(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue Not(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue UnaryMinus(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue UnaryPlus(const CPPPrimitiveValue& inValue);

	virtual CPPPrimitiveValue ToULongLong(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToLongLong(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToULong(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToLong(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToUnsignedInt(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToInt(const CPPPrimitiveValue& inValue);
	virtual CPPPrimitiveValue ToBool(const CPPPrimitiveValue& inValue);

	virtual CPPPrimitiveValue Multiply(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Divide(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Modulus(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Add(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Subtract(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);

	virtual CPPPrimitiveValue LeftByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue RightByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);

	virtual CPPPrimitiveValue LessThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue GreaterThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue LessThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue GreaterThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue Equal(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue NotEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);

	virtual CPPPrimitiveValue BitwiseAnd(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue BitwiseOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);
	virtual CPPPrimitiveValue ExclusiveOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue);

private:

	CPPPrimitiveValue BoolValue(bool inValue);
	int AsInt(const CPPPrimitiveValue& inValue);
};
