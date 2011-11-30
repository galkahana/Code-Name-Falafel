#include "CPPExpressionVariable.h"

CPPExpressionVariable::CPPExpressionVariable(const string& inVariableName,const StringList& inScopes):CPPExpression(CPPExpression::eCPPExpressionVariable)
{
	VariableName = inVariableName;
	Scopes = inScopes;
}

CPPExpressionVariable::~CPPExpressionVariable(void)
{
}

BoolAndCPPPrimitiveValue CPPExpressionVariable::Evaluate(IExpressionEvaluationContext* inContext)
{
	// TODO. need to be determined by environment
	return BoolAndCPPPrimitiveValue(false,CPPPrimitiveValue());
}
