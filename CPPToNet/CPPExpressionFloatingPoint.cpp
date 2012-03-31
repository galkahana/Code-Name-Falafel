#include "CPPExpressionFloatingPoint.h"


CPPExpressionFloatingPoint::CPPExpressionFloatingPoint(void) : CPPExpression(eCPPExpressionFloatingPoint)
{
	mType = eFloatingPointDouble;
	mDoubleValue = 0;
}


CPPExpressionFloatingPoint::CPPExpressionFloatingPoint(float inFloatValue): CPPExpression(eCPPExpressionFloatingPoint)
{
	mType = eFloatingPointFloat;
	mFloatValue = inFloatValue;
}

CPPExpressionFloatingPoint::CPPExpressionFloatingPoint(double inDoubleValue): CPPExpression(eCPPExpressionFloatingPoint)
{
	mType = eFloatingPointDouble;
	mDoubleValue = inDoubleValue;
}

CPPExpressionFloatingPoint::CPPExpressionFloatingPoint(long double inLongDoubleValue): CPPExpression(eCPPExpressionFloatingPoint)
{
	mType = eFloatingPointLongDouble;
	mLongDoubleValue = inLongDoubleValue;

}


CPPExpressionFloatingPoint::~CPPExpressionFloatingPoint(void)
{
}


BoolAndCPPPrimitiveValue CPPExpressionFloatingPoint::Evaluate(IExpressionEvaluationContext* inContext)
{
	// Does not evaluate to a integer primitive. illegal value for this kind of evaluation
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());
}

bool CPPExpressionFloatingPoint::IsEqual(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionFloatingPoint)
		return false;

	CPPExpressionFloatingPoint* otherFloat = (CPPExpressionFloatingPoint*)inOther;

	if(mType != otherFloat->mType)
		return false;

	bool result = false;

	switch(mType)
	{
		case eFloatingPointFloat:
			result = (mFloatValue == otherFloat->mFloatValue);
			break;
		case eFloatingPointDouble:
			result = (mDoubleValue == otherFloat->mDoubleValue);
			break;
		case eFloatingPointLongDouble:
			result = (mLongDoubleValue == otherFloat->mLongDoubleValue);
			break;
	}

	return result;
}

bool CPPExpressionFloatingPoint::IsLess(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionFloatingPoint)
		return CPPExpression::eCPPExpressionFloatingPoint < inOther->Type;

	CPPExpressionFloatingPoint* otherFloat = (CPPExpressionFloatingPoint*)inOther;

	if(mType != otherFloat->mType)
		return mType < otherFloat->mType;

	bool result = false;

	switch(mType)
	{
		case eFloatingPointFloat:
			result = (mFloatValue < otherFloat->mFloatValue);
			break;
		case eFloatingPointDouble:
			result = (mDoubleValue < otherFloat->mDoubleValue);
			break;
		case eFloatingPointLongDouble:
			result = (mLongDoubleValue < otherFloat->mLongDoubleValue);
			break;
	}

	return result;
}
