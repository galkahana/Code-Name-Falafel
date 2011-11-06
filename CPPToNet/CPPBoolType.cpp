#include "CPPBoolType.h"

CPPBoolType::CPPBoolType()
{
}

CPPBoolType::~CPPBoolType(void)
{
}

CPPPrimitiveValue CPPBoolType::OnesComplement(const CPPPrimitiveValue& inValue)
{
	return BoolValue(~(inValue.mBoolValue ? 1:0) == 1 ? true:false);
}

CPPPrimitiveValue CPPBoolType::Not(const CPPPrimitiveValue& inValue)
{
	return BoolValue(!inValue.mBoolValue);
}

CPPPrimitiveValue CPPBoolType::UnaryMinus(const CPPPrimitiveValue& inValue)
{
	return BoolValue(-(inValue.mBoolValue ? 1:0) == 1 ? true:false);
}

CPPPrimitiveValue CPPBoolType::UnaryPlus(const CPPPrimitiveValue& inValue)
{
	return BoolValue(inValue.mBoolValue);
}

CPPPrimitiveValue CPPBoolType::BoolValue(bool inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPBool;
	value.mBoolValue = inValue;
	return value;
}

CPPPrimitiveValue CPPBoolType::ToULongLong(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPUnsignedLongLong;
	value.mULongLongValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPPrimitiveValue CPPBoolType::ToLongLong(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPLongLong;
	value.mLongLongValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPPrimitiveValue CPPBoolType::ToULong(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPUnsignedLong;
	value.mULongValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPPrimitiveValue CPPBoolType::ToLong(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPLong;
	value.mLongValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPPrimitiveValue CPPBoolType::ToUnsignedInt(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPInt;
	value.mUIntValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPPrimitiveValue CPPBoolType::ToInt(const CPPPrimitiveValue& inValue)
{
	CPPPrimitiveValue value;
	value.mType = eCPPInt;
	value.mIntValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPPrimitiveValue CPPBoolType::ToBool(const CPPPrimitiveValue& inValue)
{
	return inValue;
}

CPPPrimitiveValue CPPBoolType::Multiply(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) * AsInt(inRightValue)) == 0 ? false:true);
}

CPPPrimitiveValue CPPBoolType::Divide(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) / AsInt(inRightValue)) == 0 ? false:true);
}

CPPPrimitiveValue CPPBoolType::Modulus(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) % AsInt(inRightValue)) == 0 ? false:true);
}

CPPPrimitiveValue CPPBoolType::Add(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) + AsInt(inRightValue)) == 0 ? false:true);
}

CPPPrimitiveValue CPPBoolType::Subtract(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) - AsInt(inRightValue)) == 0 ? false:true);
}

int CPPBoolType::AsInt(const CPPPrimitiveValue& inValue)
{
	return inValue.mBoolValue ? 1:0;
}

CPPPrimitiveValue CPPBoolType::LeftByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue)<<inRightValue.mULongLongValue) == 0 ? false:true);
}

CPPPrimitiveValue CPPBoolType::RightByteShift(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue)>>inRightValue.mULongLongValue) == 0 ? false:true);
}

CPPPrimitiveValue CPPBoolType::LessThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) < AsInt(inRightValue));
}

CPPPrimitiveValue CPPBoolType::GreaterThan(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) > AsInt(inRightValue));
}

CPPPrimitiveValue CPPBoolType::LessThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) <= AsInt(inRightValue));
}

CPPPrimitiveValue CPPBoolType::GreaterThanOrEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) >= AsInt(inRightValue));
}

CPPPrimitiveValue CPPBoolType::Equal(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) == AsInt(inRightValue));
}

CPPPrimitiveValue CPPBoolType::NotEqual(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) != AsInt(inRightValue));
}

CPPPrimitiveValue CPPBoolType::BitwiseAnd(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(inLeftValue.mBoolValue & inRightValue.mBoolValue);
}

CPPPrimitiveValue CPPBoolType::BitwiseOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(inLeftValue.mBoolValue | inRightValue.mBoolValue);
}

CPPPrimitiveValue CPPBoolType::ExclusiveOr(const CPPPrimitiveValue& inLeftValue,const CPPPrimitiveValue& inRightValue)
{
	return BoolValue(inLeftValue.mBoolValue ^ inRightValue.mBoolValue);
}