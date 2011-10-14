#include "CPPBoolType.h"

CPPBoolType::CPPBoolType()
{
}

CPPBoolType::~CPPBoolType(void)
{
}

CPPValue CPPBoolType::OnesComplement(const CPPValue& inValue)
{
	return BoolValue(~(inValue.mBoolValue ? 1:0) == 1 ? true:false);
}

CPPValue CPPBoolType::Not(const CPPValue& inValue)
{
	return BoolValue(!inValue.mBoolValue);
}

CPPValue CPPBoolType::UnaryMinus(const CPPValue& inValue)
{
	return BoolValue(-(inValue.mBoolValue ? 1:0) == 1 ? true:false);
}

CPPValue CPPBoolType::UnaryPlus(const CPPValue& inValue)
{
	return BoolValue(inValue.mBoolValue);
}

CPPValue CPPBoolType::BoolValue(bool inValue)
{
	CPPValue value;
	value.mType = eCPPBool;
	value.mBoolValue = inValue;
	return value;
}

CPPValue CPPBoolType::ToULongLong(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPUnsignedLongLong;
	value.mULongLongValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPValue CPPBoolType::ToLongLong(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPLongLong;
	value.mLongLongValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPValue CPPBoolType::ToULong(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPUnsignedLong;
	value.mULongValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPValue CPPBoolType::ToLong(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPLong;
	value.mLongValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPValue CPPBoolType::ToUnsignedInt(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPInt;
	value.mUIntValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPValue CPPBoolType::ToInt(const CPPValue& inValue)
{
	CPPValue value;
	value.mType = eCPPInt;
	value.mIntValue = inValue.mBoolValue ? 1:0;
	return value;
}

CPPValue CPPBoolType::ToBool(const CPPValue& inValue)
{
	return inValue;
}

CPPValue CPPBoolType::Multiply(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) * AsInt(inRightValue)) == 0 ? false:true);
}

CPPValue CPPBoolType::Divide(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) / AsInt(inRightValue)) == 0 ? false:true);
}

CPPValue CPPBoolType::Modulus(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) % AsInt(inRightValue)) == 0 ? false:true);
}

CPPValue CPPBoolType::Add(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) + AsInt(inRightValue)) == 0 ? false:true);
}

CPPValue CPPBoolType::Subtract(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue) - AsInt(inRightValue)) == 0 ? false:true);
}

int CPPBoolType::AsInt(const CPPValue& inValue)
{
	return inValue.mBoolValue ? 1:0;
}

CPPValue CPPBoolType::LeftByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue)<<inRightValue.mULongLongValue) == 0 ? false:true);
}

CPPValue CPPBoolType::RightByteShift(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue((AsInt(inLeftValue)>>inRightValue.mULongLongValue) == 0 ? false:true);
}

CPPValue CPPBoolType::LessThan(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) < AsInt(inRightValue));
}

CPPValue CPPBoolType::GreaterThan(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) > AsInt(inRightValue));
}

CPPValue CPPBoolType::LessThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) <= AsInt(inRightValue));
}

CPPValue CPPBoolType::GreaterThanOrEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) >= AsInt(inRightValue));
}

CPPValue CPPBoolType::Equal(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) == AsInt(inRightValue));
}

CPPValue CPPBoolType::NotEqual(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(AsInt(inLeftValue) != AsInt(inRightValue));
}

CPPValue CPPBoolType::BitwiseAnd(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(inLeftValue.mBoolValue & inRightValue.mBoolValue);
}

CPPValue CPPBoolType::BitwiseOr(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(inLeftValue.mBoolValue | inRightValue.mBoolValue);
}

CPPValue CPPBoolType::ExclusiveOr(const CPPValue& inLeftValue,const CPPValue& inRightValue)
{
	return BoolValue(inLeftValue.mBoolValue ^ inRightValue.mBoolValue);
}