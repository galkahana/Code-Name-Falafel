#pragma once
#include "CPPExpression.h"
#include "CPPPrimitiveTypes.h"

class CPPExpressionInteger : public CPPExpression
{
public:
	CPPExpressionInteger(void);
	CPPExpressionInteger(bool inBoolValue);
	CPPExpressionInteger(char inCharValue);
	CPPExpressionInteger(unsigned char inUCharValue);
	CPPExpressionInteger(int inIntValue);
	CPPExpressionInteger(unsigned inUIntValue);
	CPPExpressionInteger(long inLongValue);
	CPPExpressionInteger(unsigned long inULongValue);
	CPPExpressionInteger(long long inLongLongValue);
	CPPExpressionInteger(unsigned long long inULongLongValue);
	~CPPExpressionInteger(void);

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);
	virtual bool IsEqual(CPPExpression* inOther);
	virtual bool IsLess(CPPExpression* inOther);

private:


	ECPPPrimitiveType mType;
	union 
	{
		bool mBoolValue;
		char mCharValue;
		unsigned char mUCharValue;
		int mIntValue;
		unsigned mUIntValue;
		long mLongValue;
		unsigned long mULongValue;
		long long mLongLongValue;
		unsigned long long mULongLongValue;
	};
};
