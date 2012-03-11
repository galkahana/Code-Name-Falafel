#include "CPPExpressionInteger.h"
#include "CPPPrimitiveValue.h"

CPPExpressionInteger::CPPExpressionInteger(void):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mCharValue = 0;
	mType = eCPPChar;
}

CPPExpressionInteger::CPPExpressionInteger(bool inBoolValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mBoolValue = inBoolValue;
	mType = eCPPBool;
}


CPPExpressionInteger::CPPExpressionInteger(char inCharValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mCharValue = inCharValue;
	mType = eCPPChar;
}

CPPExpressionInteger::CPPExpressionInteger(unsigned char inUCharValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mUCharValue = inUCharValue;
	mType = eCPPUnsignedChar;
}

CPPExpressionInteger::CPPExpressionInteger(int inIntValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mIntValue = inIntValue;
	mType = eCPPInt;
}

CPPExpressionInteger::CPPExpressionInteger(unsigned inUIntValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mUIntValue = inUIntValue;
	mType = eCPPUnsigned;
}

CPPExpressionInteger::CPPExpressionInteger(long inLongValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mLongValue = inLongValue;
	mType = eCPPLong;
}

CPPExpressionInteger::CPPExpressionInteger(unsigned long inULongValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mULongValue = inULongValue;
	mType = eCPPUnsignedLong;
}

CPPExpressionInteger::CPPExpressionInteger(long long inLongLongValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mLongLongValue = inLongLongValue;
	mType = eCPPLongLong;
}

CPPExpressionInteger::CPPExpressionInteger(unsigned long long inULongLongValue):CPPExpression(CPPExpression::eCPPExpressionInteger)
{
	mULongLongValue = inULongLongValue;
	mType = eCPPUnsignedLongLong;
}

CPPExpressionInteger::~CPPExpressionInteger(void)
{
}


BoolAndCPPPrimitiveValue CPPExpressionInteger::Evaluate(IExpressionEvaluationContext* inContext)
{
	BoolAndCPPPrimitiveValue result(true,CPPPrimitiveValue());

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


bool CPPExpressionInteger::IsEqual(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionInteger)
		return false;

	CPPExpressionInteger* otherInteger = (CPPExpressionInteger*)inOther;

	if(mType != otherInteger->mType)
		return false;

	bool result = false;

	switch(mType)
	{
		case eCPPBool:
			result = (mBoolValue == otherInteger->mBoolValue);
			break;
		case eCPPChar:
			result = (mCharValue == otherInteger->mCharValue);
			break;
		case eCPPUnsignedChar:
			result = (mUCharValue == otherInteger->mUCharValue);
			break;
		case eCPPInt:
			result = (mIntValue == otherInteger->mIntValue);
			break;
		case eCPPUnsigned:
			result = (mUIntValue == otherInteger->mUIntValue);
			break;
		case eCPPLong:
			result = (mLongValue == otherInteger->mLongValue);
			break;
		case eCPPUnsignedLong:
			result = (mULongValue == otherInteger->mULongValue);
			break;
		case eCPPLongLong:
			result = (mLongLongValue == otherInteger->mLongLongValue);
			break;
		case eCPPUnsignedLongLong:
			result = (mULongLongValue == otherInteger->mULongLongValue);
			break;
	}	

	return result;
}

bool CPPExpressionInteger::IsLess(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionInteger)
		return inOther->Type < CPPExpression::eCPPExpressionInteger;

	CPPExpressionInteger* otherInteger = (CPPExpressionInteger*)inOther;

	if(mType != otherInteger->mType)
		return mType < otherInteger->mType;

	bool result = false;

	switch(mType)
	{
		case eCPPBool:
			result = (mBoolValue < otherInteger->mBoolValue);
			break;
		case eCPPChar:
			result = (mCharValue < otherInteger->mCharValue);
			break;
		case eCPPUnsignedChar:
			result = (mUCharValue < otherInteger->mUCharValue);
			break;
		case eCPPInt:
			result = (mIntValue < otherInteger->mIntValue);
			break;
		case eCPPUnsigned:
			result = (mUIntValue < otherInteger->mUIntValue);
			break;
		case eCPPLong:
			result = (mLongValue < otherInteger->mLongValue);
			break;
		case eCPPUnsignedLong:
			result = (mULongValue < otherInteger->mULongValue);
			break;
		case eCPPLongLong:
			result = (mLongLongValue < otherInteger->mLongLongValue);
			break;
		case eCPPUnsignedLongLong:
			result = (mULongLongValue < otherInteger->mULongLongValue);
			break;
	}	

	return result;
}