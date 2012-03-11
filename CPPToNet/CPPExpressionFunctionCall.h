#pragma once
#include "CPPExpression.h"

#include <string>
#include <list>

class CPPExpression;

using namespace std;

typedef list<string> StringList;
typedef list<CPPExpression*> CPPExpressionList;

class CPPExpressionFunctionCall :  public CPPExpression
{
public:
	CPPExpressionFunctionCall(const string& inFunctionName,const StringList& inScopes,const CPPExpressionList& inParameters);
	~CPPExpressionFunctionCall(void);

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);
	virtual bool IsEqual(CPPExpression* inOther);
	virtual bool IsLess(CPPExpression* inOther);

	string FunctionName;
	StringList Scopes;
	CPPExpressionList Parameters;

};
