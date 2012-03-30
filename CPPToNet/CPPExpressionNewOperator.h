#pragma once
#include "CPPExpression.h"
#include <list>

struct TypedParameter;

using namespace std;

typedef list<CPPExpression*> CPPExpressionList;

class CPPExpressionNewOperator : public CPPExpression
{
public:
	CPPExpressionNewOperator(TypedParameter* inNewOfType,const CPPExpressionList& inInitializerParameters);
	~CPPExpressionNewOperator(void);

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);
	virtual bool IsEqual(CPPExpression* inOther);
	virtual bool IsLess(CPPExpression* inOther);

	TypedParameter* NewOfType;
	CPPExpressionList InitializerParameters;

};

