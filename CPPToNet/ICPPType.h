#pragma once

#include "CPPValue.h"

class ICPPType
{
public:

	// unary operators
	virtual CPPValue OnesComplement(const CPPValue& inValue) = 0;
	virtual CPPValue Not(const CPPValue& inValue) = 0;
	virtual CPPValue UnaryMinus(const CPPValue& inValue) = 0;
	virtual CPPValue UnaryPlus(const CPPValue& inValue) = 0;

	// casting to integreal types (promotion purposes)
	virtual CPPValue ToULongLong(const CPPValue& inValue) = 0;
	virtual CPPValue ToLongLong(const CPPValue& inValue) = 0;
	virtual CPPValue ToULong(const CPPValue& inValue) = 0;
	virtual CPPValue ToLong(const CPPValue& inValue) = 0;
	virtual CPPValue ToUnsignedInt(const CPPValue& inValue) = 0;
	virtual CPPValue ToInt(const CPPValue& inValue) = 0;

	// casting to bool, for conditionals evaluations
	virtual CPPValue ToBool(const CPPValue& inValue) = 0;
		
	// binary operators
	virtual CPPValue Multiply(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue Divide(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue Modulus(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue Add(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue Subtract(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;

	// shift operators (assume that right value is always unsigned long long)
	virtual CPPValue LeftByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue RightByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;

	// comparison operators [always return boolean]
	virtual CPPValue LessThan(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue GreaterThan(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue LessThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue GreaterThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue Equal(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	virtual CPPValue NotEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;

	// bitwise operators
	virtual CPPValue BitwiseAnd(const CPPValue& inLeftValue,const CPPValue& inRightValue) =0;
	virtual CPPValue BitwiseOr(const CPPValue& inLeftValue,const CPPValue& inRightValue) =0;
	virtual CPPValue ExclusiveOr(const CPPValue& inLeftValue,const CPPValue& inRightValue) = 0;
	
	
};