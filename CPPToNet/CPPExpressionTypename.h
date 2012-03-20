#pragma once

#include "CPPExpression.h"

struct TypedParameter;
class UsedTypeDescriptor;

class CPPExpressionTypename : public CPPExpression
{
public:
	CPPExpressionTypename(void);
	CPPExpressionTypename(TypedParameter* inTypename);
	~CPPExpressionTypename(void);

	UsedTypeDescriptor* GetType();

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);
	virtual bool IsEqual(CPPExpression* inOther);
	virtual bool IsLess(CPPExpression* inOther);

private:
	TypedParameter* mTypename;
};

