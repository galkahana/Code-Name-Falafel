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
};
