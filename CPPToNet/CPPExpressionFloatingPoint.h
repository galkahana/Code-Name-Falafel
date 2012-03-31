#pragma once
#include "CPPExpression.h"

class CPPExpressionFloatingPoint : public CPPExpression
{
public:
	CPPExpressionFloatingPoint(void);
	CPPExpressionFloatingPoint(float inFloatValue);
	CPPExpressionFloatingPoint(double inDoubleValue);
	CPPExpressionFloatingPoint(long double inLongDoubleValue);
	~CPPExpressionFloatingPoint(void);


	enum EFloatingPointType
	{
		eFloatingPointFloat,
		eFloatingPointDouble,
		eFloatingPointLongDouble
	};

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);
	virtual bool IsEqual(CPPExpression* inOther);
	virtual bool IsLess(CPPExpression* inOther);

private:

	EFloatingPointType mType;
	union 
	{
		float mFloatValue;
		double mDoubleValue;
		long double mLongDoubleValue;
	};
};

