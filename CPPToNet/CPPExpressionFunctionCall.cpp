#include "CPPExpressionFunctionCall.h"

CPPExpressionFunctionCall::CPPExpressionFunctionCall(const string& inVariableName,const StringList& inScopes,const CPPExpressionList& inParameters):CPPExpression(CPPExpression::eCPPExpressionFunctionCall)
{
	VariableName = inVariableName;
	Scopes = inScopes;
	Parameters = inParameters;
}

CPPExpressionFunctionCall::~CPPExpressionFunctionCall(void)
{
}

BoolAndCPPPrimitiveValue CPPExpressionFunctionCall::Evaluate(IExpressionEvaluationContext* inContext)
{
	// TODO. need to be determined by environment
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());
}
