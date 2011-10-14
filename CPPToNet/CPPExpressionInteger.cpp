#include "CPPExpressionInteger.h"
#include "CPPValue.h"

CPPExpressionInteger::CPPExpressionInteger(void)
{
	mCharValue = 0;
	mType = eCPPChar;
}

CPPExpressionInteger::CPPExpressionInteger(bool inBoolValue)
{
	mBoolValue = inBoolValue;
	mType = eCPPBool;
}


CPPExpressionInteger::CPPExpressionInteger(char inCharValue)
{
	mCharValue = inCharValue;
	mType = eCPPChar;
}

CPPExpressionInteger::CPPExpressionInteger(unsigned char inUCharValue)
{
	mUCharValue = inUCharValue;
	mType = eCPPUnsignedChar;
}

CPPExpressionInteger::CPPExpressionInteger(int inIntValue)
{
	mIntValue = inIntValue;
	mType = eCPPInt;
}

CPPExpressionInteger::CPPExpressionInteger(unsigned inUIntValue)
{
	mUIntValue = inUIntValue;
	mType = eCPPUnsigned;
}

CPPExpressionInteger::CPPExpressionInteger(long inLongValue)
{
	mLongValue = inLongValue;
	mType = eCPPLong;
}

CPPExpressionInteger::CPPExpressionInteger(unsigned long inULongValue)
{
	mULongValue = inULongValue;
	mType = eCPPUnsignedLong;
}

CPPExpressionInteger::CPPExpressionInteger(long long inLongLongValue)
{
	mLongLongValue = inLongLongValue;
	mType = eCPPLongLong;
}

CPPExpressionInteger::CPPExpressionInteger(unsigned long long inULongLongValue)
{
	mULongLongValue = inULongLongValue;
	mType = eCPPUnsignedLongLong;
}

CPPExpressionInteger::~CPPExpressionInteger(void)
{
}


BoolAndCPPValue CPPExpressionInteger::Evaluate()
{
	BoolAndCPPValue result(true,CPPValue());

	result.second.mType = mType;
	switch(mType)
	{
		case eCPPBool:
			result.second.mBoolValue = mBoolValue;
			break;
		case eCPPChar:
			result.second.mCharValue = mCharValue;
			break;
		case eCPPUnsignedChar:
			result.second.mUCharValue = mUCharValue;
			break;
		case eCPPInt:
			result.second.mIntValue = mIntValue;
			break;
		case eCPPUnsigned:
			result.second.mUIntValue = mUIntValue;
			break;
		case eCPPLong:
			result.second.mLongValue = mLongValue;
			break;
		case eCPPUnsignedLong:
			result.second.mULongValue = mULongValue;
			break;
		case eCPPLongLong:
			result.second.mLongLongValue = mLongLongValue;
			break;
		case eCPPUnsignedLongLong:
			result.second.mULongLongValue = mULongLongValue;
			break;
	}	

	return result;
}
