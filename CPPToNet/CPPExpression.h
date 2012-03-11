#pragma once

#include "CPPPrimitiveValue.h"
#include "IExpressionEvaluationContext.h"

#include <utility>

using namespace std;

typedef pair<bool,CPPPrimitiveValue> BoolAndCPPPrimitiveValue;

class CPPExpression
{
public:
	enum ECPPExpressionType
	{
		eCPPExpressionOperator,
		eCPPExpressionInteger,
		eCPPExpressionFunctionCall,
		eCPPExpressionVariable
	};

	CPPExpression(ECPPExpressionType inType){Type = inType;}
	virtual ~CPPExpression(void){}

	// the expression type. for verifications
	ECPPExpressionType Type;

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext) = 0;
	// equality checks that the expression looks the same. note that evaluation is the proper way of figuring out
	// if it evaluates to the same result
	virtual bool IsEqual(CPPExpression* inOther) = 0;
	virtual bool IsLess(CPPExpression* inOther) = 0;
};
