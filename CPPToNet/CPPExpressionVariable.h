#pragma once

#include "CPPExpression.h"

#include <string>
#include <list>

using namespace std;

typedef list<string> StringList;

class CPPExpressionVariable :  public CPPExpression
{
public:
	CPPExpressionVariable(const string& inVariableName,const StringList& inScopes);
	~CPPExpressionVariable(void);

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);

	string VariableName;
	StringList Scopes;

};
