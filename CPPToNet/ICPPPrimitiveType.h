#pragma once

#include "CPPPrimitiveValue.h"

class ICPPPrimitiveType
{
public:

	// unary operators
	virtual CPPPrimitiveValue OnesComplement(const CPPPrimitiveValue& inValue) = 0;
	virtual CPPPrimitiveValue Not(const CPPPrimitiveValue& inValue) = 0;
	virtual CPPPrimitiveValue UnaryMinus(const CPPPrimitiveValue& inValue) = 0;
	virtual CPPPrimitiveValue UnaryPlus(const CPPPrimitiveValue& inValue) = 0;

	// casting to integreal types (promotion purposes)
	virtual CPPPrimitiveValue ToULongLong(const CPPPrimitiveValue& inValue) = 0;
	virtual CPPPrimitiveValue ToLongLong(const CPPPrimitiveValue& inValue) = 0;
	virtual CPPPrimitiveValue ToULong(const CPPPrimitiveValue& inValue) = 0;
	virtual CPPPrimitiveValue ToLong(const CPPPrimitiveValue& inValue) = 0;
	virtual CPPPrimitiveValue ToUnsignedInt(const CPPPrimitiveValue& inValue) = 0;
	virtual CPPPrimitiveValue ToInt(const CPPPrimitiveValue& inValue) = 0;

	// casting to bool, for conditionals evaluations
	virtual CPPPrimitiveValue ToBool(const CPPPrimitiveValue& inValue) = 0;
		
	// binary operators
	virtual CPPPrimitiveValue Multiply(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue Divide(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue Modulus(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue Add(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue Subtract(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;

	// shift operators (assume that right value is always unsigned long long)
	virtual CPPPrimitiveValue LeftByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue RightByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;

	// comparison operators [always return boolean]
	virtual CPPPrimitiveValue LessThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue GreaterThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue LessThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue GreaterThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue Equal(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	virtual CPPPrimitiveValue NotEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;

	// bitwise operators
	virtual CPPPrimitiveValue BitwiseAnd(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) =0;
	virtual CPPPrimitiveValue BitwiseOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) =0;
	virtual CPPPrimitiveValue ExclusiveOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue) = 0;
	
	
};