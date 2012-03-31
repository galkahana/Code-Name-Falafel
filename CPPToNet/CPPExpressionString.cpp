#include "CPPExpressionString.h"


CPPExpressionString::CPPExpressionString(void) : CPPExpression(CPPExpression::eCPPExpressionString)
{
	mIsWideString = false;
}

CPPExpressionString::CPPExpressionString(const wstring& inWideString) : CPPExpression(CPPExpression::eCPPExpressionString)
{
	mIsWideString = true;
	mWStringValue = inWideString;
}

CPPExpressionString::CPPExpressionString(const string& inString) : CPPExpression(CPPExpression::eCPPExpressionString)
{
	mIsWideString = false;
	mStringValue = inString;
}

CPPExpressionString::~CPPExpressionString(void)
{
}

BoolAndCPPPrimitiveValue CPPExpressionString::Evaluate(IExpressionEvaluationContext* inContext)
{
	// Does not evaluate to a integer primitive. illegal value for this kind of evaluation
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());


}

bool CPPExpressionString::IsEqual(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionString)
		return false;

	CPPExpressionString* otherString = (CPPExpressionString*)inOther;

	if(mIsWideString != otherString->mIsWideString)
		return false;

	return mIsWideString ? 
				(mWStringValue == otherString->mWStringValue) : 
				(mStringValue == otherString->mStringValue);
}

bool CPPExpressionString::IsLess(CPPExpression* inOther)
{
	if(inOther->Type != CPPExpression::eCPPExpressionString)
		return CPPExpression::eCPPExpressionString < inOther->Type;

	CPPExpressionString* otherString = (CPPExpressionString*)inOther;

	if(mIsWideString != otherString->mIsWideString)
		return !mIsWideString;

	return mIsWideString ? 
				(mWStringValue.compare(otherString->mWStringValue) < 0) :
				(mStringValue.compare(otherString->mStringValue) < 0);

}