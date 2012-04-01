#pragma once

#include "CPPExpression.h"

#include <string>
#include <list>

using namespace std;

typedef list<string> StringList;
class CPPElement;

class CPPExpressionVariable :  public CPPExpression
{
public:
	CPPExpressionVariable(const string& inVariableName,const StringList& inScopes);
	CPPExpressionVariable(CPPElement* inVariableElement);
	~CPPExpressionVariable(void);

	virtual BoolAndCPPPrimitiveValue Evaluate(IExpressionEvaluationContext* inContext);
	virtual bool IsEqual(CPPExpression* inOther);
	virtual bool IsLess(CPPExpression* inOther);

	string VariableName;
	StringList Scopes;
	CPPElement* mVariableElement;

};
