#pragma once

#include "ICPPType.h"
#include "CPPValue.h"

class CPPBoolType : public ICPPType
{
public:
	CPPBoolType();
	~CPPBoolType(void);

	// Unary operators implementation
	virtual CPPValue OnesComplement(const CPPValue& inValue);
	virtual CPPValue Not(const CPPValue& inValue);
	virtual CPPValue UnaryMinus(const CPPValue& inValue);
	virtual CPPValue UnaryPlus(const CPPValue& inValue);

	virtual CPPValue ToULongLong(const CPPValue& inValue);
	virtual CPPValue ToLongLong(const CPPValue& inValue);
	virtual CPPValue ToULong(const CPPValue& inValue);
	virtual CPPValue ToLong(const CPPValue& inValue);
	virtual CPPValue ToUnsignedInt(const CPPValue& inValue);
	virtual CPPValue ToInt(const CPPValue& inValue);
	virtual CPPValue ToBool(const CPPValue& inValue);

	virtual CPPValue Multiply(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Divide(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Modulus(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Add(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Subtract(const CPPValue& inLeftValue,const CPPValue& inRightValue);

	virtual CPPValue LeftByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue RightByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue);

	virtual CPPValue LessThan(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue GreaterThan(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue LessThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue GreaterThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue Equal(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue NotEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue);

	virtual CPPValue BitwiseAnd(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue BitwiseOr(const CPPValue& inLeftValue,const CPPValue& inRightValue);
	virtual CPPValue ExclusiveOr(const CPPValue& inLeftValue,const CPPValue& inRightValue);

private:

	CPPValue BoolValue(bool inValue);
	int AsInt(const CPPValue& inValue);
};
