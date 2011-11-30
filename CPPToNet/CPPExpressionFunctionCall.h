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
	CPPExpressionFunctionCall(const string& inVariableName,const StringList& inScopes,const CPPExpressionList& inParameters);
	~CPPExpressionFunctionCall(void);

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);

	string VariableName;
	StringList Scopes;
	CPPExpressionList Parameters;

};
